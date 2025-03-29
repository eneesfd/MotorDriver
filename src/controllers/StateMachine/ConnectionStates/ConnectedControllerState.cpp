#include "ConnectedControllerState.h"

ConnectedControllerState::ConnectedControllerState(std::shared_ptr<spdlog::logger> logger_, QState *parent)
    : QState(parent)
    , m_Logger{logger_}
{

}

void ConnectedControllerState::onEntry(QEvent *)
{

}

void ConnectedControllerState::onExit(QEvent *)
{

}
