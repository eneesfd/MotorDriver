#include "CommunicationManager.h"
#include "Channels/SerialPortChannel.h"
#include "Protocol/SimpleIQProtocol.h"
#include <spdlog/spdlog.h>
#include <QMutexLocker>
#include <QCoreApplication>
#include <QUuid>

// Singleton instance
CommunicationManager* CommunicationManager::s_instance = nullptr;

CommunicationManager::CommunicationManager(std::shared_ptr<spdlog::logger> logger, QObject* parent)
    : QObject(parent),
    m_logger(logger),
    m_messageQueue(new MessageQueue(this))
{
    // Zamanlayıcıları kur
    ICommunicationChannel::connect(&m_statusPollingTimer, &QTimer::timeout,
            this, &CommunicationManager::pollStatus);

    ICommunicationChannel::connect(&m_reconnectTimer, &QTimer::timeout,
            this, &CommunicationManager::attemptReconnect);

    ICommunicationChannel::connect(&m_timeoutTimer, &QTimer::timeout,
            this, &CommunicationManager::checkCommandTimeouts);

    // Komut zaman aşımlarını kontrol et
    m_timeoutTimer.start(500);

    // Mesaj kuyruğunu yapılandır
    MessageQueue::Configuration mqConfig;
    mqConfig.maxQueueSize = 2000;
    mqConfig.processingIntervalMs = 5;
    mqConfig.strategy = MessageQueue::ProcessingStrategy::Immediate;

    m_messageQueue->configure(mqConfig);

    // Mesaj işlemeyi başlat
    m_messageQueue->startProcessing([this](const QByteArray& data) {
        processData(data);
    });

    updateState(CommunicationState::Disconnected);
}

CommunicationManager::~CommunicationManager()
{
    // Zamanlayıcıları durdur
    m_statusPollingTimer.stop();
    m_reconnectTimer.stop();
    m_timeoutTimer.stop();

    // Bağlantıyı kapat
    if (m_channel && isConnected()) {
        disconnect();
    }

    // Bekleyen komutları temizle
    clearPendingCommands("Manager destroying");

    // Singleton instance'ı temizle
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

bool CommunicationManager::configure(const Configuration& config)
{
    m_config = config;

    try {
        // Haberleşme kanalını oluştur
        m_channel = createChannel(config.channelType);
        if (!m_channel) {
            m_lastError = tr("Desteklenmeyen kanal türü: %1").arg(config.channelType);
            logMessage(m_lastError, spdlog::level::err);
            emit errorOccurred(m_lastError);
            return false;
        }

        // Protokolü oluştur
        m_protocol = createProtocol(config.protocolType);
        if (!m_protocol) {
            m_lastError = tr("Desteklenmeyen protokol türü: %1").arg(config.protocolType);
            logMessage(m_lastError, spdlog::level::err);
            emit errorOccurred(m_lastError);
            return false;
        }

        // Kanal ve protokolü yapılandır
        ICommunicationChannel::Configuration channelConfig;
        channelConfig.parameters = config.channelParams;

        if (!m_channel->configure(channelConfig)) {
            m_lastError = tr("Kanal yapılandırılamadı: %1").arg(m_channel->lastError());
            logMessage(m_lastError, spdlog::level::err);
            emit errorOccurred(m_lastError);
            return false;
        }

        IProtocol::Configuration protocolConfig;
        protocolConfig.parameters = config.protocolParams;

        if (!m_protocol->configure(protocolConfig)) {
            m_lastError = tr("Protokol yapılandırılamadı: %1").arg(m_protocol->lastError());
            logMessage(m_lastError, spdlog::level::err);
            emit errorOccurred(m_lastError);
            return false;
        }

        // Zaman aralıklarını ayarla
        if (m_config.statusPollingIntervalMs > 0) {
            m_statusPollingTimer.setInterval(m_config.statusPollingIntervalMs);
        }

        if (m_config.autoReconnectIntervalMs > 0) {
            m_reconnectTimer.setInterval(m_config.autoReconnectIntervalMs);
        }

        // Yeniden bağlanma için son kanal bilgilerini kaydet
        m_lastChannelType = config.channelType;
        m_lastChannelParams = config.channelParams;

        // Bağlantılar
        ICommunicationChannel::connect(m_channel.data(), &ICommunicationChannel::stateChanged,
                this, &CommunicationManager::handleChannelStateChanged);

        ICommunicationChannel::connect(m_channel.data(), &ICommunicationChannel::dataReceived,
                this, &CommunicationManager::handleChannelDataReceived);

        ICommunicationChannel::connect(m_channel.data(), &ICommunicationChannel::errorOccurred,
                this, &CommunicationManager::handleChannelErrorOccurred);

        ICommunicationChannel::connect(m_protocol.get(), &IProtocol::responseReceived,
                this, &CommunicationManager::handleProtocolResponseReceived);

        ICommunicationChannel::connect(m_protocol.get(), &IProtocol::errorOccurred,
                this, &CommunicationManager::handleProtocolErrorOccurred);

        // Protokole ham veri işleyici ve gönderme callback'i ayarla
        m_protocol->setRawDataHandler([this](const QByteArray& data) {
            // Ham veri alındı sinyali yayınla
            emit rawDataReceived(data);
        });

        if (auto simpleIQProtocol = qobject_cast<SimpleIQProtocol*>(m_protocol.get())) {
            simpleIQProtocol->setChannelSendCallback([this](const QByteArray& data) {
                if (!m_channel || !m_channel->isConnected()) {
                    return false;
                }

                // Veriyi gönder
                qint64 bytesWritten = m_channel->write(data);

                // Trafik logla
                if (m_trafficLoggingEnabled) {
                    logTraffic(true, data);
                }

                // Ham veri gönderildi sinyali yayınla
                if (bytesWritten > 0) {
                    emit rawDataSent(data);
                }

                return bytesWritten > 0;
            });
        }

        // Otomatik durum sorgulama ve yeniden bağlanma
        m_statusPollingEnabled = m_config.statusPollingIntervalMs > 0;
        m_autoReconnectEnabled = m_config.autoReconnect;
        m_trafficLoggingEnabled = m_config.logTraffic;

        logMessage(tr("CommunicationManager yapılandırıldı - Kanal: %1, Protokol: %2")
                       .arg(config.channelType).arg(config.protocolType),
                   spdlog::level::info);

        return true;
    } catch (const std::exception& e) {
        m_lastError = tr("Yapılandırma hatası: %1").arg(e.what());
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);
        return false;
    }
}

