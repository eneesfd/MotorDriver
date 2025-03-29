#include "FeedBackSettingsControllerState.h"
#include "LogManager.h"

FeedBackSettingsControllerState::FeedBackSettingsControllerState(std::shared_ptr<spdlog::logger> logger_, QState *parent) :
    QState{parent},
    m_Logger{logger_}
{

}

void FeedBackSettingsControllerState::onEntry(QEvent *)
{
    SpdlogInfo(m_Logger, "FeedBackSettingsControllerState entered");
}

void FeedBackSettingsControllerState::onExit(QEvent *)
{
    SpdlogInfo(m_Logger, "FeedBackSettingsControllerState exited");
}
