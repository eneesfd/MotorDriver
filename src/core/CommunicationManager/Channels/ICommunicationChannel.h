#ifndef ICOMMUNICATIONCHANNEL_H
#define ICOMMUNICATIONCHANNEL_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariant>
#include <memory>
#include <optional>
#include <functional>

/**
 * @brief Haberleşme kanalı durumları
 */
enum class ChannelState {
    Disconnected,    ///< Bağlantı kapalı
    Connecting,      ///< Bağlanıyor
    Connected,       ///< Bağlantı açık
    Error            ///< Hata durumu
};

/**
 * @brief Haberleşme kanalı arayüzü
 *
 * Bu arayüz, fiziksel haberleşme kanallarını (RS232, Ethernet, USB vb.) soyutlar.
 * Farklı haberleşme kanalları için tek bir ortak arayüz sağlar.
 */
class ICommunicationChannel : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Yapılandırma bilgilerini içeren struct
     */
    struct Configuration {
        virtual ~Configuration() = default;
        QVariantMap parameters;  ///< Kanal parametreleri
    };


    explicit ICommunicationChannel(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ICommunicationChannel() = default;

    /**
     * @brief Kanalı yapılandırır
     * @param config Kanal yapılandırması
     * @return Yapılandırma başarılı ise true, değilse false
     */
    virtual bool configure(const Configuration& config) = 0;

    /**
     * @brief Bağlantıyı açar
     * @return Bağlantı başarılı ise true, değilse false
     */
    virtual bool open() = 0;

    /**
     * @brief Bağlantıyı kapatır
     * @return Kapama başarılı ise true, değilse false
     */
    virtual bool close() = 0;

    /**
     * @brief Veri gönderir
     * @param data Gönderilecek veriler
     * @return Gönderilen byte sayısı, hata durumunda -1
     */
    virtual qint64 write(const QByteArray& data) = 0;

    /**
     * @brief Kanaldan veri okur
     * @param maxSize Okunacak maksimum byte sayısı
     * @return Okunan veriler, veri yoksa veya hata durumunda boş optional
     */
    virtual std::optional<QByteArray> read(qint64 maxSize = -1) = 0;

    /**
     * @brief Mevcut bağlantı durumunu döndürür
     * @return Bağlantı durumu
     */
    virtual ChannelState state() const = 0;

    /**
     * @brief Kullanılabilir haberleşme kanallarını listeler
     * @return Kanal listesi
     */
    virtual QVariantList availableChannels() const = 0;

    /**
     * @brief Son hatayı döndürür
     * @return Hata mesajı
     */
    virtual QString lastError() const = 0;

    /**
     * @brief Mesaj alıcı callback'ini ayarlar
     * @param callback Mesaj alındığında çağrılacak fonksiyon
     */
    virtual void setMessageHandler(std::function<void(const QByteArray&)> callback) = 0;

    /**
     * @brief Bağlantı durumunu döndürür
     * @return Bağlı ise true, değilse false
     */
    virtual bool isConnected() const = 0;

signals:
    /**
     * @brief Bağlantı durumu değiştiğinde yayınlanır
     * @param state Yeni bağlantı durumu
     */
    void stateChanged(ChannelState state);

    /**
     * @brief Yeni veri alındığında yayınlanır
     * @param data Alınan veriler
     */
    void dataReceived(const QByteArray& data);

    /**
     * @brief Veri gönderildiğinde yayınlanır
     * @param bytes Gönderilen byte sayısı
     */
    void dataSent(qint64 bytes);

    /**
     * @brief Hata oluştuğunda yayınlanır
     * @param error Hata mesajı
     */
    void errorOccurred(const QString& error);
};

/**
 * @brief Shared pointer türü tanımı
 */
using ICommunicationChannelPtr = QSharedPointer<ICommunicationChannel>;

#endif // ICOMMUNICATIONCHANNEL_H
