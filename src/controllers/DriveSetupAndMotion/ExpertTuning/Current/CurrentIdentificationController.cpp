#include "CurrentIdentificationController.h"
#include "../core/CommandDispatcher/CommandDispatcher.h"

CurrentIdentificationController::CurrentIdentificationController(QSharedPointer<CommandDispatcher> commandDispatcher_,
                                                                 std::shared_ptr<spdlog::logger> logger_, QObject *parent)
    : AbstractController(parent)
{
    setLogger(logger_);
    setCommandDispatcher(commandDispatcher_);
}

void CurrentIdentificationController::identify()
{
    auto response = commandDispatcher()->sendCommand("PA",10,2,10000);
}
