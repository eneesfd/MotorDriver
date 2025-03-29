#include "PeriodicQueryManager.h"
#include "../core/CommunicationManager/Model/Command.h"
#include "../core/CommunicationManager/Model/Response.h"
#include "CommandDispatcher.h"
#include "SequentialCommandProcessor.h"
#include <QCoreApplication>
#include <algorithm>
#include "LogManager.h"

// Başlatılma durumu
bool PeriodicQueryManager::s_initialized = false;

// Singleton instance
PeriodicQueryManager& PeriodicQueryManager::instance()
{
    static PeriodicQueryManager instance;

    if (!s_initialized) {
        // SpdlogCritical(instance.m_logger, "PeriodicQueryManager: Not initialized!");
    }

    return instance;
}

void PeriodicQueryManager::initialize(QSharedPointer<CommandDispatcher> dispatcher,
                                      std::shared_ptr<spdlog::logger> logger)
{
    if (s_initialized) {
        SpdlogWarn(logger, "PeriodicQueryManager already initialized!");
        return;
    }

    PeriodicQueryManager& mgr = instance();
    mgr.m_commandDispatcher = dispatcher;
    mgr.m_logger = logger;

    // SequentialCommandProcessor'ı oluştur
    if (mgr.m_commandDispatcher) {
        mgr.m_sequentialProcessor = new SequentialCommandProcessor(
            mgr.m_commandDispatcher,
            mgr.m_logger,
            &mgr  // Parent olarak singleton instance'ı kullan
            );

        s_initialized = true;
        SpdlogInfo(mgr.m_logger, "PeriodicQueryManager initialized successfully");
    } else {
        SpdlogCritical(mgr.m_logger, "PeriodicQueryManager: Cannot initialize with null CommandDispatcher!");
    }
}

bool PeriodicQueryManager::isInitialized()
{
    return s_initialized;
}

PeriodicQueryManager::PeriodicQueryManager(QObject* parent)
    : QObject(parent)
    , m_commandDispatcher(nullptr)
    , m_logger(nullptr)
    , m_sequentialProcessor(nullptr)
    , m_running(false)
{
    // Boş yapıcı - initialize() metodunda başlatma gerçekleşir
}

void PeriodicQueryManager::registerQuery(
    const QueryId& id,
    const QueryGroupId& groupId,
    std::shared_ptr<Command> command,
    std::function<void(std::shared_ptr<Response>)> callback,
    int intervalMs,
    Priority priority)
{
    if (!s_initialized) {
        SpdlogWarn(m_logger, "PeriodicQueryManager: Cannot register query, not initialized!");
        return;
    }

    // Sorguyu hazırla
    QueryInfo queryInfo;
    queryInfo.id = id;
    queryInfo.groupId = groupId;
    queryInfo.command = command;
    queryInfo.callback = callback;
    queryInfo.intervalMs = intervalMs;
    queryInfo.priority = priority;
    queryInfo.active = false;
    queryInfo.lastExecutionTime.start();

    // Sorguyu kaydet
    {
        QMutexLocker locker(&m_mutex);
        m_queries[id] = queryInfo;
    }

    // Bu gruba ait bir zamanlayıcı var mı kontrol et
    createQueryGroupIfNotExists(groupId);

    m_logger->info("Query registered - ID: {}, Group: {}, Interval: {} ms",
                   id.toStdString(), groupId.toStdString(), intervalMs);
}

void PeriodicQueryManager::startQueryGroup(const QueryGroupId& groupId)
{
    if (!s_initialized) {
        SpdlogWarn(m_logger, "PeriodicQueryManager: Cannot start query group, not initialized!");
        return;
    }

    QMutexLocker locker(&m_mutex);

    // Grup yoksa oluştur
    createQueryGroupIfNotExists(groupId);

    // Grubu aktifleştir
    if (m_queryGroups.contains(groupId)) {
        auto& group = m_queryGroups[groupId];

        if (!group.active) {
            group.active = true;

            // Grup zamanlayıcısını başlat
            if (group.timer) {
                int timerInterval = group.highFrequencyMode ? 10 : 100;
                group.timer->start(timerInterval);

                m_logger->info("Query group started - Group: {}, Interval: {} ms",
                               groupId.toStdString(), timerInterval);

                // Bu gruptaki tüm sorguları aktifleştir
                for (auto it = m_queries.begin(); it != m_queries.end(); ++it) {
                    if (it.value().groupId == groupId) {
                        it.value().active = true;
                        emit queryStateChanged(it.value().id, true);
                    }
                }

                // İlk çalıştırmayı hemen yap
                QMetaObject::invokeMethod(this, [this, groupId]() {
                    onTimerTick(groupId);
                }, Qt::QueuedConnection);
            }
        }
    }
}

