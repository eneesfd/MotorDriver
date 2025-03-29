#include "AxisConfigurationsControllerState.h"
#include "LogManager.h"

AxisConfigurationsControllerState::AxisConfigurationsControllerState(std::shared_ptr<spdlog::logger> logger_, QState *parent) :
    QState{parent},
    m_Logger{logger_}
{

}

void AxisConfigurationsControllerState::onEntry(QEvent *)
{
    SpdlogInfo(m_Logger, "AxisConfigurationsControllerState entered");
}

void AxisConfigurationsControllerState::onExit(QEvent *)
{
    SpdlogInfo(m_Logger, "AxisConfigurationsControllerState exited");
}
