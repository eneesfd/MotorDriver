#include "PreferencesController.h"
#include "LogManager.h"

PreferencesController::PreferencesController(std::shared_ptr<spdlog::logger> logger_,
                                            QObject *parent) :
    QObject{parent},
    m_logger{logger_}
{
    SpdlogInfo(m_logger, "PreferencesController constructor called");
    setupConnections();
}

void PreferencesController::setupConnections()
{
}


