// AbstractController.h
#ifndef ABSTRACTCONTROLLER_H
#define ABSTRACTCONTROLLER_H

#include <QObject>
#include <memory>
#include <QSharedPointer>

namespace spdlog { class logger; }
class CommandDispatcher;
class BusinessStateMachineContext;

template<typename T>
using sp = QSharedPointer<T>;

class AbstractController : public QObject {
    Q_OBJECT
public:
    explicit AbstractController(QObject* parent = nullptr);
    virtual ~AbstractController() = default;

    void setBusinessStateMachineContext(QSharedPointer<BusinessStateMachineContext> context);
    QSharedPointer<BusinessStateMachineContext> businessStateMachineContext() const;

    void setLogger(std::shared_ptr<spdlog::logger> logger);
    std::shared_ptr<spdlog::logger> logger() const;

    void setCommandDispatcher(QSharedPointer<CommandDispatcher> dispatcher);
    QSharedPointer<CommandDispatcher> commandDispatcher() const;

    void setAll(QSharedPointer<BusinessStateMachineContext> context, std::shared_ptr<spdlog::logger> logger, QSharedPointer<CommandDispatcher> dispatcher);

private:
    std::shared_ptr<spdlog::logger> m_logger;
    QSharedPointer<CommandDispatcher> m_commandDispatcher;
    QSharedPointer<BusinessStateMachineContext> m_businessStateMachineContext;
};

#endif // ABSTRACTCONTROLLER_H
