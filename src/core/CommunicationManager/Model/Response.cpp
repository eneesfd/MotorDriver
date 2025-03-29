#include "Response.h"
#include <memory>
#include <QRegularExpression>

// ResponseParameter sınıfı implementasyonu
ResponseParameter::ResponseParameter(const QString& name, const ValueType& value)
    : m_name(name),
    m_value(value)
{
}

ResponseParameter::ResponseParameter(const ValueType& value)
    : m_value(value)
{
}

QString ResponseParameter::name() const
{
    return m_name;
}

void ResponseParameter::setName(const QString& name)
{
    m_name = name;
}

ResponseParameter::ValueType ResponseParameter::value() const
{
    return m_value;
}

void ResponseParameter::setValue(const ValueType& value)
{
    m_value = value;
}

std::optional<int> ResponseParameter::intValue() const
{
    if (std::holds_alternative<int>(m_value)) {
        return std::get<int>(m_value);
    }

    // Double değerden int'e dönüştürme denemesi
    if (std::holds_alternative<double>(m_value)) {
        return static_cast<int>(std::get<double>(m_value));
    }

    // String'den int'e dönüştürme denemesi
    if (std::holds_alternative<QString>(m_value)) {
        bool ok;
        int value = std::get<QString>(m_value).toInt(&ok);
        if (ok) {
            return value;
        }
    }

    return std::nullopt;
}

std::optional<double> ResponseParameter::doubleValue() const
{
    if (std::holds_alternative<double>(m_value)) {
        return std::get<double>(m_value);
    }

    // Int değerden double'a dönüştürme
    if (std::holds_alternative<int>(m_value)) {
        return static_cast<double>(std::get<int>(m_value));
    }

    // String'den double'a dönüştürme denemesi
    if (std::holds_alternative<QString>(m_value)) {
        bool ok;
        double value = std::get<QString>(m_value).toDouble(&ok);
        if (ok) {
            return value;
        }
    }

    return std::nullopt;
}

std::optional<bool> ResponseParameter::boolValue() const
{
    if (std::holds_alternative<bool>(m_value)) {
        return std::get<bool>(m_value);
    }

    // Int değerden bool'a dönüştürme (0 = false, diğer = true)
    if (std::holds_alternative<int>(m_value)) {
        return std::get<int>(m_value) != 0;
    }

    // String'den bool'a dönüştürme denemesi
    if (std::holds_alternative<QString>(m_value)) {
        QString str = std::get<QString>(m_value).toLower();
        if (str == "true" || str == "yes" || str == "1" || str == "on") {
            return true;
        } else if (str == "false" || str == "no" || str == "0" || str == "off") {
            return false;
        }
    }

    return std::nullopt;
}

std::optional<QString> ResponseParameter::stringValue() const
{
    if (std::holds_alternative<QString>(m_value)) {
        return std::get<QString>(m_value);
    }

    // Diğer türlerden QString'e dönüştürme
    if (std::holds_alternative<int>(m_value)) {
        return QString::number(std::get<int>(m_value));
    }

    if (std::holds_alternative<double>(m_value)) {
        return QString::number(std::get<double>(m_value), 'g', 15);
    }

    if (std::holds_alternative<bool>(m_value)) {
        return std::get<bool>(m_value) ? "true" : "false";
    }

    if (std::holds_alternative<QByteArray>(m_value)) {
        return QString::fromUtf8(std::get<QByteArray>(m_value));
    }

    return std::nullopt;
}

std::optional<QByteArray> ResponseParameter::byteArrayValue() const
{
    if (std::holds_alternative<QByteArray>(m_value)) {
        return std::get<QByteArray>(m_value);
    }

    // QString'den QByteArray'e dönüştürme
    if (std::holds_alternative<QString>(m_value)) {
        return std::get<QString>(m_value).toUtf8();
    }

    return std::nullopt;
}

