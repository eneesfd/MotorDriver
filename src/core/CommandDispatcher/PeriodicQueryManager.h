#pragma once

#include <QObject>
#include <QHash>
#include <QMutex>
#include <QTimer>
#include <QElapsedTimer>
#include <QDateTime>
#include <QSharedPointer>
#include <functional>
#include <memory>

// Forward declarations
class Command;
class Response;
class CommandDispatcher;
class SequentialCommandProcessor;
namespace spdlog {
class logger;
}

/**
 * @brief Periyodik sorguların yönetimini sağlayan sınıf
 *
 * Bu sınıf, farklı UI sayfaları veya bileşenler için periyodik sorguları
 * yönetir. Sorguları gruplar halinde organize eder ve her grubun kendi
 * zamanlama ayarlarına sahip olmasını sağlar.
 *
 * Singleton pattern kullanır ve uygulama genelinde tek bir merkezi yönetim sağlar.
 */
class PeriodicQueryManager : public QObject
{
    Q_OBJECT

public:
    // Sorgu tanımlayıcısı
    using QueryId = QString;

    // Sorgu grubu tanımlayıcısı (örn: "CurrentTab", "Graph1", "StatusBar")
    using QueryGroupId = QString;

    // Sorgu önceliği (daha düşük değer = daha yüksek öncelik)
    enum class Priority {
        Critical = 0,  // Kritik durum bilgileri
        High = 1,      // Kullanıcı arayüzünde görünen veriler
        Normal = 2,    // Düzenli güncellemeler
        Low = 3,       // Arkaplanda toplanan veriler
        Background = 4 // En düşük öncelik
    };

    /**
     * @brief Singleton instance erişimi
     * @return PeriodicQueryManager singleton nesnesi
     */
    static PeriodicQueryManager& instance();

    /**
     * @brief Singleton nesnesini bağımlılıklarla başlatır
     * @param dispatcher Komut gönderici
     * @param logger Log yöneticisi
     */
    static void initialize(QSharedPointer<CommandDispatcher> dispatcher,
                           std::shared_ptr<spdlog::logger> logger);

    /**
     * @brief Sınıfın doğru şekilde başlatılıp başlatılmadığını kontrol eder
     * @return Başlatıldıysa true, aksi halde false
     */
    static bool isInitialized();

    /**
     * @brief Bir sorgu ekleme (tekrar eklenirse günceller)
     * @param id Sorgu tanımlayıcısı
     * @param groupId Sorgunun ait olduğu grup
     * @param command Çalıştırılacak komut
     * @param callback Yanıt alındığında çağrılacak fonksiyon
     * @param intervalMs Sorgu aralığı (milisaniye)
     * @param priority Sorgu önceliği
     */
    void registerQuery(
        const QueryId& id,
        const QueryGroupId& groupId,
        std::shared_ptr<Command> command,
        std::function<void(std::shared_ptr<Response>)> callback,
        int intervalMs,
        Priority priority = Priority::Normal
        );

    /**
     * @brief Bir sorgu grubunu başlatma
     * @param groupId Sorgu grubu tanımlayıcısı
     */
    void startQueryGroup(const QueryGroupId& groupId);

    /**
     * @brief Bir sorgu grubunu durdurma
     * @param groupId Sorgu grubu tanımlayıcısı
     */
    void stopQueryGroup(const QueryGroupId& groupId);

    /**
     * @brief Sorgu aralığını değiştirme
     * @param id Sorgu tanımlayıcısı
     * @param intervalMs Yeni aralık (milisaniye)
     */
    void setQueryInterval(const QueryId& id, int intervalMs);

    /**
     * @brief Özel: Grafik modunda kullanım için daha hızlı update desteği
     * @param groupId Sorgu grubu tanımlayıcısı
     * @param enable Etkinleştirme durumu
     */
    void enableHighFrequencyMode(const QueryGroupId& groupId, bool enable);

    /**
     * @brief Tüm sorgu gruplarını başlatma
     */
    void startAllGroups();

    /**
     * @brief Tüm sorgu gruplarını durdurma
     */
    void stopAllGroups();

    /**
     * @brief Bir sorgunun aktif olup olmadığını kontrol etme
     * @param id Sorgu tanımlayıcısı
     * @return Aktifse true, değilse false
     */
    bool isQueryActive(const QueryId& id) const;

    /**
     * @brief Bir grubun aktif olup olmadığını kontrol etme
     * @param groupId Grup tanımlayıcısı
     * @return Aktifse true, değilse false
     */
    bool isGroupActive(const QueryGroupId& groupId) const;

signals:
    /**
     * @brief Sorgu durumu değiştiğinde sinyal
     * @param id Sorgu tanımlayıcısı
     * @param active Yeni durum
     */
    void queryStateChanged(const QueryId& id, bool active);

    /**
     * @brief Sorgu hata verdiğinde sinyal
     * @param id Sorgu tanımlayıcısı
     * @param errorMessage Hata mesajı
     */
    void queryError(const QueryId& id, const QString& errorMessage);

private:
    /**
     * @brief Private yapıcı (singleton)
     * @param parent Parent QObject
     */
    PeriodicQueryManager(QObject* parent = nullptr);

    /**
     * @brief Sorgu yapısı
     */
    struct QueryInfo {
        QueryId id;
        QueryGroupId groupId;
        std::shared_ptr<Command> command;
        std::function<void(std::shared_ptr<Response>)> callback;
        int intervalMs;
        Priority priority;
        QElapsedTimer lastExecutionTime;
        bool active = false;
    };

    /**
     * @brief Sorgu grubu yapısı
     */
    struct QueryGroup {
        QueryGroupId id;
        bool active = false;
        bool highFrequencyMode = false;
        QTimer* timer = nullptr;
    };

    /**
     * @brief Eğer yoksa sorgu grubunu oluşturma
     * @param groupId Grup tanımlayıcısı
     */
    void createQueryGroupIfNotExists(const QueryGroupId& groupId);

    /**
     * @brief Zamanlayıcı olay işleyici
     * @param groupId Grup tanımlayıcısı
     */
    void onTimerTick(const QueryGroupId& groupId);

    /**
     * @brief Sorguyu çalıştırma
     * @param query Çalıştırılacak sorgu
     */
    void executeQuery(QueryInfo& query);

    // Komut işleyici
    QSharedPointer<CommandDispatcher> m_commandDispatcher;
    std::shared_ptr<spdlog::logger> m_logger;
    SequentialCommandProcessor* m_sequentialProcessor;

    // Sorgular ve gruplar
    QHash<QueryId, QueryInfo> m_queries;
    QHash<QueryGroupId, QueryGroup> m_queryGroups;

    // Thread güvenliği
    mutable QMutex m_mutex;

    // Çalışma durumu
    bool m_running;

    // Başlatma durumu
    static bool s_initialized;
};
