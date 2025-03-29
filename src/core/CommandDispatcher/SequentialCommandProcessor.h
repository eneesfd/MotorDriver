// SequentialCommandProcessor.h
#pragma once

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <functional>
#include <memory>
#include <QSharedPointer>

class Command;
class Response;
class CommandDispatcher;

namespace spdlog {
class logger;
}

/**
 * @brief Komutların sıralı işlenmesini sağlayan sınıf
 *
 * Bu sınıf, komutların sırayla gönderilmesini ve her bir komut tamamlandıktan sonra
 * bir sonraki komuta geçilmesini sağlar. Bu şekilde, komut yanıtlarının doğru sırada
 * işlenmesi garantilenir.
 */
class SequentialCommandProcessor : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Yapıcı fonksiyon
     * @param dispatcher Komut göndermek için kullanılacak CommandDispatcher nesnesi
     * @param parent Ebeveyn QObject nesnesi
     */
    explicit SequentialCommandProcessor(QSharedPointer<CommandDispatcher> dispatcher, std::shared_ptr<spdlog::logger> logger_, QObject* parent = nullptr);

    /**
     * @brief Kuyruk durumunu kontrol eder
     * @return Kuyrukta hiç komut kalmadıysa true, aksi halde false
     */
    bool isQueueEmpty() const;

    /**
     * @brief Yeni bir komut ekler ve kuyruğu işlemeye başlar
     * @param command Gönderilecek komut
     * @param callback Komut tamamlandığında çağrılacak callback fonksiyonu
     * @param timeoutMs Zaman aşımı süresi (milisaniye)
     */
    void addCommand(std::shared_ptr<Command> command,
                    std::function<void(std::shared_ptr<Response>)> callback,
                    int timeoutMs = 1000);

    /**
     * @brief Kuyruktaki tüm komutları temizler
     */
    void clearQueue();

    /**
     * @brief Komut listesini sırayla işler
     * @param commands İşlenecek komut listesi
     * @param callbacks Her bir komut için çağrılacak callback fonksiyonları
     * @param timeoutMs Zaman aşımı süresi (milisaniye)
     */
    void processCommands(const QList<std::shared_ptr<Command>>& commands,
                         const QList<std::function<void(std::shared_ptr<Response>)>>& callbacks,
                         int timeoutMs = 1000);

private:
    /**
     * @brief Kuyruktaki bir sonraki komutu işler
     */
    void processNextCommand();

    /**
     * @brief İç yapı: Komut verisi
     */
    struct CommandEntry {
        std::shared_ptr<Command> command;
        std::function<void(std::shared_ptr<Response>)> callback;
        int timeoutMs;
    };

    QSharedPointer<CommandDispatcher> m_dispatcher;  // Komut gönderici
    std::shared_ptr<spdlog::logger> m_logger; // Loglama nesnesi
    QQueue<CommandEntry> m_queue;     // Komut kuyruğu
    mutable QMutex m_mutex;           // Thread güvenliği için mutex
    bool m_processing;                // İşleme durumu
};