QString ResponseParameter::toString() const
{
    QString result;

    if (!m_name.isEmpty()) {
        result = m_name + ": ";
    }

    if (std::holds_alternative<int>(m_value)) {
        result += QString::number(std::get<int>(m_value));
    } else if (std::holds_alternative<double>(m_value)) {
        result += QString::number(std::get<double>(m_value), 'g', 15);
    } else if (std::holds_alternative<bool>(m_value)) {
        result += std::get<bool>(m_value) ? "true" : "false";
    } else if (std::holds_alternative<QString>(m_value)) {
        result += std::get<QString>(m_value);
    } else if (std::holds_alternative<QByteArray>(m_value)) {
        result += QString::fromUtf8(std::get<QByteArray>(m_value));
    }

    return result;
}

// Response sınıfı implementasyonu
Response::Response(const QString& rawResponse, ResponseType type)
    : m_commandId(""),
    m_rawResponse(rawResponse),
    m_type(type),
    m_value(0.0),
    m_command(""),
    m_index(-1),
    m_timestamp(QDateTime::currentDateTime()),
    m_errorMessage("")
{
    // SimpleIQ yanıtı ayrıştırma
    static QRegularExpression regex(R"((\d+);([A-Za-z]{2,3})(?:\[(\d+)\])?(?:\r|\n|\r\n)?)");
    QRegularExpressionMatch match = regex.match(rawResponse);

    if (match.hasMatch()) {
        m_value = match.captured(1).toDouble();
        m_command = match.captured(2);

        if (match.lastCapturedIndex() > 2 && !match.captured(3).isEmpty()) {
            m_index = match.captured(3).toInt();
        }
    }
}

Response::Response(const QString& commandId, const QString& rawResponse, ResponseType type)
    : m_commandId(commandId),
    m_rawResponse(rawResponse),
    m_type(type),
    m_value(0.0),
    m_command(""),
    m_index(-1),
    m_timestamp(QDateTime::currentDateTime()),
    m_errorMessage("")
{
    // SimpleIQ yanıtı ayrıştırma
    static QRegularExpression regex(R"((\d+);([A-Za-z]{2,3})(?:\[(\d+)\])?(?:\r|\n|\r\n)?)");
    QRegularExpressionMatch match = regex.match(rawResponse);

    if (match.hasMatch()) {
        m_value = match.captured(1).toDouble();
        m_command = match.captured(2);

        if (match.lastCapturedIndex() > 2 && !match.captured(3).isEmpty()) {
            m_index = match.captured(3).toInt();
        }
    }
}

Response::Response(const QString& commandId, double value, const QString& command, int index, ResponseType type)
    : m_commandId(commandId),
    m_rawResponse(""),
    m_type(type),
    m_value(value),
    m_command(command),
    m_index(index),
    m_timestamp(QDateTime::currentDateTime()),
    m_errorMessage("")
{
    // Ham yanıt oluştur
    if (index >= 0) {
        m_rawResponse = QString("%1;%2[%3]").arg(value).arg(command).arg(index);
    } else {
        m_rawResponse = QString("%1;%2").arg(value).arg(command);
    }
}

QString Response::commandId() const
{
    return m_commandId;
}

void Response::setCommandId(const QString& commandId)
{
    m_commandId = commandId;
}

QString Response::rawResponse() const
{
    return m_rawResponse;
}

void Response::setRawResponse(const QString& rawResponse)
{
    m_rawResponse = rawResponse;

    // SimpleIQ yanıtı ayrıştırma
    QRegularExpression regex(R"((\d+);([A-Za-z]{2,3})(?:\[(\d+)\])?(?:\r|\n|\r\n)?)");
    QRegularExpressionMatch match = regex.match(rawResponse);

    if (match.hasMatch()) {
        m_value = match.captured(1).toDouble();
        m_command = match.captured(2);

        if (match.lastCapturedIndex() > 2 && !match.captured(3).isEmpty()) {
            m_index = match.captured(3).toInt();
        }
    }
}

ResponseType Response::type() const
{
    return m_type;
}

void Response::setType(ResponseType type)
{
    m_type = type;
}

double Response::value() const
{
    return m_value;
}

void Response::setValue(double value)
{
    m_value = value;
}

QString Response::command() const
{
    return m_command;
}

void Response::setCommand(const QString& command)
{
    m_command = command;
}

int Response::index() const
{
    return m_index;
}

void Response::setIndex(int index)
{
    m_index = index;
}

QDateTime Response::timestamp() const
{
    return m_timestamp;
}