bool CommunicationManager::configure(const QString& channelType, const QString& protocolType,
                                     const QVariantMap& channelParams, const QVariantMap& protocolParams)
{
    Configuration config;
    config.channelType = channelType;
    config.protocolType = protocolType;
    config.channelParams = channelParams;
    config.protocolParams = protocolParams;

    return configure(config);
}

bool CommunicationManager::connect()
{
    if (!m_channel) {
        m_lastError = tr("Kanal yapılandırılmamış");
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);
        return false;
    }

    // Zaten bağlıysa başarılı döndür
    if (isConnected()) {
        return true;
    }

    // Bağlanıyor durumuna geç
    updateState(CommunicationState::Connecting);

    // Kanalı aç
    if (!m_channel->open()) {
        m_lastError = tr("Bağlantı açılamadı: %1").arg(m_channel->lastError());
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);

        // Hata durumuna geç
        updateState(CommunicationState::Error);

        // Otomatik yeniden bağlanma aktifse zamanlayıcıyı başlat
        if (m_autoReconnectEnabled) {
            m_reconnectTimer.start();
        }

        return false;
    }

    // Bağlantı başarılı, durum sorgulamayı başlat
    if (m_statusPollingEnabled) {
        m_statusPollingTimer.start();

        // İlk durum sorgulamasını hemen yap
        QTimer::singleShot(100, this, &CommunicationManager::pollStatus);
    }

    logMessage(tr("Motor sürücüye bağlandı"), spdlog::level::info);

    return true;
}