void PeriodicQueryManager::stopQueryGroup(const QueryGroupId& groupId)
{
    if (!s_initialized) {
        SpdlogWarn(m_logger, "PeriodicQueryManager: Cannot stop query group, not initialized!");
        return;
    }

    QMutexLocker locker(&m_mutex);

    if (m_queryGroups.contains(groupId)) {
        auto& group = m_queryGroups[groupId];

        if (group.active) {
            group.active = false;

            // Grup zamanlayıcısını durdur
            if (group.timer) {
                group.timer->stop();

                m_logger->info("Query group stopped - Group: {}", groupId.toStdString());

                // Bu gruptaki tüm sorguları pasif yap
                for (auto it = m_queries.begin(); it != m_queries.end(); ++it) {
                    if (it.value().groupId == groupId) {
                        it.value().active = false;
                        emit queryStateChanged(it.value().id, false);
                    }
                }
            }
        }
    }
}

void PeriodicQueryManager::setQueryInterval(const QueryId& id, int intervalMs)
{
    if (!s_initialized) {
        SpdlogWarn(m_logger, "PeriodicQueryManager: Cannot set query interval, not initialized!");
        return;
    }

    QMutexLocker locker(&m_mutex);

    if (m_queries.contains(id)) {
        m_queries[id].intervalMs = intervalMs;
        m_logger->info("Query interval updated - ID: {}, Interval: {} ms",
                       id.toStdString(), intervalMs);
    }
}

void PeriodicQueryManager::enableHighFrequencyMode(const QueryGroupId& groupId, bool enable)
{
    if (!s_initialized) {
        SpdlogWarn(m_logger, "PeriodicQueryManager: Cannot enable high frequency mode, not initialized!");
        return;
    }

    QMutexLocker locker(&m_mutex);

    if (m_queryGroups.contains(groupId)) {
        auto& group = m_queryGroups[groupId];
        group.highFrequencyMode = enable;

        // Eğer grup aktifse, zamanlayıcıyı güncelle
        if (group.active && group.timer) {
            group.timer->stop();
            int timerInterval = enable ? 10 : 100;
            group.timer->start(timerInterval);

            m_logger->info("High frequency mode {} - Group: {}, Interval: {} ms",
                           enable ? "enabled" : "disabled",
                           groupId.toStdString(), timerInterval);
        }
    }
}

void PeriodicQueryManager::createQueryGroupIfNotExists(const QueryGroupId& groupId)
{
    if (!m_queryGroups.contains(groupId)) {
        QueryGroup group;
        group.id = groupId;
        group.active = false;
        group.highFrequencyMode = false;

        // Grup için zamanlayıcı oluştur
        QTimer* timer = new QTimer(this);
        timer->setTimerType(Qt::PreciseTimer);
        timer->setInterval(100); // Varsayılan aralık: 100ms

        // Zamanlayıcı olayı
        connect(timer, &QTimer::timeout, this, [this, groupId]() {
            onTimerTick(groupId);
        });

        group.timer = timer;
        m_queryGroups[groupId] = group;

        m_logger->info("New query group created - Group: {}", groupId.toStdString());
    }
}

void PeriodicQueryManager::onTimerTick(const QueryGroupId& groupId)
{
    if (!s_initialized) {
        return;
    }

    // Bu fonksiyon zamanlayıcı tarafından tetiklenir
    QList<QueryInfo*> queriesToExecute;

    // İşlenecek sorguları belirle
    {
        QMutexLocker locker(&m_mutex);

        if (!m_queryGroups.contains(groupId) || !m_queryGroups[groupId].active) {
            return;
        }

        auto currentTime = QDateTime::currentMSecsSinceEpoch();
        bool highFrequencyMode = m_queryGroups[groupId].highFrequencyMode;

        // Bu gruptaki ve aktif olan sorguları bul
        for (auto it = m_queries.begin(); it != m_queries.end(); ++it) {
            if (it.value().groupId == groupId && it.value().active) {
                auto& query = it.value();

                // Çalışma zamanını kontrol et
                qint64 elapsed = query.lastExecutionTime.elapsed();

                // Yüksek frekanslı modda sorgunun önceliğine göre farklı aralıklar kullan
                int effectiveIntervalMs = query.intervalMs;
                if (highFrequencyMode) {
                    // Yüksek frekanslı modda, önceliğe göre aralıkları düzelt
                    switch (query.priority) {
                    case Priority::Critical:
                        effectiveIntervalMs = std::min(effectiveIntervalMs, 10);
                        break;
                    case Priority::High:
                        effectiveIntervalMs = std::min(effectiveIntervalMs, 20);
                        break;
                    case Priority::Normal:
                        effectiveIntervalMs = std::min(effectiveIntervalMs, 50);
                        break;
                    default:
                        // Diğer öncelikler için aralığı değiştirme
                        break;
                    }
                }

                // Sorgu zamanı geldiyse işlem listesine ekle
                if (elapsed >= effectiveIntervalMs) {
                    queriesToExecute.append(&query);
                }
            }
        }

        // Önceliğe göre sırala (düşük değer = yüksek öncelik)
        std::sort(queriesToExecute.begin(), queriesToExecute.end(),
                  [](const QueryInfo* a, const QueryInfo* b) {
                      return static_cast<int>(a->priority) < static_cast<int>(b->priority);
                  });
    }

    // Sorguları çalıştır
    for (auto queryPtr : queriesToExecute) {
        executeQuery(*queryPtr);
    }
}

