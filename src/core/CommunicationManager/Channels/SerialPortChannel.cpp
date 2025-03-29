#include "SerialPortChannel.h"
#include <QDebug>
#include <QCoreApplication>

SerialPortChannel::SerialPortChannel(QObject* parent)
    : ICommunicationChannel(parent),
    m_serialPort(new QSerialPort(this)),
    m_state(ChannelState::Disconnected),
    m_readBuffer(new CircularBuffer(64 * 1024)),  // 64KB okuma tamponu
    m_writeBuffer(new CircularBuffer(64 * 1024))  // 64KB yazma tamponu
{
    connect(m_serialPort.get(), &QSerialPort::errorOccurred,
            this, &SerialPortChannel::handleError);

    // Tampon durumunu izle
    connect(&m_bufferTimer, &QTimer::timeout,
            this, &SerialPortChannel::checkBufferStatus);

    m_bufferTimer.setInterval(100);  // 100ms'de bir kontrol et
    m_bufferTimer.start();
}

SerialPortChannel::~SerialPortChannel()
{
    stopThreads();

    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }
}

bool SerialPortChannel::configure(const Configuration& config)
{
    // Configuration nesnesini SerialPortConfiguration'a dönüştür
    SerialPortConfiguration serialConfig;

    if (auto typedConfig = dynamic_cast<const SerialPortConfiguration*>(&config)) {
        serialConfig = *typedConfig;
    } else {
        // Configuration türünden SerialPortConfiguration'a dönüştürülemezse
        // GenericConfiguration'dan manuel dönüşüm yap
        QVariantMap params = config.parameters;

        if (params.contains("portName"))
            serialConfig.portName = params["portName"].toString();

        if (params.contains("baudRate"))
            serialConfig.baudRate = params["baudRate"].toInt();

        if (params.contains("dataBits"))
            serialConfig.dataBits = static_cast<QSerialPort::DataBits>(params["dataBits"].toInt());

        if (params.contains("parity"))
            serialConfig.parity = static_cast<QSerialPort::Parity>(params["parity"].toInt());

        if (params.contains("stopBits"))
            serialConfig.stopBits = static_cast<QSerialPort::StopBits>(params["stopBits"].toInt());

        if (params.contains("flowControl"))
            serialConfig.flowControl = static_cast<QSerialPort::FlowControl>(params["flowControl"].toInt());

        if (params.contains("readBufferSize"))
            serialConfig.readBufferSize = params["readBufferSize"].toInt();

        if (params.contains("writeBufferSize"))
            serialConfig.writeBufferSize = params["writeBufferSize"].toInt();
    }

    m_config = serialConfig;

    // Şu anda bağlıysa, önce bağlantıyı kes
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }

    // Seri port parametrelerini ayarla
    m_serialPort->setPortName(m_config.portName);
    m_serialPort->setBaudRate(m_config.baudRate);
    m_serialPort->setDataBits(m_config.dataBits);
    m_serialPort->setParity(m_config.parity);
    m_serialPort->setStopBits(m_config.stopBits);
    m_serialPort->setFlowControl(m_config.flowControl);

    // Tampon boyutlarını ayarla
    m_serialPort->setReadBufferSize(m_config.readBufferSize);
    m_readBuffer.reset(new CircularBuffer(m_config.readBufferSize));
    m_writeBuffer.reset(new CircularBuffer(m_config.writeBufferSize));

    // Bağlantı durumunu güncelle
    updateState(ChannelState::Disconnected);

    return true;
}

bool SerialPortChannel::open()
{
    if (m_serialPort->isOpen()) {
        return true; // Zaten açık
    }

    // Bağlanıyor durumuna geç
    updateState(ChannelState::Connecting);

    // Bağlanma işlemini başka bir thread'de yap ki GUI bloklama olmasın
    QMetaObject::invokeMethod(this, [this]() {
        // Seri portu aç
        if (!m_serialPort->open(QIODevice::ReadWrite)) {
            QString errorMsg = tr("Seri port açılamadı: %1").arg(m_serialPort->errorString());
            m_lastError = errorMsg;
            qDebug() << errorMsg;

            QMetaObject::invokeMethod(this, [this]() {
                updateState(ChannelState::Error);
                emit errorOccurred(m_lastError);
            }, Qt::QueuedConnection);

            return;
        }

        // Reader ve writer thread'leri başlat
        startReaderThread();
        startWriterThread();

        // Tampon durumu zamanlayıcısını başlat
        m_bufferTimer.start();

        QMetaObject::invokeMethod(this, [this]() {
            updateState(ChannelState::Connected);
            emit stateChanged(ChannelState::Connected);
        }, Qt::QueuedConnection);

        qDebug() << "Serial port" << m_config.portName << "opened successfully at"
                 << m_config.baudRate << "baud";
    }, Qt::QueuedConnection);

    return true;
}

