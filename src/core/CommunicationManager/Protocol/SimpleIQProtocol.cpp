#include "SimpleIQProtocol.h"
#include <QDateTime>
#include <QRegularExpression>
#include <QMutexLocker>
#include <QUuid>
#include <spdlog/spdlog.h>
#include <QThread>
#include <QCoreApplication>

SimpleIQProtocol::SimpleIQProtocol(QObject* parent)
    : IProtocol(parent),
    m_numericResponseRegex(R"((-?\d+\.?\d*(?:[eE][+-]?\d+)?);)"),  // Sayısal yanıt: "12345;"
    m_textResponseRegex(R"((.*);)")                                 // Metin yanıt: "herhangi bir metin;"
{
    // Komut zaman aşımı zamanlayıcısı
    connect(&m_timeoutTimer, &QTimer::timeout,
            this, &SimpleIQProtocol::checkPendingCommands);
    m_timeoutTimer.start(100); // 100ms'de bir kontrol et

    // Yanıt işleme zamanlayıcısı
    connect(&m_processingTimer, &QTimer::timeout,
            this, &SimpleIQProtocol::processResponseQueue);
    m_processingTimer.start(1); // 5ms'de bir işle
}

SimpleIQProtocol::~SimpleIQProtocol()
{
    m_timeoutTimer.stop();
    m_processingTimer.stop();
}

bool SimpleIQProtocol::configure(const Configuration& config)
{
    m_config = parseConfig(config);
    return true;
}

std::optional<std::shared_ptr<Response>> SimpleIQProtocol::sendCommand(
    const std::shared_ptr<Command>& command, int timeoutMs)
{
    // Komut kimliği oluştur
    QString commandId = command->id().isEmpty() ? generateCommandId() : command->id();
    command->setId(commandId);

    // Komutu kodla
    auto encodedCommand = encodeCommand(command);
    if (!encodedCommand) {
        m_lastError = tr("Komut kodlanamadı");
        emit errorOccurred(m_lastError);
        return std::nullopt;
    }

    // Gönderme callback'i kontrol et
    if (!m_channelSendCallback) {
        m_lastError = tr("Gönderme callback'i ayarlanmamış");
        emit errorOccurred(m_lastError);
        return std::nullopt;
    }

    // Yanıt için bekleyeceğimiz süreyi hesapla
    int actualTimeout = (timeoutMs > 0) ? timeoutMs : m_config.responseTimeoutMs;

    // Debug log
    qDebug() << "Komut gönderiliyor:" << command->toString()
             << "ID:" << commandId
             << "Timeout:" << actualTimeout << "ms";

    // Yanıt bekleme yapısını oluştur ve saklama listesine ekle
    std::shared_ptr<PendingResponse> pendingResponse = std::make_shared<PendingResponse>();

    {
        QMutexLocker locker(&m_pendingResponsesMutex);
        m_pendingResponses[commandId] = pendingResponse;
    }

    // Yanıt callback'i oluştur - şimdi sınıf üyesine erişiyor
    auto responseCallback = [pendingResponse](std::shared_ptr<Response> resp) {
        pendingResponse->response = resp;
        pendingResponse->received.store(true);
    };

    // Bekleyen komutlar listesine ekle
    {
        QMutexLocker locker(&m_pendingCommandsMutex);
        m_pendingCommands[commandId] = {
            command,
            responseCallback,
            QDateTime::currentDateTime(),
            actualTimeout,
            0, // retryCount
            false // processed değerini false olarak başlat
        };
    }

    // Komutu gönder
    if (!m_channelSendCallback(*encodedCommand)) {
        m_lastError = tr("Komut gönderilemedi");
        emit errorOccurred(m_lastError);

        // Bekleyen komutlar ve yanıtlar listesinden çıkar
        {
            QMutexLocker locker(&m_pendingCommandsMutex);
            m_pendingCommands.remove(commandId);
        }
        {
            QMutexLocker locker(&m_pendingResponsesMutex);
            m_pendingResponses.remove(commandId);
        }

        return std::nullopt;
    }

    // Komut gönderildi sinyali yayınla
    emit commandSent(commandId, *encodedCommand);

    // Yanıt gelene kadar veya timeout olana kadar bekle
    QElapsedTimer timer;
    timer.start();

    // Qt event döngüsünü işlerken bekle
    while (!pendingResponse->received.load() && timer.elapsed() < actualTimeout) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 5);
        QThread::msleep(1); // CPU kullanımını azalt
    }

    // Bekleyen yanıtı al ve listeden çıkar
    std::shared_ptr<Response> response;
    {
        QMutexLocker locker(&m_pendingResponsesMutex);
        if (m_pendingResponses.contains(commandId)) {
            if (pendingResponse->received.load()) {
                response = pendingResponse->response;
            }
            m_pendingResponses.remove(commandId);
        }
    }

    // Yanıt alındı mı kontrol et
    if (response) {
        return response;
    }

    // Timeout durumunda, bekleyen komutlar listesinden çıkar
    {
        QMutexLocker locker(&m_pendingCommandsMutex);
        if (m_pendingCommands.contains(commandId)) {
            m_pendingCommands.remove(commandId);
        }
    }

    m_lastError = tr("Yanıt zaman aşımı");
    emit errorOccurred(m_lastError);

    // Zaman aşımı için hata yanıtı oluştur
    std::shared_ptr<Response> errorResponse = createErrorResponse(
        commandId, tr("Yanıt zaman aşımı"));

    // Zaman aşımı durumunda bile bir yanıt sinyali yayınla
    emit responseReceived(commandId, errorResponse, ResponseStatus::Timeout);

    return std::nullopt;
}

