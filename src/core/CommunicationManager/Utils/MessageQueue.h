#pragma once

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QTimer>
#include <memory>
#include <functional>
#include <atomic>
#include <thread>

/**
 * @brief Yüksek performanslı thread-safe mesaj kuyruğu sınıfı
 *
 * Bu sınıf, thread'ler arası mesaj alışverişi için güvenli bir kuyruk sağlar.
 * Producer-Consumer pattern uygulamasıdır ve asenkron mesaj işleme için kullanılır.
 */
class MessageQueue : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Mesaj işleme stratejilerini tanımlayan enum
     */
    enum class ProcessingStrategy {
        Immediate,              ///< Hemen işleme
        Batched,                ///< Toplu işleme
        PriorityBased,          ///< Öncelik tabanlı işleme
        Scheduled               ///< Zamanlanmış işleme
    };

    /**
     * @brief MessageQueue yapılandırma bilgilerini içeren struct
     */
    struct Configuration {
        int maxQueueSize = 1000;             ///< Maksimum kuyruk boyutu
        int processingIntervalMs = 10;       ///< İşleme aralığı (ms)
        int batchSize = 10;                  ///< Toplu işleme boyutu
        ProcessingStrategy strategy = ProcessingStrategy::Immediate;  ///< İşleme stratejisi
        bool dropWhenFull = false;           ///< Kuyruk dolduğunda düşür
        bool asyncProcessing = true;         ///< Asenkron işleme
    };

    /**
     * @brief Mesaj işleme callback türü
     */
    using MessageProcessor = std::function<void(const QByteArray&)>;

    /**
     * @brief Constructor
     * @param parent Ebeveyn QObject
     */
    explicit MessageQueue(QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~MessageQueue() override;

    /**
     * @brief MessageQueue'yu yapılandırır
     * @param config Yapılandırma bilgileri
     */
    void configure(const Configuration& config);

    /**
     * @brief Kuyruğa mesaj ekler
     * @param message Eklenecek mesaj
     * @return Ekleme başarılı ise true, değilse false
     */
    bool enqueue(const QByteArray& message);

    /**
     * @brief Kuyruktan mesaj alır (kaldırır)
     * @return Mesaj, kuyruk boşsa boş QByteArray
     */
    QByteArray dequeue();

    /**
     * @brief Kuyruğun başındaki mesajı kuyruktan kaldırmadan döndürür
     * @return Mesaj, kuyruk boşsa boş QByteArray
     */
    QByteArray peek() const;

    /**
     * @brief Mesaj işlemeyi başlatır
     * @param processor Mesaj işleme callback'i
     * @return Başlatma başarılı ise true, değilse false
     */
    bool startProcessing(MessageProcessor processor);

    /**
     * @brief Mesaj işlemeyi durdurur
     */
    void stopProcessing();

    /**
     * @brief İşleme stratejisini değiştirir
     * @param strategy Yeni işleme stratejisi
     */
    void setProcessingStrategy(ProcessingStrategy strategy);

    /**
     * @brief Maksimum kuyruk boyutunu ayarlar
     * @param size Yeni boyut
     */
    void setMaxQueueSize(int size);

    /**
     * @brief İşleme aralığını ayarlar
     * @param intervalMs Yeni aralık (ms)
     */
    void setProcessingInterval(int intervalMs);

    /**
     * @brief Toplu işleme boyutunu ayarlar
     * @param size Yeni boyut
     */
    void setBatchSize(int size);

    /**
     * @brief Kuyruk dolduğunda mesajların düşürülüp düşürülmeyeceğini ayarlar
     * @param drop Düşürme durumu
     */
    void setDropWhenFull(bool drop);

    /**
     * @brief Mevcut kuyruk boyutunu döndürür
     * @return Kuyruk boyutu
     */
    int size() const;

    /**
     * @brief Maksimum kuyruk boyutunu döndürür
     * @return Maksimum kuyruk boyutu
     */
    int maxSize() const;

    /**
     * @brief Kuyruğun boş olup olmadığını döndürür
     * @return Kuyruk boşsa true, değilse false
     */
    bool isEmpty() const;

    /**
     * @brief Kuyruğun dolu olup olmadığını döndürür
     * @return Kuyruk doluysa true, değilse false
     */
    bool isFull() const;

    /**
     * @brief İşleme durumunu döndürür
     * @return İşleme aktifse true, değilse false
     */
    bool isProcessing() const;

    /**
     * @brief Düşürülen mesaj sayısını döndürür
     * @return Düşürülen mesaj sayısı
     */
    int droppedCount() const;

    /**
     * @brief İşlenen mesaj sayısını döndürür
     * @return İşlenen mesaj sayısı
     */
    int processedCount() const;

    /**
     * @brief Düşürülen mesaj sayacını sıfırlar
     */
    void resetDroppedCount();

    /**
     * @brief İşlenen mesaj sayacını sıfırlar
     */
    void resetProcessedCount();

    /**
     * @brief Kuyruğu temizler
     */
    void clear();

signals:
    /**
     * @brief Mesaj eklendiğinde yayınlanır
     * @param size Güncel kuyruk boyutu
     */
    void messageEnqueued(int size);

    /**
     * @brief Mesaj alındığında yayınlanır
     * @param size Güncel kuyruk boyutu
     */
    void messageDequeued(int size);

    /**
     * @brief Mesaj işlendiğinde yayınlanır
     * @param message İşlenen mesaj
     */
    void messageProcessed(const QByteArray& message);

    /**
     * @brief Mesaj düşürüldüğünde yayınlanır
     * @param message Düşürülen mesaj
     */
    void messageDropped(const QByteArray& message);

    /**
     * @brief Kuyruk dolduğunda yayınlanır
     */
    void queueFull();

    /**
     * @brief Kuyruk boşaldığında yayınlanır
     */
    void queueEmpty();

    /**
     * @brief İşleme başladığında yayınlanır
     */
    void processingStarted();

    /**
     * @brief İşleme durduğunda yayınlanır
     */
    void processingStopped();

    /**
     * @brief Hata oluştuğunda yayınlanır
     * @param error Hata mesajı
     */
    void errorOccurred(const QString& error);

private slots:
    void processMessages();
    void checkQueueStatus();

private:
    // Yapılandırma
    Configuration m_config;

    // Veri yapıları
    QQueue<QByteArray> m_queue;
    mutable QMutex m_queueMutex;
    QWaitCondition m_dataCondition;
    QWaitCondition m_spaceCondition;

    // İşleme durumu
    std::atomic<bool> m_processing{false};
    std::atomic<int> m_droppedCount{0};
    std::atomic<int> m_processedCount{0};
    MessageProcessor m_processor;

    // Zamanlayıcılar
    QTimer m_processingTimer;
    QTimer m_statusTimer;

    // Asenkron işleme thread'i
    std::unique_ptr<std::thread> m_processingThread;

    // İç metodlar
    void processImmediate();
    void processBatch();
    void processPriority();
    void processScheduled();
    void startProcessingThread();
    void stopProcessingThread();
    void processingThreadFunction();
};

using MessageQueuePtr = QSharedPointer<MessageQueue>;