void Response::updateTimestamp()
{
    m_timestamp = QDateTime::currentDateTime();
}

bool Response::isSuccess() const
{
    return m_type != ResponseType::Error;
}

bool Response::isError() const
{
    return m_type == ResponseType::Error;
}

QString Response::errorMessage() const
{
    return m_errorMessage;
}

void Response::setErrorMessage(const QString& errorMessage)
{
    m_errorMessage = errorMessage;

    // Hata mesajı ayarlandıysa, tür de hata olarak ayarlanmalı
    if (!errorMessage.isEmpty()) {
        m_type = ResponseType::Error;
    }
}

void Response::addParameter(const ResponseParameter& param)
{
    m_parameters.append(param);
}

void Response::addParameter(const QString& name, const ResponseParameter::ValueType& value)
{
    m_parameters.append(ResponseParameter(name, value));
}

void Response::addParameter(const ResponseParameter::ValueType& value)
{
    m_parameters.append(ResponseParameter(value));
}

QList<ResponseParameter> Response::parameters() const
{
    return m_parameters;
}

std::optional<ResponseParameter::ValueType> Response::getParameterValue(const QString& name) const
{
    for (const auto& param : m_parameters) {
        if (param.name() == name) {
            return param.value();
        }
    }

    return std::nullopt;
}

std::optional<ResponseParameter::ValueType> Response::getParameterValue(int index) const
{
    if (index >= 0 && index < m_parameters.size()) {
        return m_parameters[index].value();
    }

    return std::nullopt;
}

void Response::clearParameters()
{
    m_parameters.clear();
}

std::optional<int> Response::intValue() const
{
    return static_cast<int>(m_value);
}

std::optional<double> Response::doubleValue() const
{
    return m_value;
}

std::optional<bool> Response::boolValue() const
{
    return m_value != 0.0;
}

std::optional<QString> Response::stringValue() const
{
    // Değer string olarak dönüştürülür
    return m_rawResponse;
}

QString Response::toString() const
{
    QString result;

    if (!m_commandId.isEmpty()) {
        result += "CommandID: " + m_commandId + "\n";
    }

    result += "Command: " + m_command;

    if (m_index >= 0) {
        result += "[" + QString::number(m_index) + "]";
    }

    result += "\nValue: " + QString::number(m_value);

    if (!m_parameters.isEmpty()) {
        result += "\nParameters:";
        for (const auto& param : m_parameters) {
            result += "\n  " + param.toString();
        }
    }

    if (isError()) {
        result += "\nError: " + m_errorMessage;
    }

    return result;
}

// Factory metodları
std::shared_ptr<Response> Response::createSuccessResponse(const QString& commandId, const QString& rawResponse)
{
    auto response = std::make_shared<Response>(commandId, rawResponse, ResponseType::Success);
    return response;
}

std::shared_ptr<Response> Response::createErrorResponse(const QString& commandId, const QString& errorMessage)
{
    auto response = std::make_shared<Response>(commandId, 0.0, "ERR", -1, ResponseType::Error);
    response->setErrorMessage(errorMessage);
    return response;
}

std::shared_ptr<Response> Response::createDataResponse(const QString& commandId, double value, const QString& command, int index)
{
    return std::make_shared<Response>(commandId, value, command, index, ResponseType::Data);
}

std::shared_ptr<Response> Response::createVersionResponse(const QString& commandId, const QString& version)
{
    auto response = std::make_shared<Response>(commandId, 0.0, "VR", -1, ResponseType::Version);
    response->addParameter("version", version);
    return response;
}

std::shared_ptr<Response> Response::createPositionResponse(const QString& commandId, double position)
{
    return std::make_shared<Response>(commandId, position, "PS", -1, ResponseType::Position);
}

std::shared_ptr<Response> Response::createStatusResponse(const QString& commandId, int status)
{
    auto response = std::make_shared<Response>(commandId, status, "SC", -1, ResponseType::Status);

    // Durum kodundan motor durumunu çıkar
    bool enabled = (status & 0x01) != 0;
    bool moving = (status & 0x02) != 0;
    bool error = (status & 0x04) != 0;

    response->addParameter("enabled", enabled);
    response->addParameter("moving", moving);
    response->addParameter("error", error);

    return response;
}
