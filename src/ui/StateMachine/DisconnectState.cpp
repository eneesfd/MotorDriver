#include "DisconnectState.h"
#include <LogManager.h>

DisconnectState::DisconnectState(std::shared_ptr<spdlog::logger> logger_, QState* parent) :
    QState{parent},
    m_Logger{logger_}
{
}

void DisconnectState::onEntry(QEvent *)
{
    SpdlogInfo(m_Logger, "Disconnect State onEntry.");
    emit disconnectStateEntered();
}

void DisconnectState::onExit(QEvent *)
{
    SpdlogInfo(m_Logger, "Discoonect State onExit.");
    emit disconnectStateExited();
}