bool SerialPortChannel::close()
{
    if (!m_serialPort->isOpen()) {
        return true; // Zaten kapalı
    }

    // Bağlantı kesiliyor durumuna geç
    updateState(ChannelState::Disconnected);

    // Thread'leri durdur
    stopThreads();

    // Tampon durumu zamanlayıcısını durdur
    m_bufferTimer.stop();

    // Seri portu kapat
    m_serialPort->close();

    qDebug() << "Serial port closed";

    return true;
}

qint64 SerialPortChannel::write(const QByteArray& data)
{
    if (!m_serialPort->isOpen()) {
        m_lastError = tr("Yazma hatası: Seri port bağlı değil");
        emit errorOccurred(m_lastError);
        return -1;
    }

    // Veriyi yazma tamponuna ekle
    QMutexLocker writeLocker(&m_writeMutex);
    size_t bytesWritten = m_writeBuffer->write(data);
    m_writeCondition.wakeAll();  // Yazma thread'ini uyandır

    return static_cast<qint64>(bytesWritten);
}

std::optional<QByteArray> SerialPortChannel::read(qint64 maxSize)
{
    if (!m_serialPort->isOpen()) {
        m_lastError = tr("Okuma hatası: Seri port bağlı değil");
        emit errorOccurred(m_lastError);
        return std::nullopt;
    }

    // Okuma tamponundan veri oku
    QMutexLocker readLocker(&m_readMutex);

    // Tampon boşsa veya veri yoksa null dön
    if (m_readBuffer->size() == 0) {
        return std::nullopt;
    }

    // Veriyi oku ve dön
    QByteArray data;
    if (maxSize < 0 || static_cast<size_t>(maxSize) > m_readBuffer->size()) {
        data = m_readBuffer->readAll();
    } else {
        data = m_readBuffer->read(maxSize);
    }

    return data;
}

ChannelState SerialPortChannel::state() const
{
    QMutexLocker stateLocker(&m_stateMutex);
    return m_state;
}

QVariantList SerialPortChannel::availableChannels() const
{
    QVariantList result;

    // Sistem üzerindeki tüm seri portları al
    const auto serialPortInfos = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo &info : serialPortInfos) {
        QVariantMap portInfo;
        portInfo["portName"] = info.portName();
        portInfo["description"] = info.description();
        portInfo["manufacturer"] = info.manufacturer();
        portInfo["serialNumber"] = info.serialNumber();
        portInfo["systemLocation"] = info.systemLocation();
        portInfo["vendorIdentifier"] = info.hasVendorIdentifier() ?
                                           QString::number(info.vendorIdentifier(), 16) : "";
        portInfo["productIdentifier"] = info.hasProductIdentifier() ?
                                            QString::number(info.productIdentifier(), 16) : "";

        result.append(portInfo);
    }

    return result;
}

QString SerialPortChannel::lastError() const
{
    return m_lastError;
}

void SerialPortChannel::setMessageHandler(std::function<void(const QByteArray&)> callback)
{
    QMutexLocker locker(&m_callbackMutex);
    m_messageHandler = callback;
}

bool SerialPortChannel::isConnected() const
{
    return m_serialPort->isOpen();
}

void SerialPortChannel::setBufferSize(int size)
{
    if (size <= 0) {
        return;
    }

    QMutexLocker readLocker(&m_readMutex);
    QMutexLocker writeLocker(&m_writeMutex);

    m_readBuffer.reset(new CircularBuffer(size));
    m_writeBuffer.reset(new CircularBuffer(size));
}

