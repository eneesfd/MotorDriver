#include "TabToolMenuBarController.h"

TabToolMenuBarController::TabToolMenuBarController(std::shared_ptr<spdlog::logger> logger_,
                                                   QObject *parent)
    : QObject{parent},
      m_Logger{logger_}
{
    setupConnections();
}

void TabToolMenuBarController::setupConnections()
{
}
