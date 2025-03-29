#include "CurrentLimitsState.h"
#include "LogManager.h"

CurrentLimitsState::CurrentLimitsState(std::shared_ptr<spdlog::logger> logger_, QState *parent)
    : QState{parent}
    , m_logger{logger_}
{

}

void CurrentLimitsState::onEntry(QEvent *)
{
    SpdlogInfo(m_logger, "CurrentLimitsState entered");
    emit currentLimitsStateEntered();
}

void CurrentLimitsState::onExit(QEvent *)
{
    SpdlogInfo(m_logger, "CurrentLimitsState exited");
    emit currentLimitsStateExited();
}
