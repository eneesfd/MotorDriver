#include "MessageQueue.h"
#include <memory>
#include <QDebug>

MessageQueue::MessageQueue(QObject* parent)
    : QObject(parent)
{
    // Zamanlayıcıları bağla
    connect(&m_processingTimer, &QTimer::timeout,
            this, &MessageQueue::processMessages);

    connect(&m_statusTimer, &QTimer::timeout,
            this, &MessageQueue::checkQueueStatus);

    // Durum zamanlayıcısını başlat
    m_statusTimer.setInterval(1000); // Her saniye kuyruk durumunu kontrol et
    m_statusTimer.start();
}

MessageQueue::~MessageQueue()
{
    // İşlemeyi durdur
    stopProcessing();

    // Zamanlayıcıları durdur
    m_processingTimer.stop();
    m_statusTimer.stop();

    // Kuyruğu temizle
    clear();
}

void MessageQueue::configure(const Configuration& config)
{
    m_config = config;

    // İşleme zamanlayıcısını güncelle
    if (m_config.asyncProcessing && m_processingTimer.isActive()) {
        m_processingTimer.setInterval(m_config.processingIntervalMs);
    }
}

bool MessageQueue::enqueue(const QByteArray& message)
{
    QMutexLocker locker(&m_queueMutex);

    // Kuyruk dolu mu kontrol et
    if (m_queue.size() >= m_config.maxQueueSize) {
        if (m_config.dropWhenFull) {
            // En eski mesajı düşür
            m_queue.dequeue();
            m_droppedCount++;
            emit messageDropped(message);
        } else {
            // Yer açılana kadar bekle
            if (!m_spaceCondition.wait(&m_queueMutex, 100)) { // 100ms timeout
                emit queueFull();
                return false;
            }
        }
    }

    // Mesajı kuyruğa ekle
    m_queue.enqueue(message);

    // Sinyal gönder
    emit messageEnqueued(m_queue.size());

    // Eğer kuyruk boşken ilk mesaj eklendiyse, veri geldi sinyali gönder
    if (m_queue.size() == 1) {
        m_dataCondition.wakeAll();

        if (m_queue.size() == 1 && m_queue.size() == 1) {
            emit queueEmpty();
        }
    }

    return true;
}

QByteArray MessageQueue::dequeue()
{
    QMutexLocker locker(&m_queueMutex);

    // Kuyruk boş mu kontrol et
    if (m_queue.isEmpty()) {
        return QByteArray();
    }

    // Mesajı kuyruktan al
    QByteArray message = m_queue.dequeue();

    // Sinyal gönder
    emit messageDequeued(m_queue.size());

    // Kuyrukta yer açıldı sinyali gönder
    m_spaceCondition.wakeAll();

    // Kuyruk boşaldıysa sinyal gönder
    if (m_queue.isEmpty()) {
        emit queueEmpty();
    }

    return message;
}

QByteArray MessageQueue::peek() const
{
    QMutexLocker locker(&m_queueMutex);

    if (m_queue.isEmpty()) {
        return QByteArray();
    }

    return m_queue.head();
}

bool MessageQueue::startProcessing(MessageProcessor processor)
{
    // İşleme zaten çalışıyorsa, mevcut işlemeyi durdur
    if (m_processing) {
        stopProcessing();
    }

    // İşleme fonksiyonunu ayarla
    m_processor = processor;

    if (!m_processor) {
        emit errorOccurred("Invalid message processor");
        return false;
    }

    // İşleme başladı
    m_processing = true;
    emit processingStarted();

    // İşleme stratejisine göre işleme başlat
    if (m_config.asyncProcessing) {
        // Asenkron işleme için thread veya zamanlayıcı kullan
        if (m_config.strategy == ProcessingStrategy::Scheduled) {
            // Zamanlayıcı ile işleme
            m_processingTimer.setInterval(m_config.processingIntervalMs);
            m_processingTimer.start();
        } else {
            // Thread ile işleme
            startProcessingThread();
        }
    } else {
        // Senkron işleme - hemen işle
        processMessages();
    }

    return true;
}

void MessageQueue::stopProcessing()
{
    if (!m_processing) {
        return;
    }

    // İşlemeyi durdur
    m_processing = false;

    // Zamanlayıcıyı durdur
    m_processingTimer.stop();

    // Thread'i durdur
    if (m_processingThread) {
        stopProcessingThread();
    }

    emit processingStopped();
}

void MessageQueue::setProcessingStrategy(ProcessingStrategy strategy)
{
    // Strateji değiştiğinde mevcut işlemeyi durdur ve yeniden başlat
    bool wasProcessing = m_processing;

    if (wasProcessing) {
        stopProcessing();
    }

    m_config.strategy = strategy;

    if (wasProcessing) {
        startProcessing(m_processor);
    }
}

void MessageQueue::setMaxQueueSize(int size)
{
    if (size <= 0) {
        emit errorOccurred("Invalid queue size");
        return;
    }

    m_config.maxQueueSize = size;
}

void MessageQueue::setProcessingInterval(int intervalMs)
{
    if (intervalMs <= 0) {
        emit errorOccurred("Invalid processing interval");
        return;
    }

    m_config.processingIntervalMs = intervalMs;

    // Eğer zamanlayıcı aktifse güncelle
    if (m_processingTimer.isActive()) {
        m_processingTimer.setInterval(intervalMs);
    }
}

void MessageQueue::setBatchSize(int size)
{
    if (size <= 0) {
        emit errorOccurred("Invalid batch size");
        return;
    }

    m_config.batchSize = size;
}

void MessageQueue::setDropWhenFull(bool drop)
{
    m_config.dropWhenFull = drop;
}

