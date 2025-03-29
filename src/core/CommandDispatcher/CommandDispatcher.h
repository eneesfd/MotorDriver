#ifndef COMMANDDISPATCHER_H
#define COMMANDDISPATCHER_H

#include <QObject>
#include <optional>
#include <QSharedPointer>

class Command;
class Response;
class CommunicationManager;
namespace spdlog { class logger; }

class CommandDispatcher : public QObject
{
    Q_OBJECT
public:
    explicit CommandDispatcher(std::shared_ptr<spdlog::logger> logger, QObject* parent = nullptr);
    ~CommandDispatcher() = default;

    void setCommunicationManager(QSharedPointer<CommunicationManager> commManager);

    std::optional<std::shared_ptr<Response>> sendCommand(const QString& commandName, double value, int index = -1, int timeoutMs = 1000);
    std::optional<std::shared_ptr<Response>> sendCommand(std::shared_ptr<Command> command, int timeoutMs = 1000);
    bool sendCommandAsync(std::shared_ptr<Command> command, std::function<void (std::shared_ptr<Response>)> callback, int timeoutMs);
    bool sendCommandAsync(const QString &commandName, double value, int index, std::function<void (std::shared_ptr<Response>)> callback, int timeoutMs);

    bool isConnected() const;
    QString lastError() const;

    QStringList getAvailablePorts();
signals:
    void commandSent(const QString& commandName, double value, int index);
    void responseReceived(const QString& commandName, bool success, const QVariant& response);
    void connectionStatusChanged(bool connected);
    void errorOccurred(const QString& error);
private:
    std::shared_ptr<spdlog::logger> m_logger;
    QSharedPointer<CommunicationManager> m_communicationManager;
};

#endif // COMMANDDISPATCHER_H
