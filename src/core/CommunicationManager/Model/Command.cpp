#include "Command.h"
#include <memory>
#include <QUuid>
#include <QStringList>

// Static formatter fonksiyonu
std::function<QString(const Command&)> Command::s_formatter = nullptr;

// CommandParameter sınıfı implementasyonu
CommandParameter::CommandParameter(const QString& name, const ValueType& value)
    : m_name(name),
    m_value(value)
{
}

CommandParameter::CommandParameter(const ValueType& value)
    : m_value(value)
{
}

QString CommandParameter::name() const
{
    return m_name;
}

void CommandParameter::setName(const QString& name)
{
    m_name = name;
}

CommandParameter::ValueType CommandParameter::value() const
{
    return m_value;
}

void CommandParameter::setValue(const ValueType& value)
{
    m_value = value;
}

std::optional<int> CommandParameter::intValue() const
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

std::optional<double> CommandParameter::doubleValue() const
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

std::optional<bool> CommandParameter::boolValue() const
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

std::optional<QString> CommandParameter::stringValue() const
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

std::optional<QByteArray> CommandParameter::byteArrayValue() const
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

QString CommandParameter::toString() const
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

// Command sınıfı implementasyonu
Command::Command(double value, const QString& command, int index, CommandType type)
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces).left(8)),
    m_command(command),
    m_index(index),
    m_type(type),
    m_value(value),
    m_timestamp(QDateTime::currentDateTime())
{
}

Command::Command(const QString& command, int index, CommandType type)
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces).left(8)),
    m_command(command),
    m_index(index),
    m_type(type),
    m_value(0.0),
    m_timestamp(QDateTime::currentDateTime())
{
}

QString Command::id() const
{
    return m_id;
}

void Command::setId(const QString& id)
{
    m_id = id;
}

QString Command::command() const
{
    return m_command;
}

void Command::setCommand(const QString& command)
{
    m_command = command;
}

int Command::index() const
{
    return m_index;
}

void Command::setIndex(int index)
{
    m_index = index;
}

CommandType Command::type() const
{
    return m_type;
}

void Command::setType(CommandType type)
{
    m_type = type;
}

double Command::value() const
{
    return m_value;
}

void Command::setValue(double value)
{
    m_value = value;
}

QDateTime Command::timestamp() const
{
    return m_timestamp;
}

void Command::updateTimestamp()
{
    m_timestamp = QDateTime::currentDateTime();
}

void Command::addParameter(const CommandParameter& param)
{
    m_parameters.append(param);
}

void Command::addParameter(const QString& name, const CommandParameter::ValueType& value)
{
    m_parameters.append(CommandParameter(name, value));
}

void Command::addParameter(const CommandParameter::ValueType& value)
{
    m_parameters.append(CommandParameter(value));
}

QList<CommandParameter> Command::parameters() const
{
    return m_parameters;
}

void Command::clearParameters()
{
    m_parameters.clear();
}

QString Command::description() const
{
    return m_description;
}

void Command::setDescription(const QString& description)
{
    m_description = description;
}

QString Command::toString() const
{
    // Özel formatter varsa kullan
    if (s_formatter) {
        return s_formatter(*this);
    }

    // Varsayılan format
    QString result;

    if (m_index >= 0) {
        result = QString("%1;%2[%3]").arg(m_value).arg(m_command).arg(m_index);
    } else {
        result = QString("%1;%2").arg(m_value).arg(m_command);
    }

    return result;
}

void Command::setFormatter(std::function<QString(const Command&)> formatter)
{
    s_formatter = formatter;
}

// Factory metodları
std::shared_ptr<Command> Command::createVersionRequest()
{
    return std::make_shared<Command>(0, "VR", -1, CommandType::Query);
}

std::shared_ptr<Command> Command::createParameterCommand(int index, double value)
{
    return std::make_shared<Command>(value, "ZI", index, CommandType::Write);
}

std::shared_ptr<Command> Command::createPositionRequest()
{
    return std::make_shared<Command>(0, "PS", -1, CommandType::Query);
}

std::shared_ptr<Command> Command::createSpeedCommand(double speed)
{
    return std::make_shared<Command>(speed, "SR", -1, CommandType::Write);
}

std::shared_ptr<Command> Command::createAccelerationCommand(int accel)
{
    return std::make_shared<Command>(accel, "AC", -1, CommandType::Write);
}

std::shared_ptr<Command> Command::createDecelerationCommand(int decel)
{
    return std::make_shared<Command>(decel, "DC", -1, CommandType::Write);
}

std::shared_ptr<Command> Command::createStopCommand(bool emergency)
{
    return std::make_shared<Command>(0, emergency ? "AB" : "ST", -1, CommandType::Control);
}

std::shared_ptr<Command> Command::createMotionCommand(double position, double speed)
{
    auto cmd = std::make_shared<Command>(position, "PA", -1, CommandType::Motion);
    cmd->addParameter("speed", speed);
    return cmd;
}

std::shared_ptr<Command> Command::createHomeCommand()
{
    return std::make_shared<Command>(0, "HM", -1, CommandType::Control);
}
