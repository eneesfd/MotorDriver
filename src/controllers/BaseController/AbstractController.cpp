#include "AbstractController.h"
#include "../core/CommandDispatcher/CommandDispatcher.h"

AbstractController::AbstractController(QObject *parent)
    : QObject(parent)
{
}

void AbstractController::setBusinessStateMachineContext(QSharedPointer<BusinessStateMachineContext> context)
{
    m_businessStateMachineContext = context;
}

QSharedPointer<BusinessStateMachineContext> AbstractController::businessStateMachineContext() const
{
    return m_businessStateMachineContext;
}

void AbstractController::setLogger(std::shared_ptr<spdlog::logger> logger)
{
    m_logger = logger;
}

std::shared_ptr<spdlog::logger> AbstractController::logger() const
{
    return m_logger;
}

void AbstractController::setCommandDispatcher(QSharedPointer<CommandDispatcher> dispatcher)
{
    m_commandDispatcher = dispatcher;
}

QSharedPointer<CommandDispatcher> AbstractController::commandDispatcher() const
{
    return m_commandDispatcher;
}

void AbstractController::setAll(QSharedPointer<BusinessStateMachineContext> context, std::shared_ptr<spdlog::logger> logger, QSharedPointer<CommandDispatcher> dispatcher)
{
    m_businessStateMachineContext = context;
    m_logger = logger;
    m_commandDispatcher = dispatcher;
}
