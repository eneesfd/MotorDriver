#include "DriveSetupAndMotionController.h"
#include "LogManager.h"
#include "BusinessStateMachineContext.h"

DriveSetupAndMotionController::DriveSetupAndMotionController(QSharedPointer<BusinessStateMachineContext> context, QSharedPointer<CommandDispatcher> commandDispatcher_,
                                                             std::shared_ptr<spdlog::logger> logger_, QObject *parent)
    : AbstractController(parent)
{
    setAll(context,logger_,commandDispatcher_);

    SpdlogInfo(logger(), "DriveSetupAndMotionController constructor called");
    InitilazeSubClasses();
    stateMachineConnects();
}

QSharedPointer<CurrentIdentificationController> DriveSetupAndMotionController::currentIdentificationController() const
{
    return m_currentIdentificationController;
}

sp<CurrentLimitsController> DriveSetupAndMotionController::currentLimitsController() const
{
    return m_currentLimitsController;
}

void DriveSetupAndMotionController::InitilazeSubClasses()
{
    m_currentIdentificationController = QSharedPointer<CurrentIdentificationController>::create(commandDispatcher(), logger());
    m_currentLimitsController = sp<CurrentLimitsController>::create(businessStateMachineContext(), commandDispatcher(), logger());
}

void DriveSetupAndMotionController::stateMachineConnects()
{
    connect(businessStateMachineContext().get(), &BusinessStateMachineContext::driveSetupAndMotionStateEntered, this, [=, this] () {
        m_currentLimitsController->queryParameters();
    });
}