int MessageQueue::size() const
{
    QMutexLocker locker(&m_queueMutex);
    return m_queue.size();
}

int MessageQueue::maxSize() const
{
    return m_config.maxQueueSize;
}

bool MessageQueue::isEmpty() const
{
    QMutexLocker locker(&m_queueMutex);
    return m_queue.isEmpty();
}

bool MessageQueue::isFull() const
{
    QMutexLocker locker(&m_queueMutex);
    return m_queue.size() >= m_config.maxQueueSize;
}

bool MessageQueue::isProcessing() const
{
    return m_processing;
}

int MessageQueue::droppedCount() const
{
    return m_droppedCount;
}

int MessageQueue::processedCount() const
{
    return m_processedCount;
}

void MessageQueue::resetDroppedCount()
{
    m_droppedCount = 0;
}

void MessageQueue::resetProcessedCount()
{
    m_processedCount = 0;
}

void MessageQueue::clear()
{
    QMutexLocker locker(&m_queueMutex);
    m_queue.clear();

    // Kuyruk temizlendi sinyalleri
    emit messageDequeued(0);
    emit queueEmpty();

    // Kuyrukta yer açıldı sinyali
    m_spaceCondition.wakeAll();
}

void MessageQueue::processMessages()
{
    if (!m_processing || !m_processor) {
        return;
    }

    // İşleme stratejisine göre mesajları işle
    switch (m_config.strategy) {
    case ProcessingStrategy::Immediate:
        processImmediate();
        break;

    case ProcessingStrategy::Batched:
        processBatch();
        break;

    case ProcessingStrategy::PriorityBased:
        processPriority();
        break;

    case ProcessingStrategy::Scheduled:
        // Zamanlayıcı tarafından çağrıldığı için, hemen işle
        processImmediate();
        break;
    }
}

void MessageQueue::checkQueueStatus()
{
    // Kuyruk durumunu düzenli kontrol et
    int currentSize = size();
    int fillPercentage = (currentSize * 100) / m_config.maxQueueSize;

    // Kuyruk doluluk oranı %90'dan fazlaysa uyarı log'u
    if (fillPercentage > 90) {
        qWarning() << "MessageQueue: Queue is almost full (" << fillPercentage << "%)";
    }
}

void MessageQueue::processImmediate()
{
    // Kuyruktaki ilk mesajı al
    QByteArray message = dequeue();

    if (message.isEmpty()) {
        return;
    }

    try {
        // Mesajı işle
        m_processor(message);

        // İşlenen mesaj sayacını artır
        m_processedCount++;

        // Mesaj işlendi sinyali
        emit messageProcessed(message);
    } catch (const std::exception& e) {
        emit errorOccurred(QString("Error processing message: %1").arg(e.what()));
    }
}

void MessageQueue::processBatch()
{
    QMutexLocker locker(&m_queueMutex);

    // Toplu işleme için mesajları topla
    QList<QByteArray> batch;
    int batchSize = qMin(m_config.batchSize, m_queue.size());

    for (int i = 0; i < batchSize; ++i) {
        if (m_queue.isEmpty()) {
            break;
        }

        batch.append(m_queue.dequeue());
    }

    // Kuyruk durumunu güncelle
    if (!batch.isEmpty()) {
        emit messageDequeued(m_queue.size());

        if (m_queue.isEmpty()) {
            emit queueEmpty();
        }

        // Kuyrukta yer açıldı sinyali
        m_spaceCondition.wakeAll();
    }

    locker.unlock();

    // Toplu mesajları işle
    if (!batch.isEmpty()) {
        try {
            for (const QByteArray& message : batch) {
                m_processor(message);

                // İşlenen mesaj sayacını artır
                m_processedCount++;

                // Mesaj işlendi sinyali
                emit messageProcessed(message);
            }
        } catch (const std::exception& e) {
            emit errorOccurred(QString("Error processing batch: %1").arg(e.what()));
        }
    }
}

void MessageQueue::processPriority()
{
    // Bu örnekte basit bir öncelik stratejisi uyguluyoruz:
    // Şu anda sadece ilk mesajı işliyoruz, gerçek uygulamada
    // mesajların içeriğine göre önceliklendirilmesi gerekir
    processImmediate();
}

void MessageQueue::startProcessingThread()
{
    // İşleme thread'ini başlat
    m_processingThread = std::make_unique<std::thread>(&MessageQueue::processingThreadFunction, this);
}

void MessageQueue::stopProcessingThread()
{
    // İşleme thread'ini durdur
    if (m_processingThread && m_processingThread->joinable()) {
        m_processingThread->join();
        m_processingThread = nullptr;
    }
}

void MessageQueue::processingThreadFunction()
{
    while (m_processing) {
        // İşleme stratejisine göre mesajları işle
        QMutexLocker locker(&m_queueMutex);

        // Kuyruk boşsa, veri gelene kadar bekle
        if (m_queue.isEmpty()) {
            m_dataCondition.wait(&m_queueMutex, 100); // 100ms timeout
            locker.unlock();
            continue;
        }

        // İşleme stratejisine göre işle
        locker.unlock();

        switch (m_config.strategy) {
        case ProcessingStrategy::Immediate:
            processImmediate();
            break;

        case ProcessingStrategy::Batched:
            processBatch();
            break;

        case ProcessingStrategy::PriorityBased:
            processPriority();
            break;

        case ProcessingStrategy::Scheduled:
            // Thread içinde zamanlayıcı kullanmıyoruz
            processImmediate();
            break;
        }

        // Kısa bekleme
        std::this_thread::sleep_for(std::chrono::milliseconds(m_config.processingIntervalMs));
    }
}