bool SimpleIQProtocol::sendCommandAsync(
    const std::shared_ptr<Command>& command,
    std::function<void(std::shared_ptr<Response>)> callback,
    int timeoutMs)
{
    // Komut kimliği oluştur
    QString commandId = command->id().isEmpty() ? generateCommandId() : command->id();
    command->setId(commandId);

    // Komutu kodla
    auto encodedCommand = encodeCommand(command);
    if (!encodedCommand) {
        m_lastError = tr("Komut kodlanamadı");
        emit errorOccurred(m_lastError);
        return false;
    }

    // Gönderme callback'i kontrol et
    if (!m_channelSendCallback) {
        m_lastError = tr("Gönderme callback'i ayarlanmamış");
        emit errorOccurred(m_lastError);
        return false;
    }

    // Bekleyen komutlar listesine ekle
    int actualTimeout = (timeoutMs > 0) ? timeoutMs : m_config.responseTimeoutMs;

    {
        QMutexLocker locker(&m_pendingCommandsMutex);
        m_pendingCommands[commandId] = {
            command,
            callback,
            QDateTime::currentDateTime(),
            actualTimeout,
            0 // retryCount
        };
    }

    // Komutu gönder
    if (!m_channelSendCallback(*encodedCommand)) {
        m_lastError = tr("Komut gönderilemedi");
        emit errorOccurred(m_lastError);

        // Bekleyen komutlar listesinden çıkar
        QMutexLocker locker(&m_pendingCommandsMutex);
        m_pendingCommands.remove(commandId);

        return false;
    }

    // Komut gönderildi sinyali yayınla
    emit commandSent(commandId, *encodedCommand);

    return true;
}

std::optional<std::shared_ptr<Response>> SimpleIQProtocol::parseResponse(
    const QByteArray& data)
{
    QString responseStr = QString::fromUtf8(data).trimmed();

    // Yanıt komut kim olduğunu bilmiyoruz - gelen yanıt önceki gönderilen komutla ilişkilendirilecek
    // Önce sayısal yanıt formatını dene
    // QRegularExpressionMatch numericMatch = m_numericResponseRegex.match(responseStr);
    // if (numericMatch.hasMatch()) {
    //     QString valueStr = numericMatch.captured(1);
    //     double value = valueStr.toDouble();

    //     // Sayısal yanıt oluştur
    //     std::shared_ptr<Response> response = std::make_shared<Response>(
    //         "", value, "", -1, ResponseType::Data
    //         );
    //     response->setRawResponse(responseStr);
    //     return response;
    // }

    // Sayısal format değilse, metin yanıt formatını dene
    QRegularExpressionMatch textMatch = m_textResponseRegex.match(responseStr);
    if (textMatch.hasMatch()) {
        QString textValue = textMatch.captured(1);

        // Metin yanıtı oluştur
        std::shared_ptr<Response> response = std::make_shared<Response>(
            "", 0.0, "", -1, ResponseType::Text
            );
        // Metin değerini bir parametre olarak ekle
        response->addParameter("text", textValue);
        response->setRawResponse(responseStr);
        return response;
    }

    // Hiçbir format uymuyorsa
    return std::nullopt;
}