bool CommunicationManager::disconnect()
{
    if (!m_channel) {
        return false;
    }

    // Bağlı değilse başarılı döndür
    if (!isConnected()) {
        return true;
    }

    // Bağlantı kesiliyor durumuna geç
    updateState(CommunicationState::Disconnecting);

    // Zamanlayıcıları durdur
    m_statusPollingTimer.stop();
    m_reconnectTimer.stop();

    // Bekleyen komutları temizle
    clearPendingCommands("Disconnecting");

    // Kanalı kapat
    if (!m_channel->close()) {
        m_lastError = tr("Bağlantı kapatılamadı: %1").arg(m_channel->lastError());
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);

        // Hata durumuna geç
        updateState(CommunicationState::Error);

        return false;
    }

    logMessage(tr("Motor sürücüyle bağlantı kesildi"), spdlog::level::info);

    return true;
}

bool CommunicationManager::isConnected() const
{
    return m_channel && m_channel->state() == ChannelState::Connected;
}

CommunicationManager::CommunicationState CommunicationManager::state() const
{
    return m_state;
}

bool CommunicationManager::createDefaultChannel()
{
    // Henüz bir kanal oluşturulmamışsa, varsayılan bir SerialPort kanalı oluştur
    if (!m_channel) {
        m_channel = ICommunicationChannelPtr(new SerialPortChannel(this));
        return true;
    }
    return false; // Zaten bir kanal var
}

QVariantList CommunicationManager::availableChannels() const
{
    // Eğer kanal henüz oluşturulmadıysa, oluştur
    if (!m_channel) {
        const_cast<CommunicationManager*>(this)->createDefaultChannel();
    }

    if (!m_channel) {
        return QVariantList(); // Hala oluşturulamadıysa boş liste döndür
    }

    return m_channel->availableChannels();
}
QString CommunicationManager::lastError() const
{
    return m_lastError;
}

std::optional<std::shared_ptr<Response>> CommunicationManager::sendCommand(
    const std::shared_ptr<Command>& command, int timeoutMs)
{
    if (!m_protocol) {
        m_lastError = tr("Protokol yapılandırılmamış");
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);
        return std::nullopt;
    }

    if (!isConnected()) {
        m_lastError = tr("Motor sürücüye bağlı değil");
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);
        return std::nullopt;
    }

    // Timeout değerini ayarla
    int actualTimeout = (timeoutMs > 0) ? timeoutMs : getDefaultCommandTimeout();

    // Komutu gönder ve yanıtı bekle
    auto response = m_protocol->sendCommand(command, actualTimeout);

    // Yanıt kontrolü
    if (!response || !(*response)->isSuccess()) {
        m_lastError = response ? (*response)->errorMessage() : tr("Komut başarısız");
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);
        return std::nullopt;
    }

    // Motor durumunu güncelle
    updateMotorStatus(*response);

    return response;
}

bool CommunicationManager::sendCommandAsync(
    const std::shared_ptr<Command>& command, CommandCallback callback, int timeoutMs)
{
    if (!m_protocol) {
        m_lastError = tr("Protokol yapılandırılmamış");
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);
        return false;
    }

    if (!isConnected()) {
        m_lastError = tr("Motor sürücüye bağlı değil");
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);
        return false;
    }

    // Timeout değerini ayarla
    int actualTimeout = (timeoutMs > 0) ? timeoutMs : getDefaultCommandTimeout();

    // Komutu bekleyen komutlar listesine ekle
    QString commandId = command->id().isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces) : command->id();
    command->setId(commandId);

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

    // Komutu asenkron olarak gönder
    return m_protocol->sendCommandAsync(command, [this, commandId](std::shared_ptr<Response> response) {
        handleCommandComplete(commandId, response);
    }, actualTimeout);
}

bool CommunicationManager::processData(const QByteArray& data)
{
    if (!m_protocol) {
        return false;
    }

    try {
        // Trafik logla
        if (m_trafficLoggingEnabled) {
            logTraffic(false, data);
        }

        // Protokole veriyi gönder
        if (auto simpleIQProtocol = qobject_cast<SimpleIQProtocol*>(m_protocol.get())) {
            simpleIQProtocol->processRawData(data);
        }

        return true;
    } catch (const std::exception& e) {
        m_lastError = tr("Veri işleme hatası: %1").arg(e.what());
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);
        return false;
    }
}

