#include "SystemConfigurationControllerState.h"
#include "LogManager.h"

SystemConfigurationControllerState::SystemConfigurationControllerState(std::shared_ptr<spdlog::logger> logger, QState *parent)
    : QState{parent},
      m_Logger{logger}
{
}

void SystemConfigurationControllerState::onEntry(QEvent *)
{
    SpdlogInfo(m_Logger, "SystemConfigurationControllerState entered");
}

void SystemConfigurationControllerState::onExit(QEvent *)
{
    SpdlogInfo(m_Logger, "SystemConfigurationControllerState exited");
}
