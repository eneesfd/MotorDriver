#include "MotionSingleAxisState.h"
#include "LogManager.h"

MotionSingleAxisState::MotionSingleAxisState(std::shared_ptr<spdlog::logger> logger_, QState *parent) :
    QState(parent),
    m_Logger{logger_}
{

}

void MotionSingleAxisState::onEntry(QEvent *)
{
    SpdlogInfo(m_Logger, "MotionSingleAxisState::onEntry");
    emit motionSingleAxisModeEntered();
}

void MotionSingleAxisState::onExit(QEvent *)
{
    SpdlogInfo(m_Logger, "MotionSingleAxisState::onExit");
    emit motionSingleAxisModeExited();
}