std::optional<QByteArray> SimpleIQProtocol::encodeCommand(
    const std::shared_ptr<Command>& command)
{
    // SimpleIQ formatı için komut formatı değişkeni
    QString formattedCommand;

    // Komut tipine göre format belirleme
    switch (command->type()) {
    case CommandType::Write:
        // Yazma komutu: İndeks varsa KOMUT[İNDEKS]=DEĞER<CR>, yoksa KOMUT=DEĞER<CR>
        if (command->index() >= 0) {
            // İndeksli yazma: KOMUT[İNDEKS]=DEĞER<CR>
            // Örnek: MO[10]=1<CR>
            formattedCommand = QString("%1[%2]=%3%4")
                                   .arg(command->command())
                                   .arg(command->index())
                                   .arg(command->value())
                                   .arg(m_config.terminationChar);
        } else {
            // İndekssiz yazma: KOMUT=DEĞER<CR>
            // Örnek: MO=1<CR>
            formattedCommand = QString("%1=%2%3")
                                   .arg(command->command())
                                   .arg(command->value())
                                   .arg(m_config.terminationChar);
        }
        break;

    case CommandType::Read:
    case CommandType::Query:
        // Sorgu komutu: İndeks varsa KOMUT[İNDEKS]<CR>, yoksa KOMUT<CR>
        if (command->index() >= 0) {
            // İndeksli sorgu: KOMUT[İNDEKS]<CR>
            // Örnek: MO[30]<CR>
            formattedCommand = QString("%1[%2]%3")
                                   .arg(command->command())
                                   .arg(command->index())
                                   .arg(m_config.terminationChar);
        } else {
            // İndekssiz sorgu: KOMUT<CR>
            // Örnek: MO<CR>
            formattedCommand = QString("%1%2")
                                   .arg(command->command())
                                   .arg(m_config.terminationChar);
        }
        break;

    case CommandType::Control:
    case CommandType::Motion:
    default:
        // Diğer komut tipleri için de benzer şekilde sorgu formatını kullan
        if (command->index() >= 0) {
            formattedCommand = QString("%1[%2]%3")
            .arg(command->command())
                .arg(command->index())
                .arg(m_config.terminationChar);
        } else {
            formattedCommand = QString("%1%2")
            .arg(command->command())
                .arg(m_config.terminationChar);
        }
        break;
    }

    return formattedCommand.toUtf8();
}

QString SimpleIQProtocol::protocolName() const
{
    return "SimpleIQ";
}

QString SimpleIQProtocol::protocolVersion() const
{
    return "1.0";
}

QString SimpleIQProtocol::lastError() const
{
    return m_lastError;
}

void SimpleIQProtocol::setRawDataHandler(std::function<void(const QByteArray&)> callback)
{
    m_rawDataHandler = callback;
}

void SimpleIQProtocol::processRawData(const QByteArray& data)
{
    // Ham veri alındı sinyali yayınla
    emit rawDataReceived(data);

    // Ham veri işleyici varsa çağır
    if (m_rawDataHandler) {
        m_rawDataHandler(data);
    }

    // DEBUG: Gelen veriyi logla
    qDebug() << "Alınan ham veri:" << data;

    // Yanıt kuyruğuna ekle - kilit kullanarak
    QMutexLocker queueLocker(&m_responseQueueMutex);
    m_responseQueue.push(data);
    queueLocker.unlock();

    // Yanıt işleme mekanizmasını hemen tetikle - bu kritik!
    QMetaObject::invokeMethod(this, &SimpleIQProtocol::processResponseQueue, Qt::QueuedConnection);
}

void SimpleIQProtocol::setChannelSendCallback(std::function<bool(const QByteArray&)> sendCallback)
{
    m_channelSendCallback = sendCallback;
}

