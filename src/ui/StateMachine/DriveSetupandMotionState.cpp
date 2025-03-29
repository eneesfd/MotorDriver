#include "DriveSetupandMotionState.h"
#include "LogManager.h"

DriveSetupandMotionState::DriveSetupandMotionState(std::shared_ptr<spdlog::logger> logger_, QState* parent) :
    QState(parent),
    m_Logger{logger_}
{
}

void DriveSetupandMotionState::onEntry(QEvent *)
{
    SpdlogInfo(m_Logger, "Drive Setup and Motion State Entered");
    emit driveSetupAndMotionStateEntered();
}

void DriveSetupandMotionState::onExit(QEvent *)
{
    SpdlogInfo(m_Logger, "Drive Setup and Motion State Exited");
    emit driveSetupAndMotionStateExited();
}
