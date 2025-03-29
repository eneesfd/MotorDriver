#include "SplashScreenState.h"
#include "LogManager.h"

SplashScreenState::SplashScreenState(std::shared_ptr<spdlog::logger> logger_, QState* parent) :
    QState{parent},
    m_Logger{logger_}
{

}

void SplashScreenState::onEntry(QEvent *)
{
    SpdlogInfo(m_Logger, "SplashScreenState onEntry");
    emit splashScreenStateEntered();
}

void SplashScreenState::onExit(QEvent *)
{
    SpdlogInfo(m_Logger, "SplashScreenState onExit");
    emit splashScreenStateExited();
}