void SimpleIQProtocol::checkPendingCommands()
{
    if (m_pendingCommands.isEmpty()) {
        return;
    }
    QMutexLocker locker(&m_pendingCommandsMutex);
    QDateTime now = QDateTime::currentDateTime();

    QStringList timedOutCommands;

    // Tüm bekleyen komutları kontrol et
    for (auto it = m_pendingCommands.begin(); it != m_pendingCommands.end(); ++it) {
        QString commandId = it.key();
        PendingCommand& pendingCmd = it.value();

        // İşlenen komutları kontrol etmeye gerek yok (bunlar zaten silinmeli)
        if (pendingCmd.processed) {
            timedOutCommands.append(commandId); // İşlenmiş ancak listede kalmış komutları temizle
            continue;
        }

        // Zaman aşımı kontrolü
        int elapsedMs = pendingCmd.timestamp.msecsTo(now);
        if (elapsedMs > pendingCmd.timeoutMs * 1.5) { // %50 ek tolerans
            timedOutCommands.append(commandId);

            // Debug log ekle
            qDebug() << "Komut zaman aşımı:"
                     << pendingCmd.command->command()
                     << "Index:" << pendingCmd.command->index()
                     << "ID:" << commandId
                     << "Geçen süre:" << elapsedMs << "ms";

            // Hata yanıtı oluştur
            std::shared_ptr<Response> errorResponse = createErrorResponse(
                commandId, tr("Yanıt zaman aşımı"));

            // Callback'i çağırmadan önce işlenmiş olarak işaretle
            pendingCmd.processed = true;

            // Callback'i mutex'i serbest bırakarak çağır
            locker.unlock();

            if (pendingCmd.callback) {
                pendingCmd.callback(errorResponse);
            }

            // Yanıt alındı sinyali yayınla
            emit responseReceived(commandId, errorResponse, ResponseStatus::Timeout);

            // Mutex'i tekrar kilitle
            locker.relock();
        }
    }

    // Zaman aşımına uğrayan veya işlenmiş komutları temizle
    for (const QString& id : timedOutCommands) {
        m_pendingCommands.remove(id);
    }
}

void SimpleIQProtocol::processResponseQueue()
{
    QByteArray responseData;

    // Yanıt kuyruğundan bir yanıt al
    {
        QMutexLocker locker(&m_responseQueueMutex);
        if (m_responseQueue.empty()) {
            return;
        }

        responseData = m_responseQueue.front();
        m_responseQueue.pop();
    }

    // Yanıtı ayrıştır
    auto parsedResponse = parseResponse(responseData);
    if (!parsedResponse) {
        qDebug() << "Yanıt ayrıştırılamadı:" << responseData;
        return;
    }

    std::shared_ptr<Response> response = parsedResponse.value();

    // Bekleyen komutları kilitle
    QMutexLocker locker(&m_pendingCommandsMutex);

    if (m_pendingCommands.isEmpty()) {
        qDebug() << "Bekleyen komut yok, yanıt işlenemiyor:" << responseData;
        return;
    }

    // İlk bekleyen ve henüz işlenmemiş komutu bul
    QString commandId;
    PendingCommand pendingCmd;
    bool hasPendingCommand = false;

    for (auto it = m_pendingCommands.begin(); it != m_pendingCommands.end(); ++it) {
        // Eğer komut zaten işlendiyse atla
        if (it.value().processed) {
            continue;
        }

        commandId = it.key();
        pendingCmd = it.value();
        hasPendingCommand = true;

        // Bu komutu işleniyor olarak işaretle
        m_pendingCommands[commandId].processed = true;
        break;
    }

    if (!hasPendingCommand) {
        qDebug() << "İşlenmemiş bekleyen komut yok, yanıt işlenemiyor:" << responseData;
        return;
    }

    // İlişkilendirmek için komutun bilgilerini yanıta ekle
    response->setCommandId(commandId);
    response->setCommand(pendingCmd.command->command());
    response->setIndex(pendingCmd.command->index());

    // Debug log ekle
    qDebug() << "Yanıt eşleştiriliyor - Komut:" << pendingCmd.command->command()
             << "Index:" << pendingCmd.command->index()
             << "ID:" << commandId;

    // Mutex'i serbest bırak - callback içinde tekrar lock olmayı önlemek için
    locker.unlock();

    // Callback varsa çağır
    if (pendingCmd.callback) {
        pendingCmd.callback(response);
    }

    // Yanıt alındı sinyali yayınla
    emit responseReceived(commandId, response, response->isSuccess() ?
                                                   ResponseStatus::Success : ResponseStatus::Error);

    // Yanıt işleme tamamlandı log'u
    qDebug() << "Yanıt işleme tamamlandı - Komut:" << pendingCmd.command->command()
             << "Index:" << pendingCmd.command->index();

    // ÖNEMLİ: Komutu bekleyen komutlar listesinden çıkar
    {
        QMutexLocker removeLocker(&m_pendingCommandsMutex);
        m_pendingCommands.remove(commandId);
    }
}