void SerialPortChannel::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) {
        return;
    }

    // Hata mesajını oluştur
    QString errorMsg;
    switch (error) {
    case QSerialPort::DeviceNotFoundError:
        errorMsg = tr("Cihaz bulunamadı");
        break;
    case QSerialPort::PermissionError:
        errorMsg = tr("Erişim hatası: Port başka bir uygulama tarafından kullanılıyor olabilir");
        break;
    case QSerialPort::OpenError:
        errorMsg = tr("Port açılamadı");
        break;
    case QSerialPort::WriteError:
        errorMsg = tr("Yazma hatası");
        break;
    case QSerialPort::ReadError:
        errorMsg = tr("Okuma hatası");
        break;
    case QSerialPort::ResourceError:
        errorMsg = tr("Kaynak hatası: Cihaz bağlantısı kopmuş olabilir");
        break;
    case QSerialPort::UnsupportedOperationError:
        errorMsg = tr("Desteklenmeyen işlem");
        break;
    case QSerialPort::TimeoutError:
        errorMsg = tr("Zaman aşımı");
        break;
    case QSerialPort::NotOpenError:
        errorMsg = tr("Port açık değil");
        break;
    default:
        errorMsg = tr("Bilinmeyen hata");
        break;
    }

    m_lastError = tr("Seri port hatası: %1").arg(errorMsg);
    qDebug() << m_lastError;

    // Ciddi bir hata ise durumu güncelle
    if (error != QSerialPort::TimeoutError && error != QSerialPort::NotOpenError) {
        updateState(ChannelState::Error);

        // Hata oluştu sinyalini gönder
        emit errorOccurred(m_lastError);

        // Ciddi bir hata ise bağlantıyı kapat
        if (error == QSerialPort::ResourceError ||
            error == QSerialPort::PermissionError ||
            error == QSerialPort::OpenError) {
            close();
        }
    }
}

void SerialPortChannel::handleBytesWritten(qint64 bytes)
{
    emit dataSent(bytes);
}

void SerialPortChannel::checkBufferStatus()
{
    // Okuma tamponu durumunu kontrol et
    {
        QMutexLocker readLocker(&m_readMutex);

        // Tampon doluluk oranı %90'ı geçtiyse uyarı
        if (m_readBuffer->fillPercentage() > 90) {
            qWarning() << "Okuma tamponu dolmak üzere:" << m_readBuffer->fillPercentage() << "%";
        }

        // Tampon taşması olduysa uyarı
        if (m_readBuffer->hasOverflowed()) {
            qWarning() << "Okuma tamponu taştı! Taşma sayısı:" << m_readBuffer->overflowCount();
            m_readBuffer->resetOverflowCounter();
        }
    }

    // Yazma tamponu durumunu kontrol et
    {
        QMutexLocker writeLocker(&m_writeMutex);

        // Tampon doluluk oranı %90'ı geçtiyse uyarı
        if (m_writeBuffer->fillPercentage() > 90) {
            qWarning() << "Yazma tamponu dolmak üzere:" << m_writeBuffer->fillPercentage() << "%";
        }

        // Tampon taşması olduysa uyarı
        if (m_writeBuffer->hasOverflowed()) {
            qWarning() << "Yazma tamponu taştı! Taşma sayısı:" << m_writeBuffer->overflowCount();
            m_writeBuffer->resetOverflowCounter();
        }
    }
}

void SerialPortChannel::handleDataRead(const QByteArray &data)
{
    if (!data.isEmpty()) {
        // Ham veri alındı sinyalini gönder
        emit dataReceived(data);

        // Mesaj işleyici varsa çağır
        QMutexLocker callbackLocker(&m_callbackMutex);
        if (m_messageHandler) {
            m_messageHandler(data);
        }
    }
}

void SerialPortChannel::handleWriteRequest(const QByteArray &data)
{
    // Bu metod ana thread'de çalışır ve SerialPort'a direkt yazma yapar
    if (m_serialPort && m_serialPort->isOpen()) {
        // Veriyi gönder
        qint64 bytesWritten = m_serialPort->write(data);

        if (bytesWritten <= 0) {
            m_lastError = tr("Yazma hatası: %1").arg(m_serialPort->errorString());
            emit errorOccurred(m_lastError);
        }

        // Yazma işleminin tamamlanmasını bekle
        m_serialPort->waitForBytesWritten(m_config.writeTimeoutMs);
    }
}

void SerialPortChannel::stopThreads()
{
    m_running = false;

    // Reader thread'i durdur
    if (m_readerThread) {
        m_readerThread->requestInterruption();
        m_readerThread->quit();
        // Thread'in durmasını beklemeden önce koşulları uyandır
        m_readCondition.wakeAll();
        if (m_readerThread->isRunning()) {
            m_readerThread->wait(500); // En fazla 500ms bekle
        }
        m_readerThread.reset();
    }

    // Writer thread'i durdur
    if (m_writerThread) {
        m_writerThread->requestInterruption();
        m_writerThread->quit();
        // Thread'in durmasını beklemeden önce koşulları uyandır
        m_writeCondition.wakeAll();
        if (m_writerThread->isRunning()) {
            m_writerThread->wait(500); // En fazla 500ms bekle
        }
        m_writerThread.reset();
    }
}

