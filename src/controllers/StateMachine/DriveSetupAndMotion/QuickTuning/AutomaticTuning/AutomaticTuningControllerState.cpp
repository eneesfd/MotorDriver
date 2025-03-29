#include "AutomaticTuningControllerState.h"
#include "LogManager.h"

AutomaticTuningControllerState::AutomaticTuningControllerState(std::shared_ptr<spdlog::logger> logger_, QState *parent) :
    QState{parent},
    m_Logger{logger_}
{

}

void AutomaticTuningControllerState::onEntry(QEvent *)
{
    SpdlogInfo(m_Logger, "AutomaticTuningControllerState entered");
}

void AutomaticTuningControllerState::onExit(QEvent *)
{
    SpdlogInfo(m_Logger, "AutomaticTuningControllerState exited");
}
