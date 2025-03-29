#include "MotorSettingsControllerState.h"
#include "LogManager.h"

MotorSettingsControllerState::MotorSettingsControllerState(std::shared_ptr<spdlog::logger> logger_, QState *parent) :
    QState{parent},
    m_Logger{logger_}
{

}

void MotorSettingsControllerState::onEntry(QEvent *)
{
    SpdlogInfo(m_Logger, "MotorSettingsControllerState entered");
}

void MotorSettingsControllerState::onExit(QEvent *)
{
    SpdlogInfo(m_Logger, "MotorSettingsControllerState exited");
}
