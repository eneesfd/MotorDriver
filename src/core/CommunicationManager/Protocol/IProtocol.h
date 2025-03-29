#pragma once

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariant>
#include <memory>
#include <optional>
#include <functional>

class Command;
class Response;

/**
 * @brief Protokol cevap durumlarını temsil eden enum
 */
enum class ResponseStatus {
    Success,            ///< Başarılı
    Error,              ///< Hata durumu
    Timeout,            ///< Zaman aşımı
    InvalidResponse,    ///< Geçersiz yanıt
    ParseError,         ///< Ayrıştırma hatası
    NotSupported        ///< Desteklenmeyen işlem
};

/**
 * @brief Protokol uygulama arayüzü
 *
 * Bu arayüz, farklı protokoller (SimpleIQ, Modbus, CAN, vb.) için ortak bir arayüz sağlar.
 * Haberleşme kanalından bağımsız olarak protokol işlemlerini soyutlar.
 */
class IProtocol : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Protokol yapılandırma bilgilerini içeren struct
     */
    struct Configuration {
        QVariantMap parameters;  ///< Protokol parametreleri
    };

    explicit IProtocol(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IProtocol() = default;

    /**
     * @brief Protokolü yapılandırır
     * @param config Protokol yapılandırması
     * @return Yapılandırma başarılı ise true, değilse false
     */
    virtual bool configure(const Configuration& config) = 0;

    /**
     * @brief Komut gönderir ve yanıtı senkron olarak bekler
     * @param command Gönderilecek komut
     * @param timeoutMs Zaman aşımı süresi (ms)
     * @return Yanıt veya hata durumunda nullopt
     */
    virtual std::optional<std::shared_ptr<Response>> sendCommand(
        const std::shared_ptr<Command>& command, int timeoutMs = 1000) = 0;

    /**
     * @brief Komut gönderir ve yanıtı asenkron olarak callback ile bildirir
     * @param command Gönderilecek komut
     * @param callback Yanıt geldiğinde çağrılacak fonksiyon
     * @param timeoutMs Zaman aşımı süresi (ms)
     * @return İşlem başlatıldıysa true, değilse false
     */
    virtual bool sendCommandAsync(
        const std::shared_ptr<Command>& command,
        std::function<void(std::shared_ptr<Response>)> callback,
        int timeoutMs = 1000) = 0;

    /**
     * @brief Ham veriyi protokole göre ayrıştırır
     * @param data Ham veri
     * @return Ayrıştırılmış yanıt veya hata durumunda nullopt
     */
    virtual std::optional<std::shared_ptr<Response>> parseResponse(
        const QByteArray& data) = 0;

    /**
     * @brief Komutu protokole uygun formatta kodlar
     * @param command Kodlanacak komut
     * @return Kodlanmış veri veya hata durumunda nullopt
     */
    virtual std::optional<QByteArray> encodeCommand(
        const std::shared_ptr<Command>& command) = 0;

    /**
     * @brief Protokolün adını döndürür
     * @return Protokol adı
     */
    virtual QString protocolName() const = 0;

    /**
     * @brief Protokol sürümünü döndürür
     * @return Protokol sürümü
     */
    virtual QString protocolVersion() const = 0;

    /**
     * @brief Son hatayı döndürür
     * @return Hata mesajı
     */
    virtual QString lastError() const = 0;

    /**
     * @brief Ham veri alıcı callback'ini ayarlar
     * @param callback Ham veri alındığında çağrılacak fonksiyon
     */
    virtual void setRawDataHandler(std::function<void(const QByteArray&)> callback) = 0;

signals:
    /**
     * @brief Komut gönderildiğinde yayınlanır
     * @param commandId Komut kimliği
     * @param rawData Ham komut verisi
     */
    void commandSent(const QString& commandId, const QByteArray& rawData);

    /**
     * @brief Yanıt alındığında yayınlanır
     * @param commandId Komut kimliği
     * @param response Alınan yanıt
     * @param status Yanıt durumu
     */
    void responseReceived(const QString& commandId, std::shared_ptr<Response> response, ResponseStatus status);

    /**
     * @brief Ham veri alındığında yayınlanır
     * @param data Ham veri
     */
    void rawDataReceived(const QByteArray& data);

    /**
     * @brief Hata oluştuğunda yayınlanır
     * @param error Hata mesajı
     */
    void errorOccurred(const QString& error);
};

/**
 * @brief Shared pointer türü tanımı
 */
using IProtocolPtr = std::shared_ptr<IProtocol>;
