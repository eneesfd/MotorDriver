#include "DisconnectControllerState.h"

DisconnectControllerState::DisconnectControllerState(std::shared_ptr<spdlog::logger> logger_, QState *parent)
    : QState(parent)
    , m_Logger{logger_}
{

}

void DisconnectControllerState::onEntry(QEvent *)
{

}

void DisconnectControllerState::onExit(QEvent *)
{

}
