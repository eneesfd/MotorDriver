#include "ExpertTuningState.h"
#include "LogManager.h"

ExpertTuningState::ExpertTuningState(std::shared_ptr<spdlog::logger> logger_, QState *parent) :
    QState(parent),
    m_Logger{logger_}
{

}

void ExpertTuningState::onEntry(QEvent *)
{
    SpdlogInfo(m_Logger, "ExpertTuningState::onEntry");
    emit expertTuningModeEntered();
}

void ExpertTuningState::onExit(QEvent *)
{
    SpdlogInfo(m_Logger, "ExpertTuningState::onExit");
    emit expertTuningModeExited();
}
