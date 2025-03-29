#include "BrakeControllerState.h"
#include "LogManager.h"

BrakeControllerState::BrakeControllerState(std::shared_ptr<spdlog::logger> logger_, QState *parent) :
    QState{parent},
    m_Logger{logger_}
{

}

void BrakeControllerState::onEntry(QEvent *)
{
    SpdlogInfo(m_Logger, "BrakeControllerState entered");
}

void BrakeControllerState::onExit(QEvent *)
{
    SpdlogInfo(m_Logger, "BrakeControllerState exited");
}
