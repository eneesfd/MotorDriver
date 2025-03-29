#include "CommandDispatcher.h"
#include "../CommunicationManager/CommunicationManager.h"
#include "../CommunicationManager/Model/Command.h"
#include "../CommunicationManager/Model/Response.h"
#include "LogManager.h"

CommandDispatcher::CommandDispatcher(std::shared_ptr<spdlog::logger> logger, QObject* parent)
    : QObject(parent)
    , m_logger(logger)
    , m_communicationManager(nullptr)
{
}

void CommandDispatcher::setCommunicationManager(QSharedPointer<CommunicationManager> commManager)
{
    m_communicationManager = commManager;

    // CommManager sinyallerini bağla
    if (m_communicationManager) {
        connect(m_communicationManager.get(), &CommunicationManager::connectionStatusChanged,
                this, &CommandDispatcher::connectionStatusChanged);
        connect(m_communicationManager.get(), &CommunicationManager::errorOccurred,
                this, &CommandDispatcher::errorOccurred);
    }
}

std::optional<std::shared_ptr<Response>> CommandDispatcher::sendCommand(
    const QString& commandName, double value, int index, int timeoutMs)
{
    if (!m_communicationManager || !m_communicationManager->isConnected()) {
        SpdlogError(m_logger, "Not connected to serial port");
        return std::nullopt;
    }
    // Komut oluştur
    auto command = std::make_shared<Command>(value, commandName, index, CommandType::Query);
    // Komutu gönder
    SpdlogInfo(m_logger,
               fmt::format("Sending command: {}", command->toString().toStdString()));
    auto response = m_communicationManager->sendCommand(command, timeoutMs);

    // if (response.has_value()) {
    //     std::shared_ptr<Response> resp = response.value();

    //     // Yanıt tipine göre uygun loglama yap
    //     if (resp->type() == ResponseType::Text) {
    //         // Metin yanıtı için parametre olarak kaydedilen text değerini al
    //         auto textParam = resp->getParameterValue("text");
    //         if (textParam.has_value() && std::holds_alternative<QString>(textParam.value())) {
    //             QString textValue = std::get<QString>(textParam.value());
    //             SpdlogInfo(m_logger,
    //                        fmt::format("Text response: {}", textValue.toStdString()));
    //         } else {
    //             // Alternatif olarak ham yanıtı kullan
    //             SpdlogInfo(m_logger,
    //                        fmt::format("Raw response: {}", resp->rawResponse().toStdString()));
    //         }
    //     } else {
    //         // Sayısal yanıt için value() değerini kullan
    //         SpdlogInfo(m_logger,
    //                    fmt::format("Numeric response: {}", resp->value()));
    //     }
    // } else {
    //     SpdlogError(m_logger,
    //                 "No response received: " + m_communicationManager->lastError().toStdString());
    // }
    return response;
}

std::optional<std::shared_ptr<Response> > CommandDispatcher::sendCommand(std::shared_ptr<Command> command, int timeoutMs)
{
    if (!m_communicationManager || !m_communicationManager->isConnected()) {
        SpdlogError(m_logger, "Not connected to serial port");
        return std::nullopt;
    }

    SpdlogInfo(m_logger,
               fmt::format("Sending command: {}", command->toString().toStdString()));
    auto response = m_communicationManager->sendCommand(command, timeoutMs);

    if (response.has_value()) {
        std::shared_ptr<Response> resp = response.value();

        // Yanıt tipine göre uygun loglama yap
        if (resp->type() == ResponseType::Text) {
            // Metin yanıtı için parametre olarak kaydedilen text değerini al
            auto textParam = resp->getParameterValue("text");
            if (textParam.has_value() && std::holds_alternative<QString>(textParam.value())) {
                QString textValue = std::get<QString>(textParam.value());
                SpdlogInfo(m_logger,
                           fmt::format("Text response: {}", textValue.toStdString()));
            } else {
                // Alternatif olarak ham yanıtı kullan
                SpdlogInfo(m_logger,
                           fmt::format("Raw response: {}", resp->rawResponse().toStdString()));
            }
        } else {
            // Sayısal yanıt için value() değerini kullan
            SpdlogInfo(m_logger,
                       fmt::format("Numeric response: {}", resp->value()));
        }
    } else {
        SpdlogError(m_logger,
                    "No response received: " + m_communicationManager->lastError().toStdString());
    }
    return response;
}

bool CommandDispatcher::sendCommandAsync(
    std::shared_ptr<Command> command,
    std::function<void(std::shared_ptr<Response>)> callback,
    int timeoutMs)
{
    if (!m_communicationManager || !m_communicationManager->isConnected()) {
        SpdlogError(m_logger, "Not connected to serial port");
        return false;
    }

    SpdlogInfo(m_logger,
               fmt::format("Sending command async: {}", command->toString().toStdString()));

    return m_communicationManager->sendCommandAsync(command, callback, timeoutMs);
}

bool CommandDispatcher::sendCommandAsync(
    const QString& commandName,
    double value,
    int index,
    std::function<void(std::shared_ptr<Response>)> callback,
    int timeoutMs)
{
    if (!m_communicationManager || !m_communicationManager->isConnected()) {
        SpdlogError(m_logger, "Not connected to serial port");
        return false;
    }

    // Komut oluştur
    auto command = std::make_shared<Command>(value, commandName, index, CommandType::Query);

    // Asenkron gönder
    return sendCommandAsync(command, callback, timeoutMs);
}

bool CommandDispatcher::isConnected() const
{
    return m_communicationManager && m_communicationManager->isConnected();
}

QString CommandDispatcher::lastError() const
{
    return m_communicationManager ? m_communicationManager->lastError() : "No communication manager";
}