std::optional<double> CommunicationManager::readParameter(int parameterId)
{
    if (!isConnected()) {
        m_lastError = tr("Motor sürücüye bağlı değil");
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);
        return std::nullopt;
    }

    // Parametre okuma komutu oluştur
    auto command = Command::createParameterCommand(parameterId, 0);
    command->setType(CommandType::Read);

    // Komutu gönder ve yanıtı bekle
    auto response = sendCommand(command);

    // Yanıt kontrolü
    if (!response) {
        return std::nullopt;
    }

    // Parametre değerini döndür
    return (*response)->doubleValue();
}

bool CommunicationManager::writeParameter(int parameterId, double value)
{
    if (!isConnected()) {
        m_lastError = tr("Motor sürücüye bağlı değil");
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);
        return false;
    }

    // Parametre yazma komutu oluştur
    auto command = Command::createParameterCommand(parameterId, value);

    // Komutu gönder ve yanıtı bekle
    auto response = sendCommand(command);

    // Yanıt kontrolü
    return response.has_value();
}

bool CommunicationManager::startMotion(double targetPosition, double speed)
{
    if (!isConnected()) {
        m_lastError = tr("Motor sürücüye bağlı değil");
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);
        return false;
    }

    // Hareket komutu oluştur
    auto command = Command::createMotionCommand(targetPosition, speed);

    // Komutu gönder ve yanıtı bekle
    auto response = sendCommand(command);

    // Yanıt kontrolü
    return response.has_value();
}

bool CommunicationManager::stopMotion(bool emergency)
{
    if (!isConnected()) {
        m_lastError = tr("Motor sürücüye bağlı değil");
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);
        return false;
    }

    // Durdurma komutu oluştur
    auto command = Command::createStopCommand(emergency);

    // Komutu gönder ve yanıtı bekle
    auto response = sendCommand(command);

    // Yanıt kontrolü
    return response.has_value();
}

bool CommunicationManager::homeMotor()
{
    if (!isConnected()) {
        m_lastError = tr("Motor sürücüye bağlı değil");
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);
        return false;
    }

    // Home komutu oluştur
    auto command = Command::createHomeCommand();

    // Komutu gönder ve yanıtı bekle
    auto response = sendCommand(command);

    // Yanıt kontrolü
    return response.has_value();
}

bool CommunicationManager::queryMotorStatus()
{
    if (!isConnected()) {
        return false;
    }

    try {
        // Pozisyon sorgusu
        auto posCommand = Command::createPositionRequest();
        sendCommandAsync(posCommand, [this](std::shared_ptr<Response> response) {
            if (response->isSuccess()) {
                if (auto pos = response->doubleValue()) {
                    QMutexLocker locker(&m_motorStatusMutex);
                    m_motorStatus.position = *pos;
                    emit positionChanged(*pos);
                }
            }
        });

        // Diğer durum sorgularını ekle
        // ...

        return true;
    } catch (const std::exception& e) {
        m_lastError = tr("Durum sorgulama hatası: %1").arg(e.what());
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);
        return false;
    }
}

QVariantMap CommunicationManager::getMotorStatus() const
{
    QMutexLocker locker(&m_motorStatusMutex);

    QVariantMap status;
    status["enabled"] = m_motorStatus.enabled;
    status["moving"] = m_motorStatus.moving;
    status["error"] = m_motorStatus.error;
    status["position"] = m_motorStatus.position;
    status["velocity"] = m_motorStatus.velocity;
    status["torque"] = m_motorStatus.torque;
    status["errorMessage"] = m_motorStatus.errorMessage;

    return status;
}

double CommunicationManager::getCurrentPosition() const
{
    QMutexLocker locker(&m_motorStatusMutex);
    return m_motorStatus.position;
}

double CommunicationManager::getCurrentVelocity() const
{
    QMutexLocker locker(&m_motorStatusMutex);
    return m_motorStatus.velocity;
}

void CommunicationManager::setStatusPollingInterval(int intervalMs)
{
    if (intervalMs <= 0) {
        m_statusPollingEnabled = false;
        m_statusPollingTimer.stop();
    } else {
        m_statusPollingEnabled = true;
        m_statusPollingTimer.setInterval(intervalMs);

        if (isConnected() && !m_statusPollingTimer.isActive()) {
            m_statusPollingTimer.start();
        }
    }

    m_config.statusPollingIntervalMs = intervalMs;
}