QString SimpleIQProtocol::generateCommandId()
{
    // Benzersiz komut kimliği oluştur
    m_commandCounter++;
    return QString("cmd_%1").arg(m_commandCounter);
}

void SimpleIQProtocol::handleCommandTimeout(const QString& commandId)
{
    QMutexLocker locker(&m_pendingCommandsMutex);

    // Bekleyen komut listesinde bu kimlik var mı kontrol et
    if (!m_pendingCommands.contains(commandId)) {
        return;
    }

    PendingCommand pendingCmd = m_pendingCommands[commandId];

    // Hata yanıtı oluştur
    std::shared_ptr<Response> errorResponse = createErrorResponse(
        commandId, tr("Yanıt zaman aşımı")
        );

    // Callback varsa çağır
    if (pendingCmd.callback) {
        pendingCmd.callback(errorResponse);
    }

    // Yanıt alındı sinyali yayınla (hata)
    emit responseReceived(commandId, errorResponse, ResponseStatus::Timeout);

    // Bekleyen komutlar listesinden çıkar
    m_pendingCommands.remove(commandId);
}

void SimpleIQProtocol::handleResponseForCommand(const QString& commandId, std::shared_ptr<Response> response)
{
    QMutexLocker locker(&m_pendingCommandsMutex);

    // Bekleyen komut listesinde bu kimlik var mı kontrol et
    if (!m_pendingCommands.contains(commandId)) {
        return;
    }

    PendingCommand pendingCmd = m_pendingCommands[commandId];

    // Callback varsa çağır
    if (pendingCmd.callback) {
        pendingCmd.callback(response);
    }

    // Yanıt alındı sinyali yayınla
    emit responseReceived(commandId, response, response->isSuccess() ?
                                                   ResponseStatus::Success : ResponseStatus::Error);

    // Bekleyen komutlar listesinden çıkar
    m_pendingCommands.remove(commandId);
}

SimpleIQProtocol::SimpleIQConfiguration SimpleIQProtocol::parseConfig(const Configuration& config)
{
    SimpleIQConfiguration result;

    QVariantMap params = config.parameters;

    if (params.contains("commandDelayMs"))
        result.commandDelayMs = params["commandDelayMs"].toInt();

    if (params.contains("responseTimeoutMs"))
        result.responseTimeoutMs = params["responseTimeoutMs"].toInt();

    if (params.contains("maxRetryCount"))
        result.maxRetryCount = params["maxRetryCount"].toInt();

    if (params.contains("autoRetry"))
        result.autoRetry = params["autoRetry"].toBool();

    if (params.contains("terminationChar"))
        result.terminationChar = params["terminationChar"].toString();

    if (params.contains("trimResponse"))
        result.trimResponse = params["trimResponse"].toBool();

    return result;
}

std::pair<QString, int> SimpleIQProtocol::extractCommandAndIndex(const QString& commandPart) const
{
    QRegularExpression regex(R"(([A-Za-z]{2,3})(?:\[(\d+)\])?)");
    QRegularExpressionMatch match = regex.match(commandPart);

    if (match.hasMatch()) {
        QString command = match.captured(1);
        int index = -1;

        if (match.lastCapturedIndex() > 1 && !match.captured(2).isEmpty()) {
            index = match.captured(2).toInt();
        }

        return {command, index};
    }

    return {commandPart, -1};
}

std::shared_ptr<Response> SimpleIQProtocol::createErrorResponse(const QString& commandId, const QString& error)
{
    std::shared_ptr<Response> response = std::make_shared<Response>(commandId, "");
    response->setType(ResponseType::Error);
    response->setErrorMessage(error);
    return response;
}
