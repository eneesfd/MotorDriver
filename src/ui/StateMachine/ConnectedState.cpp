#include "ConnectedState.h"
#include "LogManager.h"

ConnectedState::ConnectedState(std::shared_ptr<spdlog::logger> logger_, QState* parent) :
    QState(parent),
    m_Logger{logger_}
{

}

void ConnectedState::onEntry(QEvent *)
{
    SpdlogInfo(m_Logger, "ConnectedState::onEntry");
    emit connectedModeEntered();
}

void ConnectedState::onExit(QEvent *)
{
    SpdlogInfo(m_Logger, "ConnectedState::onExit");
    emit connectedModeExited();
}
