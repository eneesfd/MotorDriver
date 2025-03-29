#include "QuickTuningController.h"

QuickTuningController::QuickTuningController(std::shared_ptr<spdlog::logger> logger_, QObject *parent) :
    QObject{parent},
    m_logger{logger_}
{

}
