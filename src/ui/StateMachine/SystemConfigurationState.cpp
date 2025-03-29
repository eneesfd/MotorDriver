#include "SystemConfigurationState.h"
#include "LogManager.h"

SystemConfigurationState::SystemConfigurationState(std::shared_ptr<spdlog::logger> logger_, QState *parent) :
    m_Logger{logger_},
    QState{parent}
{

}

void SystemConfigurationState::onEntry(QEvent *)
{
    SpdlogInfo(m_Logger, "SystemConfigurationState::onEntry");
    emit systemConfigurationStateEntered();
}

void SystemConfigurationState::onExit(QEvent *)
{
    SpdlogInfo(m_Logger, "SystemConfigurationState::onExit");
    emit systemConfigurationStateExited();
}
