#pragma once

#include "IProtocol.h"
#include "../Model/Command.h"
#include "../Model/Response.h"
#include <QMap>
#include <QTimer>
#include <QMutex>
#include <QRegularExpression>
#include <memory>
#include <unordered_map>
#include <queue>

/**
 * @brief SimpleIQ protokolü uygulayan sınıf
 *
 * Bu sınıf, motor sürücüler için SimpleIQ protokolünü uygular.
 * SimpleIQ formatı: DEĞER;KOMUT[INDEX]<CR>
 */
class SimpleIQProtocol : public IProtocol {
    Q_OBJECT
public:
    /**
     * @brief SimpleIQ protokolü için yapılandırma bilgilerini içeren struct
     */
    struct SimpleIQConfiguration : public Configuration {
        int commandDelayMs = 5;          ///< Komutlar arası gecikme (ms)
        int responseTimeoutMs = 1000;    ///< Yanıt bekleme zaman aşımı (ms)
        int maxRetryCount = 3;           ///< Maksimum yeniden deneme sayısı
        bool autoRetry = true;           ///< Otomatik yeniden deneme
        QString terminationChar = "\r";  ///< Komut sonlandırma karakteri (CR)
        bool trimResponse = true;        ///< Yanıtlardan boşlukları temizle
    };

    /**
     * @brief Constructor
     * @param parent Ebeveyn QObject
     */
    explicit SimpleIQProtocol(QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~SimpleIQProtocol() override;

    // IProtocol arayüz uygulaması
    bool configure(const Configuration& config) override;
    std::optional<std::shared_ptr<Response>> sendCommand(
        const std::shared_ptr<Command>& command, int timeoutMs = 1000) override;
    bool sendCommandAsync(
        const std::shared_ptr<Command>& command,
        std::function<void(std::shared_ptr<Response>)> callback,
        int timeoutMs = 1000) override;
    std::optional<std::shared_ptr<Response>> parseResponse(
        const QByteArray& data) override;
    std::optional<QByteArray> encodeCommand(
        const std::shared_ptr<Command>& command) override;
    QString protocolName() const override;
    QString protocolVersion() const override;
    QString lastError() const override;
    void setRawDataHandler(std::function<void(const QByteArray&)> callback) override;

    /**
     * @brief Ham veri işler
     * @param data İşlenecek veri
     */
    void processRawData(const QByteArray& data);

    /**
     * @brief Kanal üzerinden gönderim yapmak için callback ayarlar
     * @param sendCallback Gönderim callback'i
     */
    void setChannelSendCallback(std::function<bool(const QByteArray&)> sendCallback);

private slots:
    void checkPendingCommands();
    void processResponseQueue();

private:
    // Yapılandırma ve durum
    SimpleIQConfiguration m_config;
    QString m_lastError;
    std::function<void(const QByteArray&)> m_rawDataHandler;
    std::function<bool(const QByteArray&)> m_channelSendCallback;

    // Bekleyen komutlar
    struct PendingCommand {
        std::shared_ptr<Command> command;
        std::function<void(std::shared_ptr<Response>)> callback;
        QDateTime timestamp;
        int timeoutMs;
        int retryCount;
        bool processed{false};
    };

    QMap<QString, PendingCommand> m_pendingCommands;
    QMutex m_pendingCommandsMutex;

    // Yanıt kuyruğu
    std::queue<QByteArray> m_responseQueue;
    QMutex m_responseQueueMutex;

    // Zamanlayıcılar
    QTimer m_timeoutTimer;
    QTimer m_processingTimer;

    // Yanıt ayırma için regex
    QRegularExpression m_numericResponseRegex;  // Sayısal yanıtlar için
    QRegularExpression m_textResponseRegex;     // Metin yanıtları için

    // Komut kimliği sayacı
    int m_commandCounter = 0;

    // Yardımcı iç metodlar
    QString generateCommandId();
    void handleCommandTimeout(const QString& commandId);
    void handleResponseForCommand(const QString& commandId, std::shared_ptr<Response> response);
    SimpleIQConfiguration parseConfig(const Configuration& config);
    std::pair<QString, int> extractCommandAndIndex(const QString& commandPart) const;
    std::shared_ptr<Response> createErrorResponse(const QString& commandId, const QString& error);
    QMutex m_processingMutex; // Yanıt işleme için kilit
    bool m_isProcessingResponse = false; // Yanıt işleme durumu

    struct PendingResponse {
        std::atomic<bool> received{false};
        std::shared_ptr<Response> response;
    };

    QMap<QString, std::shared_ptr<PendingResponse>> m_pendingResponses;
    QMutex m_pendingResponsesMutex;
};