void PeriodicQueryManager::executeQuery(QueryInfo& query)
{
    if (!s_initialized || !m_sequentialProcessor || !m_commandDispatcher) {
        m_logger->warn("Cannot execute query: not properly initialized - ID: {}",
                       query.id.toStdString());
        return;
    }

    // Komutun kopyasını oluştur
    auto command = std::make_shared<Command>(
        query.command->command(),
        query.command->index(),
        query.command->type()
        );

    // Parametreleri kopyala
    if (!query.command->parameters().isEmpty()) {
        for (const auto& param : query.command->parameters()) {
            command->addParameter(param);
        }
    }

    // Değeri kopyala
    command->setValue(query.command->value());

    // Komut ID'sini ayarla
    if (!query.command->id().isEmpty()) {
        command->setId(query.command->id());
    }

    // Sorguyu çalıştır
    m_sequentialProcessor->addCommand(
        command,
        [this, &query](std::shared_ptr<Response> response) {
            // Yanıt zamanını kaydet
            query.lastExecutionTime.restart();

            if (response->isSuccess()) {
                // Başarılı yanıt, callback'i çağır
                query.callback(response);
            } else {
                // Hata durumu
                QString errorMsg = response->errorMessage();
                m_logger->warn("Query error - ID: {}, Error: {}",
                               query.id.toStdString(), errorMsg.toStdString());

                emit queryError(query.id, errorMsg);
            }
        },
        2000 // Sorgular için varsayılan timeout: 2 saniye
        );
}

void PeriodicQueryManager::startAllGroups()
{
    if (!s_initialized) {
        qWarning() << "PeriodicQueryManager: Cannot start all groups, not initialized!";
        return;
    }

    QMutexLocker locker(&m_mutex);

    m_running = true;

    // Tüm grupları başlat
    for (auto it = m_queryGroups.begin(); it != m_queryGroups.end(); ++it) {
        const QueryGroupId& groupId = it.key();

        // Mutex'i geçici olarak serbest bırak ve grubu başlat
        locker.unlock();
        startQueryGroup(groupId);
        locker.relock();
    }

    m_logger->info("All query groups started");
}

void PeriodicQueryManager::stopAllGroups()
{
    if (!s_initialized) {
        qWarning() << "PeriodicQueryManager: Cannot stop all groups, not initialized!";
        return;
    }

    QMutexLocker locker(&m_mutex);

    m_running = false;

    // Tüm grupları durdur
    for (auto it = m_queryGroups.begin(); it != m_queryGroups.end(); ++it) {
        const QueryGroupId& groupId = it.key();

        // Mutex'i geçici olarak serbest bırak ve grubu durdur
        locker.unlock();
        stopQueryGroup(groupId);
        locker.relock();
    }

    m_logger->info("All query groups stopped");
}

bool PeriodicQueryManager::isQueryActive(const QueryId& id) const
{
    if (!s_initialized) {
        return false;
    }

    QMutexLocker locker(&m_mutex);

    if (m_queries.contains(id)) {
        return m_queries[id].active;
    }

    return false;
}

bool PeriodicQueryManager::isGroupActive(const QueryGroupId& groupId) const
{
    if (!s_initialized) {
        return false;
    }

    QMutexLocker locker(&m_mutex);

    if (m_queryGroups.contains(groupId)) {
        return m_queryGroups[groupId].active;
    }

    return false;
}
