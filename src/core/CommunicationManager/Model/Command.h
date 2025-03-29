#pragma once

#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QObject>
#include <memory>
#include <optional>
#include <variant>

/**
 * @brief Komut türlerini tanımlayan enum
 */
enum class CommandType {
    Read,               ///< Okuma komutu
    Write,              ///< Yazma komutu
    Motion,             ///< Hareket komutu
    Query,              ///< Sorgulama komutu
    System,             ///< Sistem komutu
    Control,            ///< Kontrol komutu
    Configuration       ///< Yapılandırma komutu
};

/**
 * @brief Komut parametrelerini temsil eden sınıf
 */
class CommandParameter {
public:
    /**
     * @brief Parametre değeri için kullanılabilecek türler
     */
    using ValueType = std::variant<int, double, bool, QString, QByteArray>;

    /**
     * @brief Boş parametre oluşturur
     */
    CommandParameter() = default;

    /**
     * @brief İsimli parametre oluşturur
     * @param name Parametre adı
     * @param value Parametre değeri
     */
    CommandParameter(const QString& name, const ValueType& value);

    /**
     * @brief İsimsiz parametre oluşturur
     * @param value Parametre değeri
     */
    explicit CommandParameter(const ValueType& value);

    /**
     * @brief Parametre adını döndürür
     * @return Parametre adı
     */
    QString name() const;

    /**
     * @brief Parametre adını ayarlar
     * @param name Parametre adı
     */
    void setName(const QString& name);

    /**
     * @brief Parametre değerini döndürür
     * @return Parametre değeri
     */
    ValueType value() const;

    /**
     * @brief Parametre değerini ayarlar
     * @param value Parametre değeri
     */
    void setValue(const ValueType& value);

    /**
     * @brief int türünde değer döndürür
     * @return int değer, değer int değilse nullopt
     */
    std::optional<int> intValue() const;

    /**
     * @brief double türünde değer döndürür
     * @return double değer, değer double değilse nullopt
     */
    std::optional<double> doubleValue() const;

    /**
     * @brief bool türünde değer döndürür
     * @return bool değer, değer bool değilse nullopt
     */
    std::optional<bool> boolValue() const;

    /**
     * @brief QString türünde değer döndürür
     * @return QString değer, değer QString değilse nullopt
     */
    std::optional<QString> stringValue() const;

    /**
     * @brief QByteArray türünde değer döndürür
     * @return QByteArray değer, değer QByteArray değilse nullopt
     */
    std::optional<QByteArray> byteArrayValue() const;

    /**
     * @brief Parametreyi string olarak formatlar
     * @return Formatlı string
     */
    QString toString() const;

private:
    QString m_name;     ///< Parametre adı
    ValueType m_value;  ///< Parametre değeri
};

/**
 * @brief Cihazlara gönderilecek komutları temsil eden sınıf
 */
class Command {
public:
    /**
     * @brief Değer ve komut ile bir komut oluşturur
     * @param value Değer
     * @param command Komut adı
     * @param index Komut indeksi (opsiyonel)
     * @param type Komut türü
     */
    Command(double value, const QString& command, int index = -1,
            CommandType type = CommandType::Write);

    /**
     * @brief Sadece komut adı ile bir komut oluşturur
     * @param command Komut adı
     * @param type Komut türü
     */
    explicit Command(const QString& command, int index, CommandType type = CommandType::Query);

    /**
     * @brief Komut kimliğini döndürür
     * @return Komut kimliği
     */
    QString id() const;

    /**
     * @brief Komut kimliği ayarlar
     * @param id Komut kimliği
     */
    void setId(const QString& id);

    /**
     * @brief Komut adını döndürür
     * @return Komut adı
     */
    QString command() const;

    /**
     * @brief Komut adını ayarlar
     * @param command Komut adı
     */
    void setCommand(const QString& command);

    /**
     * @brief Komut indeksini döndürür
     * @return Komut indeksi, indeks yoksa -1
     */
    int index() const;

    /**
     * @brief Komut indeksini ayarlar
     * @param index Komut indeksi
     */
    void setIndex(int index);

    /**
     * @brief Komut türünü döndürür
     * @return Komut türü
     */
    CommandType type() const;

    /**
     * @brief Komut türünü ayarlar
     * @param type Komut türü
     */
    void setType(CommandType type);

    /**
     * @brief Komut değerini döndürür
     * @return Komut değeri
     */
    double value() const;

    /**
     * @brief Komut değerini ayarlar
     * @param value Komut değeri
     */
    void setValue(double value);

    /**
     * @brief Komut zaman damgasını döndürür
     * @return Zaman damgası
     */
    QDateTime timestamp() const;

    /**
     * @brief Zaman damgasını günceller
     */
    void updateTimestamp();

    /**
     * @brief Parametre ekler
     * @param param Eklenecek parametre
     */
    void addParameter(const CommandParameter& param);

    /**
     * @brief İsimli parametre ekler
     * @param name Parametre adı
     * @param value Parametre değeri
     */
    void addParameter(const QString& name, const CommandParameter::ValueType& value);

    /**
     * @brief İsimsiz parametre ekler
     * @param value Parametre değeri
     */
    void addParameter(const CommandParameter::ValueType& value);

    /**
     * @brief Parametreleri döndürür
     * @return Parametreler
     */
    QList<CommandParameter> parameters() const;

    /**
     * @brief Parametreleri temizler
     */
    void clearParameters();

    /**
     * @brief Komut açıklamasını döndürür
     * @return Komut açıklaması
     */
    QString description() const;

    /**
     * @brief Komut açıklamasını ayarlar
     * @param description Komut açıklaması
     */
    void setDescription(const QString& description);

    /**
     * @brief Komutu string olarak formatlar
     * @return Formatlı string
     */
    QString toString() const;

    /**
     * @brief String gösterimini ayarlar
     * @param formatter Formatlayıcı fonksiyon
     */
    static void setFormatter(std::function<QString(const Command&)> formatter);

    // SimpleIQ komutları için factory metodları
    static std::shared_ptr<Command> createVersionRequest();
    static std::shared_ptr<Command> createParameterCommand(int index, double value);
    static std::shared_ptr<Command> createPositionRequest();
    static std::shared_ptr<Command> createSpeedCommand(double speed);
    static std::shared_ptr<Command> createAccelerationCommand(int accel);
    static std::shared_ptr<Command> createDecelerationCommand(int decel);
    static std::shared_ptr<Command> createStopCommand(bool emergency = false);
    static std::shared_ptr<Command> createMotionCommand(double position, double speed);
    static std::shared_ptr<Command> createHomeCommand();

private:
    QString m_id;                                                   ///< Komut kimliği
    QString m_command;                                              ///< Komut adı
    int m_index;                                                    ///< Komut indeksi
    CommandType m_type;                                             ///< Komut türü
    double m_value;                                                 ///< Komut değeri
    QDateTime m_timestamp;                                          ///< Zaman damgası
    QString m_description;                                          ///< Komut açıklaması
    QList<CommandParameter> m_parameters;                           ///< Parametreler

    static std::function<QString(const Command&)> s_formatter;      ///< String formatlaması için fonksiyon
};

using CommandPtr = std::shared_ptr<Command>;