void SerialPortChannel::startReaderThread()
{
    // Önce eski thread varsa durdur
    if (m_readerThread) {
        m_readerThread->requestInterruption();
        if (m_readerThread->isRunning()) {
            m_readerThread->wait();
        }
        m_readerThread.reset();
    }

    // Yeni thread oluştur
    m_readerThread.reset(new QThread());

    // Ana thread'deki readyRead bağlantısını kaldır (varsa)
    disconnect(m_serialPort.get(), &QSerialPort::readyRead, this, nullptr);

    // Reader worker oluştur ve thread'e taşı
    auto readerWorker = new SerialPortReaderWorker(m_serialPort.get(), m_readBuffer, &m_readMutex, &m_readCondition);
    readerWorker->moveToThread(m_readerThread.get());

    // Sinyalleri bağla
    connect(m_readerThread.get(), &QThread::started, readerWorker, &SerialPortReaderWorker::start);
    connect(m_readerThread.get(), &QThread::finished, readerWorker, &QObject::deleteLater);

    // SerialPort readyRead sinyalini worker'a bağla
    connect(m_serialPort.get(), &QSerialPort::readyRead,
            readerWorker, &SerialPortReaderWorker::onReadyRead, Qt::QueuedConnection);

    // Worker'dan gelen veri sinyalini ana sınıfa bağla
    connect(readerWorker, &SerialPortReaderWorker::dataRead,
            this, &SerialPortChannel::handleDataRead, Qt::QueuedConnection);

    // Thread'i başlat
    m_running = true;
    m_readerThread->start();
}

void SerialPortChannel::startWriterThread()
{
    // Önce eski thread varsa durdur
    if (m_writerThread) {
        m_writerThread->requestInterruption();
        if (m_writerThread->isRunning()) {
            m_writerThread->wait();
        }
        m_writerThread.reset();
    }

    // Yeni thread oluştur
    m_writerThread.reset(new QThread());

    // Writer worker oluştur ve thread'e taşı
    auto writerWorker = new SerialPortWriterWorker(m_serialPort.get(), m_writeBuffer, &m_writeMutex, &m_writeCondition);
    writerWorker->moveToThread(m_writerThread.get());

    // Sinyalleri bağla
    connect(m_writerThread.get(), &QThread::started, writerWorker, &SerialPortWriterWorker::start);
    connect(m_writerThread.get(), &QThread::finished, writerWorker, &QObject::deleteLater);

    // Worker'ın sinyallerini bağla
    connect(writerWorker, &SerialPortWriterWorker::dataWritten,
            this, &SerialPortChannel::handleBytesWritten, Qt::QueuedConnection);

    // Önemli: writeData sinyalini handleWriteRequest slot'una bağla
    connect(writerWorker, &SerialPortWriterWorker::writeData,
            this, &SerialPortChannel::handleWriteRequest, Qt::QueuedConnection);

    // SerialPort bytesWritten sinyalini worker'a bağla
    connect(m_serialPort.get(), &QSerialPort::bytesWritten,
            writerWorker, &SerialPortWriterWorker::onBytesWritten, Qt::QueuedConnection);

    // Thread'i başlat
    m_running = true;
    m_writerThread->start();
}

void SerialPortChannel::updateState(ChannelState newState)
{
    {
        QMutexLocker locker(&m_stateMutex);

        if (m_state == newState) {
            return; // Değişiklik yok
        }

        m_state = newState;
    }

    // Durum değişikliği sinyali gönder
    emit stateChanged(newState);
}

QString SerialPortChannel::errorString(QSerialPort::SerialPortError error)
{
    //TODO ERROR HANGDLE MEKANİZMASI KURGULA

    switch (error) {
    case QSerialPort::NoError:
        return tr("Hata yok");
    case QSerialPort::DeviceNotFoundError:
        return tr("Cihaz bulunamadı");
    case QSerialPort::PermissionError:
        return tr("Yetki hatası");
    case QSerialPort::OpenError:
        return tr("Açma hatası");
    case QSerialPort::NotOpenError:
        return tr("Parite hatası");
    case QSerialPort::WriteError:
        return tr("Yazma hatası");
    case QSerialPort::ReadError:
        return tr("Okuma hatası");
    case QSerialPort::ResourceError:
        return tr("Kaynak hatası");
    case QSerialPort::UnsupportedOperationError:
        return tr("Desteklenmeyen işlem");
    case QSerialPort::UnknownError:
        return tr("Bilinmeyen hata");
    case QSerialPort::TimeoutError:
        return tr("Zaman aşımı");
    default:
        return tr("Tanımlanmamış hata");
    }
}