void CommunicationManager::enableStatusPolling(bool enabled)
{
    m_statusPollingEnabled = enabled;

    if (enabled) {
        if (isConnected() && !m_statusPollingTimer.isActive()) {
            m_statusPollingTimer.start();
        }
    } else {
        m_statusPollingTimer.stop();
    }
}

void CommunicationManager::enableAutoReconnect(bool enabled, int intervalMs)
{
    m_autoReconnectEnabled = enabled;

    if (intervalMs > 0) {
        m_reconnectTimer.setInterval(intervalMs);
        m_config.autoReconnectIntervalMs = intervalMs;
    }

    if (enabled) {
        if (!isConnected() && !m_reconnectTimer.isActive()) {
            m_reconnectTimer.start();
        }
    } else {
        m_reconnectTimer.stop();
    }

    m_config.autoReconnect = enabled;
}

void CommunicationManager::enableTrafficLogging(bool enabled)
{
    m_trafficLoggingEnabled = enabled;
    m_config.logTraffic = enabled;
}

CommunicationManager::Configuration CommunicationManager::getConfiguration() const
{
    return m_config;
}

ICommunicationChannelPtr CommunicationManager::getChannel() const
{
    return m_channel;
}

IProtocolPtr CommunicationManager::getProtocol() const
{
    return m_protocol;
}

void CommunicationManager::handleChannelStateChanged(ChannelState state)
{
    switch (state) {
    case ChannelState::Connected:
        updateState(CommunicationState::Connected);
        emit connectionStatusChanged(true);

        // Durum sorgulama zamanlayıcısını başlat
        if (m_statusPollingEnabled) {
            m_statusPollingTimer.start();
        }

        // Yeniden bağlanma zamanlayıcısını durdur
        m_reconnectTimer.stop();
        break;

    case ChannelState::Disconnected:
        updateState(CommunicationState::Disconnected);
        emit connectionStatusChanged(false);

        // Durum sorgulama zamanlayıcısını durdur
        m_statusPollingTimer.stop();

        // Otomatik yeniden bağlanma aktifse zamanlayıcıyı başlat
        if (m_autoReconnectEnabled) {
            m_reconnectTimer.start();
        }
        break;

    case ChannelState::Connecting:
        updateState(CommunicationState::Connecting);
        break;

    case ChannelState::Error:
        updateState(CommunicationState::Error);

        // Otomatik yeniden bağlanma aktifse zamanlayıcıyı başlat
        if (m_autoReconnectEnabled) {
            m_reconnectTimer.start();
        }
        break;
    }
}

void CommunicationManager::handleChannelDataReceived(const QByteArray& data)
{
    // Veriyi mesaj kuyruğuna ekle
    m_messageQueue->enqueue(data);
}

void CommunicationManager::handleChannelErrorOccurred(const QString& error)
{
    m_lastError = tr("Kanal hatası: %1").arg(error);
    logMessage(m_lastError, spdlog::level::err);
    emit errorOccurred(m_lastError);
}

void CommunicationManager::handleProtocolResponseReceived(
    const QString& commandId, std::shared_ptr<Response> response, ResponseStatus status)
{
    // Yanıt durumunu kontrol et
    if (status != ResponseStatus::Success) {
        m_lastError = tr("Protokol hatası: %1").arg(response->errorMessage());
        logMessage(m_lastError, spdlog::level::err);
        emit errorOccurred(m_lastError);
    }

    // Yanıt sinyali yayınla
    emit responseReceived(commandId, response);

    // Motor durumunu güncelle
    updateMotorStatus(response);
}

void CommunicationManager::handleProtocolErrorOccurred(const QString& error)
{
    m_lastError = tr("Protokol hatası: %1").arg(error);
    logMessage(m_lastError, spdlog::level::err);
    emit errorOccurred(m_lastError);
}

void CommunicationManager::pollStatus()
{
    if (!isConnected()) {
        return;
    }

    // Motor durumunu sorgula
    // queryMotorStatus();
}

