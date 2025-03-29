#include "QuickTuningState.h"
#include "LogManager.h"

QuickTuningState::QuickTuningState(std::shared_ptr<spdlog::logger> logger_, QState *parent) :
    QState(parent),
    m_Logger{logger_}
{

}

void QuickTuningState::onEntry(QEvent *)
{
    SpdlogInfo(m_Logger, "QuickTuningState::onEntry");
    emit quickTuningModeEntered();
}

void QuickTuningState::onExit(QEvent *)
{
    SpdlogInfo(m_Logger, "QuickTuningState::onExit");
    emit quickTuningModeExited();
}
