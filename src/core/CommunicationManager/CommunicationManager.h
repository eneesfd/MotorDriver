#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QTimer>
#include <QMutex>
#include <memory>
#include <functional>
#include <optional>
#include <unordered_map>

#include "Channels/ICommunicationChannel.h"
#include "Protocol/IProtocol.h"
#include "Model/Command.h"
#include "Model/Response.h"
#include "Utils/MessageQueue.h"
#include "spdlog/common.h"

namespace spdlog {
class logger;
}

/**
 * @brief Haberleşme sistemini yöneten ana sınıf
 *
 * Bu sınıf, haberleşme kanalları ve protokoller arasında aracılık yapar,
 * komut gönderme, yanıt işleme ve durum takibi gibi işlemleri yönetir.
 * Facade Design Pattern kullanılarak haberleşme sisteminin karmaşıklığını gizler.
 */
class CommunicationManager : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Haberleşme durumu enum'u
     */
    enum class CommunicationState {
        Disconnected,       ///< Bağlantı yok
        Connecting,         ///< Bağlanıyor
        Connected,          ///< Bağlı
        Disconnecting,      ///< Bağlantı kesiliyor
        Error               ///< Hata durumu
    };

    /**
     * @brief CommunicationManager yapılandırma bilgilerini içeren struct
     */
    struct Configuration {
        QString channelType = "SerialPort";  ///< Kanal türü (SerialPort, Ethernet, vb.)
        QString protocolType = "SimpleIQ";   ///< Protokol türü (SimpleIQ, Modbus, vb.)
        QVariantMap channelParams;           ///< Kanal parametreleri
        QVariantMap protocolParams;          ///< Protokol parametreleri
        int commandTimeoutMs = 1000;         ///< Komut zaman aşımı (ms)
        int statusPollingIntervalMs = 100;   ///< Durum sorgulama aralığı (ms)
        bool autoReconnect = true;           ///< Otomatik yeniden bağlanma
        int autoReconnectIntervalMs = 5000;  ///< Yeniden bağlanma aralığı (ms)
        int maxRetryCount = 3;               ///< Maksimum yeniden deneme sayısı
        bool logTraffic = true;              ///< Trafik loglaması
    };

    /**
     * @brief Komut tamamlama callback türü
     */
    using CommandCallback = std::function<void(const ResponsePtr&)>;

    /**
     * @brief Constructor
     * @param logger Logger
     * @param parent Ebeveyn QObject
     */
    explicit CommunicationManager(std::shared_ptr<spdlog::logger> logger, QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~CommunicationManager() override;

    /**
     * @brief CommunicationManager'ı yapılandırır
     * @param config Yapılandırma bilgileri
     * @return Yapılandırma başarılı ise true, değilse false
     */
    bool configure(const Configuration& config);

    /**
     * @brief Belirtilen kanal ve protokol ile yapılandırır
     * @param channelType Kanal türü
     * @param protocolType Protokol türü
     * @param channelParams Kanal parametreleri
     * @param protocolParams Protokol parametreleri
     * @return Yapılandırma başarılı ise true, değilse false
     */
    bool configure(const QString& channelType, const QString& protocolType,
                   const QVariantMap& channelParams = QVariantMap(),
                   const QVariantMap& protocolParams = QVariantMap());

    /**
     * @brief Haberleşme kanalını bağlar
     * @return Bağlantı başarılı ise true, değilse false
     */
    bool connect();

    /**
     * @brief Haberleşme kanalının bağlantısını keser
     * @return Bağlantı kesme başarılı ise true, değilse false
     */
    bool disconnect();

    /**
     * @brief Haberleşme kanalının bağlı olup olmadığını döndürür
     * @return Bağlı ise true, değilse false
     */
    bool isConnected() const;

    /**
     * @brief Mevcut haberleşme durumunu döndürür
     * @return Haberleşme durumu
     */
    CommunicationState state() const;

    /**
     * @brief Kullanılabilir haberleşme kanallarını listeler
     * @return Kanal listesi
     */
    QVariantList availableChannels() const;

    /**
     * @brief Son hatayı döndürür
     * @return Hata mesajı
     */
    QString lastError() const;

    /**
     * @brief Komut gönderir ve yanıtı senkron olarak bekler
     * @param command Gönderilecek komut
     * @param timeoutMs Zaman aşımı süresi (ms)
     * @return Yanıt veya hata durumunda nullopt
     */
    std::optional<ResponsePtr> sendCommand(const CommandPtr& command, int timeoutMs = -1);

    /**
     * @brief Komut gönderir ve yanıtı asenkron olarak callback ile bildirir
     * @param command Gönderilecek komut
     * @param callback Yanıt geldiğinde çağrılacak fonksiyon
     * @param timeoutMs Zaman aşımı süresi (ms)
     * @return İşlem başlatıldıysa true, değilse false
     */
    bool sendCommandAsync(const CommandPtr& command, CommandCallback callback, int timeoutMs = -1);

    /**
     * @brief MessageQueue'dan ham veri alır ve protokole gönderir
     * @param data Ham veri
     * @return İşlem başarılı ise true, değilse false
     */
    bool processData(const QByteArray& data);

    /**
     * @brief Belirtilen parametre değerini okur
     * @param parameterId Parametre ID'si
     * @return Parametre değeri, hata durumunda nullopt
     */
    std::optional<double> readParameter(int parameterId);

    /**
     * @brief Belirtilen parametreye değer yazar
     * @param parameterId Parametre ID'si
     * @param value Yazılacak değer
     * @return İşlem başarılı ise true, değilse false
     */
    bool writeParameter(int parameterId, double value);

    /**
     * @brief Motor hareketini başlatır
     * @param targetPosition Hedef pozisyon
     * @param speed Hız
     * @return İşlem başarılı ise true, değilse false
     */
    bool startMotion(double targetPosition, double speed);

    /**
     * @brief Motor hareketini durdurur
     * @param emergency Acil durdurma
     * @return İşlem başarılı ise true, değilse false
     */
    bool stopMotion(bool emergency = false);

    /**
     * @brief Motoru referans pozisyonuna getirir (home)
     * @return İşlem başarılı ise true, değilse false
     */
    bool homeMotor();

    /**
     * @brief Motor durumunu sorgular
     * @return İşlem başarılı ise true, değilse false
     */
    bool queryMotorStatus();

    /**
     * @brief Motor durumunu döndürür
     * @return Motor durumu (enabled, moving, error)
     */
    QVariantMap getMotorStatus() const;

    /**
     * @brief Mevcut pozisyonu döndürür
     * @return Mevcut pozisyon
     */
    double getCurrentPosition() const;

    /**
     * @brief Mevcut hızı döndürür
     * @return Mevcut hız
     */
    double getCurrentVelocity() const;

    /**
     * @brief Otomatik durum sorgulama zaman aralığını ayarlar
     * @param intervalMs Zaman aralığı (ms)
     */
    void setStatusPollingInterval(int intervalMs);

    /**
     * @brief Otomatik durum sorgulamayı etkinleştirir/devre dışı bırakır
     * @param enabled Etkinleştirme durumu
     */
    void enableStatusPolling(bool enabled);

    /**
     * @brief Otomatik yeniden bağlanmayı etkinleştirir/devre dışı bırakır
     * @param enabled Etkinleştirme durumu
     * @param intervalMs Yeniden bağlanma aralığı (ms)
     */
    void enableAutoReconnect(bool enabled, int intervalMs = -1);

    /**
     * @brief Trafik loglamasını etkinleştirir/devre dışı bırakır
     * @param enabled Etkinleştirme durumu
     */
    void enableTrafficLogging(bool enabled);

    /**
     * @brief Mevcut yapılandırmayı döndürür
     * @return Yapılandırma
     */
    Configuration getConfiguration() const;

    /**
     * @brief Haberleşme kanalını döndürür
     * @return Haberleşme kanalı
     */
    ICommunicationChannelPtr getChannel() const;

    /**
     * @brief Protokolü döndürür
     * @return Protokol
     */
    IProtocolPtr getProtocol() const;

signals:
    /**
     * @brief Haberleşme durumu değiştiğinde yayınlanır
     * @param state Yeni durum
     */
    void stateChanged(CommunicationState state);

    /**
     * @brief Komut gönderildiğinde yayınlanır
     * @param commandId Komut kimliği
     * @param command Komut
     */
    void commandSent(const QString& commandId, const CommandPtr& command);

    /**
     * @brief Yanıt alındığında yayınlanır
     * @param commandId Komut kimliği
     * @param response Yanıt
     */
    void responseReceived(const QString& commandId, const ResponsePtr& response);

    /**
     * @brief Ham veri alındığında yayınlanır
     * @param data Ham veri
     */
    void rawDataReceived(const QByteArray& data);

    /**
     * @brief Ham veri gönderildiğinde yayınlanır
     * @param data Ham veri
     */
    void rawDataSent(const QByteArray& data);

    /**
     * @brief Motor durumu değiştiğinde yayınlanır
     * @param status Motor durumu
     */
    void motorStatusChanged(const QVariantMap& status);

    /**
     * @brief Pozisyon değiştiğinde yayınlanır
     * @param position Yeni pozisyon
     */
    void positionChanged(double position);

    /**
     * @brief Hız değiştiğinde yayınlanır
     * @param velocity Yeni hız
     */
    void velocityChanged(double velocity);

    /**
     * @brief Hata oluştuğunda yayınlanır
     * @param error Hata mesajı
     */
    void errorOccurred(const QString& error);

    /**
     * @brief Bağlantı durumu değiştiğinde yayınlanır
     * @param connected Bağlantı durumu
     */
    void connectionStatusChanged(bool connected);

private slots:
    void handleChannelStateChanged(ChannelState state);
    void handleChannelDataReceived(const QByteArray& data);
    void handleChannelErrorOccurred(const QString& error);
    void handleProtocolResponseReceived(const QString& commandId, std::shared_ptr<Response> response, ResponseStatus status);
    void handleProtocolErrorOccurred(const QString& error);
    void pollStatus();
    void attemptReconnect();
    void checkCommandTimeouts();

private:
    // Singleton instance
    static CommunicationManager* s_instance;

    // Yapılandırma ve durum
    Configuration m_config;
    CommunicationState m_state = CommunicationState::Disconnected;
    QString m_lastError;
    bool m_statusPollingEnabled = false;
    bool m_autoReconnectEnabled = false;
    bool m_trafficLoggingEnabled = false;

    // Bileşenler
    ICommunicationChannelPtr m_channel;
    IProtocolPtr m_protocol;
    MessageQueuePtr m_messageQueue;

    // Logger
    std::shared_ptr<spdlog::logger> m_logger;

    // Bekleyen komutlar
    struct PendingCommand {
        CommandPtr command;
        CommandCallback callback;
        QDateTime timestamp;
        int timeoutMs;
        int retryCount = 0;
    };

    std::unordered_map<QString, PendingCommand> m_pendingCommands;
    QMutex m_pendingCommandsMutex;

    // Motor durumu
    struct MotorStatus {
        bool enabled = false;
        bool moving = false;
        bool error = false;
        double position = 0.0;
        double velocity = 0.0;
        double torque = 0.0;
        QString errorMessage;
    };

    MotorStatus m_motorStatus;
    mutable QMutex m_motorStatusMutex;

    // Zamanlayıcılar
    QTimer m_statusPollingTimer;
    QTimer m_reconnectTimer;
    QTimer m_timeoutTimer;

    // Son kanal yapılandırması (yeniden bağlanma için)
    QString m_lastChannelType;
    QVariantMap m_lastChannelParams;

    // Factory metodları
    ICommunicationChannelPtr createChannel(const QString& channelType);
    IProtocolPtr createProtocol(const QString& protocolType);

    // Yardımcı iç metodlar
    void updateState(CommunicationState newState);
    void updateMotorStatus(const ResponsePtr& response);
    void logMessage(const QString& message, spdlog::level::level_enum level);
    void logTraffic(bool isSending, const QByteArray& data);
    void clearPendingCommands(const QString& reason = "");
    void handleCommandTimeout(const QString& commandId);
    void handleCommandComplete(const QString& commandId, const ResponsePtr& response);
    int getDefaultCommandTimeout() const;
    bool createDefaultChannel();
};