void CommunicationManager::attemptReconnect()
{
    if (isConnected()) {
        m_reconnectTimer.stop();
        return;
    }

    // Yeniden bağlanmayı dene
    logMessage(tr("Yeniden bağlanma deneniyor..."), spdlog::level::info);

    // Kanal ayarlandıysa
    if (m_lastChannelType.isEmpty() || m_channel) {
        connect();
    } else {
        // Son kanal bilgileriyle yeniden yapılandır
        configure(m_lastChannelType, m_config.protocolType, m_lastChannelParams, m_config.protocolParams);
        connect();
    }
}

void CommunicationManager::checkCommandTimeouts()
{
    QMutexLocker locker(&m_pendingCommandsMutex);
    QDateTime now = QDateTime::currentDateTime();

    QStringList timedOutCommands;

    // Tüm bekleyen komutları kontrol et
    for (auto it = m_pendingCommands.begin(); it != m_pendingCommands.end(); ++it) {
        const PendingCommand& pendingCmd = it->second;

        // Zaman aşımı kontrolü
        qint64 elapsedMs = pendingCmd.timestamp.msecsTo(now);
        if (elapsedMs > pendingCmd.timeoutMs) {
            QString commandId = it->first;
            // Zaman aşımına uğrayan komut
            timedOutCommands.append(commandId);

            handleCommandTimeout(commandId);
        }
    }

    // Zaman aşımına uğrayan komutları temizle
    for (const QString& id : timedOutCommands) {
        m_pendingCommands.erase(id);
    }
}

ICommunicationChannelPtr CommunicationManager::createChannel(const QString& channelType)
{
    if (channelType.toLower() == "serialport") {
        return ICommunicationChannelPtr(new SerialPortChannel(this));
    }

    // Desteklenmeyen kanal türü
    m_lastError = tr("Desteklenmeyen kanal türü: %1").arg(channelType);
    logMessage(m_lastError, spdlog::level::err);
    emit errorOccurred(m_lastError);

    return nullptr;
}

IProtocolPtr CommunicationManager::createProtocol(const QString& protocolType)
{
    if (protocolType.toLower() == "simpleiq") {
        return IProtocolPtr(new SimpleIQProtocol(this));
    }

    // Desteklenmeyen protokol türü
    m_lastError = tr("Desteklenmeyen protokol türü: %1").arg(protocolType);
    logMessage(m_lastError, spdlog::level::err);
    emit errorOccurred(m_lastError);

    return nullptr;
}

void CommunicationManager::updateState(CommunicationState newState)
{
    if (m_state == newState) {
        return;
    }

    m_state = newState;

    QString stateStr;
    switch (newState) {
    case CommunicationState::Disconnected:
        stateStr = "Disconnected";
        break;
    case CommunicationState::Connecting:
        stateStr = "Connecting";
        break;
    case CommunicationState::Connected:
        stateStr = "Connected";
        break;
    case CommunicationState::Disconnecting:
        stateStr = "Disconnecting";
        break;
    case CommunicationState::Error:
        stateStr = "Error";
        break;
    }

    logMessage(tr("Durum değişti: %1").arg(stateStr), spdlog::level::info);

    emit stateChanged(newState);
}

