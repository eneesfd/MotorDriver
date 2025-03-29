#pragma once

#include <QString>
#include <QVariant>
#include <QDateTime>
#include <memory>
#include <optional>
#include <variant>

/**
 * @brief Yanıt türlerini tanımlayan enum
 */
enum class ResponseType {
    Success,            ///< Başarılı yanıt
    Error,              ///< Hata yanıtı
    Data,               ///< Veri yanıtı
    Text,               /// Yeni: Metin yanıtları için
    Status,             ///< Durum yanıtı
    Version,            ///< Versiyon yanıtı
    Parameter,          ///< Parametre yanıtı
    Position,           ///< Pozisyon yanıtı
    Velocity,           ///< Hız yanıtı
    Motion,             ///< Hareket yanıtı
    Acknowledgement     ///< Onay yanıtı
};

/**
 * @brief Yanıt parametrelerini temsil eden sınıf
 */
class ResponseParameter {
public:
    /**
     * @brief Parametre değeri için kullanılabilecek türler
     */
    using ValueType = std::variant<int, double, bool, QString, QByteArray>;

    /**
     * @brief Boş parametre oluşturur
     */
    ResponseParameter() = default;

    /**
     * @brief İsimli parametre oluşturur
     * @param name Parametre adı
     * @param value Parametre değeri
     */
    ResponseParameter(const QString& name, const ValueType& value);

    /**
     * @brief İsimsiz parametre oluşturur
     * @param value Parametre değeri
     */
    explicit ResponseParameter(const ValueType& value);

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
 * @brief Cihazlardan gelen yanıtları temsil eden sınıf
 */
class Response {
public:
    /**
     * @brief Ham yanıt verisiyle bir yanıt oluşturur
     * @param rawResponse Ham yanıt verisi
     * @param type Yanıt türü
     */
    explicit Response(const QString& rawResponse, ResponseType type = ResponseType::Data);

    /**
     * @brief Komut kimliği ve ham yanıt verisiyle bir yanıt oluşturur
     * @param commandId İlgili komut kimliği
     * @param rawResponse Ham yanıt verisi
     * @param type Yanıt türü
     */
    Response(const QString& commandId, const QString& rawResponse, ResponseType type = ResponseType::Data);

    /**
     * @brief Komut kimliği, yanıt değeri ve komut adıyla bir yanıt oluşturur
     * @param commandId İlgili komut kimliği
     * @param value Yanıt değeri
     * @param command İlgili komut adı
     * @param index Komut indeksi (opsiyonel)
     * @param type Yanıt türü
     */
    Response(const QString& commandId, double value, const QString& command, int index = -1,
            ResponseType type = ResponseType::Data);

    /**
     * @brief İlgili komut kimliğini döndürür
     * @return Komut kimliği
     */
    QString commandId() const;

    /**
     * @brief Komut kimliğini ayarlar
     * @param commandId Komut kimliği
     */
    void setCommandId(const QString& commandId);

    /**
     * @brief Ham yanıt verisini döndürür
     * @return Ham yanıt verisi
     */
    QString rawResponse() const;

    /**
     * @brief Ham yanıt verisini ayarlar
     * @param rawResponse Ham yanıt verisi
     */
    void setRawResponse(const QString& rawResponse);

    /**
     * @brief Yanıt türünü döndürür
     * @return Yanıt türü
     */
    ResponseType type() const;

    /**
     * @brief Yanıt türünü ayarlar
     * @param type Yanıt türü
     */
    void setType(ResponseType type);

    /**
     * @brief Yanıt değerini döndürür
     * @return Yanıt değeri
     */
    double value() const;

    /**
     * @brief Yanıt değerini ayarlar
     * @param value Yanıt değeri
     */
    void setValue(double value);

    /**
     * @brief İlgili komut adını döndürür
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
     * @brief Yanıt zaman damgasını döndürür
     * @return Zaman damgası
     */
    QDateTime timestamp() const;

    /**
     * @brief Zaman damgasını günceller
     */
    void updateTimestamp();

    /**
     * @brief Yanıtın başarılı olup olmadığını döndürür
     * @return Başarılı ise true, değilse false
     */
    bool isSuccess() const;

    /**
     * @brief Yanıtın hata içerip içermediğini döndürür
     * @return Hata varsa true, yoksa false
     */
    bool isError() const;

    /**
     * @brief Hata mesajını döndürür
     * @return Hata mesajı, hata yoksa boş string
     */
    QString errorMessage() const;

    /**
     * @brief Hata mesajını ayarlar
     * @param errorMessage Hata mesajı
     */
    void setErrorMessage(const QString& errorMessage);

    /**
     * @brief Parametre ekler
     * @param param Eklenecek parametre
     */
    void addParameter(const ResponseParameter& param);

    /**
     * @brief İsimli parametre ekler
     * @param name Parametre adı
     * @param value Parametre değeri
     */
    void addParameter(const QString& name, const ResponseParameter::ValueType& value);

    /**
     * @brief İsimsiz parametre ekler
     * @param value Parametre değeri
     */
    void addParameter(const ResponseParameter::ValueType& value);

    /**
     * @brief Parametreleri döndürür
     * @return Parametreler
     */
    QList<ResponseParameter> parameters() const;

    /**
     * @brief İsme göre parametre değerini döndürür
     * @param name Parametre adı
     * @return Parametre değeri, yoksa nullopt
     */
    std::optional<ResponseParameter::ValueType> getParameterValue(const QString& name) const;

    /**
     * @brief İndekse göre parametre değerini döndürür
     * @param index Parametre indeksi
     * @return Parametre değeri, yoksa nullopt
     */
    std::optional<ResponseParameter::ValueType> getParameterValue(int index) const;

    /**
     * @brief Parametreleri temizler
     */
    void clearParameters();

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
     * @brief Yanıtı string olarak formatlar
     * @return Formatlı string
     */
    QString toString() const;

    // Factory metodları
    static std::shared_ptr<Response> createSuccessResponse(const QString& commandId, const QString& rawResponse);
    static std::shared_ptr<Response> createErrorResponse(const QString& commandId, const QString& errorMessage);
    static std::shared_ptr<Response> createDataResponse(const QString& commandId, double value,
                                                        const QString& command, int index = -1);
    static std::shared_ptr<Response> createVersionResponse(const QString& commandId, const QString& version);
    static std::shared_ptr<Response> createPositionResponse(const QString& commandId, double position);
    static std::shared_ptr<Response> createStatusResponse(const QString& commandId, int status);

private:
    QString m_commandId;                                            ///< İlgili komut kimliği
    QString m_rawResponse;                                          ///< Ham yanıt verisi
    ResponseType m_type;                                            ///< Yanıt türü
    double m_value;                                                 ///< Yanıt değeri
    QString m_command;                                              ///< İlgili komut adı
    int m_index;                                                    ///< Komut indeksi
    QDateTime m_timestamp;                                          ///< Zaman damgası
    QString m_errorMessage;                                         ///< Hata mesajı
    QList<ResponseParameter> m_parameters;                          ///< Parametreler
};

using ResponsePtr = std::shared_ptr<Response>;
