#include "DriveSetupAndMotionControllerState.h"
#include "LogManager.h"

DriveSetupAndMotionControllerState::DriveSetupAndMotionControllerState(std::shared_ptr<spdlog::logger> logger, QState *parent) :
    QState{parent},
    m_Logger{logger}
{

}

void DriveSetupAndMotionControllerState::onEntry(QEvent *)
{
    SpdlogInfo(m_Logger, "DriveSetupAndMotionControllerState entered");
}

void DriveSetupAndMotionControllerState::onExit(QEvent *)
{
    SpdlogInfo(m_Logger, "DriveSetupAndMotionControllerState exited");
}