void CommunicationManager::updateMotorStatus(const std::shared_ptr<Response>& response)
{
    QMutexLocker locker(&m_motorStatusMutex);

    bool statusChanged = false;
    bool positionChanged = false;
    bool velocityChanged = false;

    // Yanıt türüne göre motor durumunu güncelle
    if (response->command() == "PS" || response->command() == "PA") {
        // Pozisyon yanıtı
        if (auto pos = response->doubleValue()) {
            positionChanged = (m_motorStatus.position != *pos);
            m_motorStatus.position = *pos;
        }
    } else if (response->command() == "VL" || response->command() == "JV") {
        // Hız yanıtı
        if (auto vel = response->doubleValue()) {
            velocityChanged = (m_motorStatus.velocity != *vel);
            m_motorStatus.velocity = *vel;
        }
    } else if (response->command() == "SC") {
        // Durum kodu yanıtı
        if (auto status = response->intValue()) {
            // Durum kodunu yorumla (protokole özgü)
            bool enabled = ((*status & 0x01) != 0);
            bool moving = ((*status & 0x02) != 0);
            bool error = ((*status & 0x04) != 0);

            statusChanged = (m_motorStatus.enabled != enabled ||
                             m_motorStatus.moving != moving ||
                             m_motorStatus.error != error);

            m_motorStatus.enabled = enabled;
            m_motorStatus.moving = moving;
            m_motorStatus.error = error;
        }
    } else if (response->type() == ResponseType::Error) {
        // Hata yanıtı
        m_motorStatus.error = true;
        m_motorStatus.errorMessage = response->errorMessage();
        statusChanged = true;
    }

    // Sinyalleri yayınla
    locker.unlock();

    if (statusChanged) {
        emit motorStatusChanged(getMotorStatus());
    }

    if (positionChanged) {
        emit ::CommunicationManager::positionChanged(m_motorStatus.position);
    }

    if (velocityChanged) {
        emit ::CommunicationManager::velocityChanged(m_motorStatus.velocity);
    }
}

void CommunicationManager::logMessage(const QString& message, spdlog::level::level_enum level)
{
    if (m_logger) {
        m_logger->log(level, "{}", message.toStdString());
    }
}

void CommunicationManager::logTraffic(bool isSending, const QByteArray& data)
{
    if (!m_logger) {
        return;
    }

    QString dataStr = QString::fromUtf8(data);
    // Yeni satır karakterlerini temizle
    dataStr = dataStr.replace("\r", "\\r").replace("\n", "\\n");

    if (isSending) {
        m_logger->debug("[TX] {}", dataStr.toStdString());
    } else {
        m_logger->debug("[RX] {}", dataStr.toStdString());
    }
}

void CommunicationManager::clearPendingCommands(const QString& reason)
{
    QMutexLocker locker(&m_pendingCommandsMutex);

    // Tüm bekleyen komutlara hata yanıtı gönder
    for (auto it = m_pendingCommands.begin(); it != m_pendingCommands.end(); ++it) {
        QString commandId = it->first;
        PendingCommand& pendingCmd = it->second;

        // Hata yanıtı oluştur
        std::shared_ptr<Response> errorResponse = std::make_shared<Response>(commandId, "");
        errorResponse->setType(ResponseType::Error);
        errorResponse->setErrorMessage(reason.isEmpty() ? tr("Komut iptal edildi") : reason);

        // Callback varsa çağır
        if (pendingCmd.callback) {
            pendingCmd.callback(errorResponse);
        }

        // Yanıt alındı sinyali yayınla (hata)
        emit responseReceived(commandId, errorResponse);
    }

    // Bekleyen komutları temizle
    m_pendingCommands.clear();
}

void CommunicationManager::handleCommandTimeout(const QString& commandId)
{
    QMutexLocker locker(&m_pendingCommandsMutex);

    // Bekleyen komut listesinde bu kimlik var mı kontrol et
    if (!m_pendingCommands.contains(commandId)) {
        return;
    }

    PendingCommand pendingCmd = m_pendingCommands[commandId];

    // Hata yanıtı oluştur
    std::shared_ptr<Response> errorResponse = std::make_shared<Response>(commandId, "");
    errorResponse->setType(ResponseType::Error);
    errorResponse->setErrorMessage(tr("Komut zaman aşımı"));

    // Callback varsa çağır
    if (pendingCmd.callback) {
        pendingCmd.callback(errorResponse);
    }

    // Yanıt alındı sinyali yayınla (hata)
    emit responseReceived(commandId, errorResponse);

    // Log
    logMessage(tr("Komut zaman aşımı: %1").arg(commandId), spdlog::level::warn);
}

void CommunicationManager::handleCommandComplete(const QString& commandId, const std::shared_ptr<Response>& response)
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
    emit responseReceived(commandId, response);

    // Bekleyen komutlar listesinden çıkar
    m_pendingCommands.erase(commandId);

    // Motor durumunu güncelle
    updateMotorStatus(response);
}

int CommunicationManager::getDefaultCommandTimeout() const
{
    return m_config.commandTimeoutMs > 0 ? m_config.commandTimeoutMs : 1000;
}
