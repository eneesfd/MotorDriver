// SerialPortChannel.h - Güncellenmiş sınıf tanımı

#ifndef SERIALPORTCHANNEL_H
#define SERIALPORTCHANNEL_H

#include <QObject>
#include "ICommunicationChannel.h"
#include "../Utils/CircularBuffer.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <mutex>
#include <condition_variable>
#include <QThread>
#include <atomic>
#include <QEventLoop>

class SerialPortChannel : public ICommunicationChannel
{
    Q_OBJECT
public:
    struct SerialPortConfiguration : public Configuration {
        QString portName;
        qint32 baudRate = QSerialPort::Baud115200;
        QSerialPort::DataBits dataBits = QSerialPort::Data8;
        QSerialPort::Parity parity = QSerialPort::NoParity;
        QSerialPort::StopBits stopBits = QSerialPort::OneStop;
        QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;
        int readBufferSize = 64 * 1024;
        int writeBufferSize = 64 * 1024;
        int readTimeoutMs = 50;
        int writeTimeoutMs = 50;
    };

    explicit SerialPortChannel(QObject *parent = nullptr);
    ~SerialPortChannel() override;

    bool configure(const Configuration& config) override;
    bool open() override;
    bool close() override;
    qint64 write(const QByteArray& data) override;
    std::optional<QByteArray> read(qint64 maxSize = -1) override;
    ChannelState state() const override;
    QVariantList availableChannels() const override;
    QString lastError() const override;
    void setMessageHandler(std::function<void(const QByteArray&)> callback) override;
    bool isConnected() const override;
    void setBufferSize(int size);

private slots:
    void handleError(QSerialPort::SerialPortError error);
    void handleBytesWritten(qint64 bytes);
    void checkBufferStatus();
    void handleDataRead(const QByteArray& data);
    void handleWriteRequest(const QByteArray& data);

private:
    std::unique_ptr<QSerialPort> m_serialPort;
    SerialPortConfiguration m_config;
    ChannelState m_state = ChannelState::Disconnected;
    QString m_lastError;

    std::shared_ptr<CircularBuffer> m_readBuffer;
    std::shared_ptr<CircularBuffer> m_writeBuffer;

    mutable QMutex m_stateMutex;
    mutable QMutex m_readMutex;
    mutable QMutex m_writeMutex;
    mutable QMutex m_callbackMutex;

    QWaitCondition m_readCondition;
    QWaitCondition m_writeCondition;

    std::atomic<bool> m_running{false};
    std::unique_ptr<QThread> m_readerThread;
    std::unique_ptr<QThread> m_writerThread;

    std::function<void(const QByteArray&)> m_messageHandler;
    QTimer m_bufferTimer;

    void startReaderThread();
    void startWriterThread();
    void stopThreads();
    void updateState(ChannelState newState);
    static QString errorString(QSerialPort::SerialPortError error);
};

// SerialPortReaderWorker - Doğru event loop implementasyonu ile
class SerialPortReaderWorker : public QObject
{
    Q_OBJECT
public:
    SerialPortReaderWorker(QSerialPort* serialPort, std::shared_ptr<CircularBuffer> buffer,
                           QMutex* mutex, QWaitCondition* condition)
        : m_serialPort(serialPort), m_buffer(buffer), m_mutex(mutex), m_condition(condition)
    {}

public slots:
    void start() {
        m_running = true;

        // Event loop oluştur ve çalıştır
        QEventLoop eventLoop;

        while (m_running && !QThread::currentThread()->isInterruptionRequested()) {
            // Event'leri işle (bu, QueuedConnection ile bağlanan sinyallerin çalışmasını sağlar)
            eventLoop.processEvents(QEventLoop::AllEvents, 10);

            // CPU kullanımını azaltmak için kısa bekle
            QThread::msleep(10);
        }

        qDebug() << "Reader worker stopped";
    }

    void onReadyRead() {
        if (m_serialPort && m_serialPort->isOpen() && m_serialPort->bytesAvailable() > 0) {
            QByteArray data = m_serialPort->readAll();
            if (!data.isEmpty()) {
                QMutexLocker locker(m_mutex);
                m_buffer->write(data);
                m_condition->wakeAll();

                // Veri okundu sinyali gönder
                emit dataRead(data);
            }
        }
    }

    void stop() {
        m_running = false;
    }

signals:
    void dataRead(const QByteArray& data);

private:
    QSerialPort* m_serialPort;
    std::shared_ptr<CircularBuffer> m_buffer;
    QMutex* m_mutex;
    QWaitCondition* m_condition;
    bool m_running = false;
};

// SerialPortWriterWorker - Doğru event loop ve writeData sinyali işleme ile
class SerialPortWriterWorker : public QObject
{
    Q_OBJECT
public:
    SerialPortWriterWorker(QSerialPort* serialPort, std::shared_ptr<CircularBuffer> buffer,
                           QMutex* mutex, QWaitCondition* condition)
        : m_serialPort(serialPort), m_buffer(buffer), m_mutex(mutex), m_condition(condition)
    {}

public slots:
    void start() {
        m_running = true;

        // Event loop oluştur
        QEventLoop eventLoop;

        while (m_running && !QThread::currentThread()->isInterruptionRequested()) {
            // Event'leri işle (QueuedConnection için gerekli)
            eventLoop.processEvents(QEventLoop::AllEvents, 10);

            QMutexLocker locker(m_mutex);

            // Buffer'da veri var mı kontrol et
            if (m_buffer->size() > 0) {
                QByteArray data = m_buffer->readAll();
                locker.unlock(); // Mutex'i serbest bırak

                // Ana thread'e yazma isteği gönder
                emit writeData(data);
            } else {
                // Veri yoksa, yeni veri gelene kadar bekle (en fazla 100ms)
                m_condition->wait(m_mutex, 100);
            }

            // CPU yükünü azaltmak için kısa bekleme
            QThread::msleep(5);
        }

        qDebug() << "Writer worker stopped";
    }

    void onBytesWritten(qint64 bytes) {

        // Bytes written sinyalini işle ve ilet
        emit dataWritten(bytes);

        // Yazma işlemi tamamlandıktan sonra kısa bir süre bekle
        // Bu, yarış koşullarını azaltmaya yardımcı olabilir
        QThread::msleep(1);
    }

    void stop() {
        m_running = false;
        m_condition->wakeAll(); // Bekleyen koşulları uyandır
    }

signals:
    void writeData(const QByteArray& data); // Ana thread'e yazma isteği sinyali
    void dataWritten(qint64 bytes);

private:
    QSerialPort* m_serialPort;
    std::shared_ptr<CircularBuffer> m_buffer;
    QMutex* m_mutex;
    QWaitCondition* m_condition;
    bool m_running = false;
};

#endif // SERIALPORTCHANNEL_H
