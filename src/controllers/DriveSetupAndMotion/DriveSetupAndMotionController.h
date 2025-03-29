#ifndef DRIVESETUPANDMOTIONCONTROLLER_H
#define DRIVESETUPANDMOTIONCONTROLLER_H

#include "BaseController/AbstractController.h"
#include <QObject>
#include <QStackedWidget>
#include "ExpertTuning/Current/CurrentIdentificationController.h"
#include "ExpertTuning/LimitsAndProtections/CurrentLimitsController.h"

class DriveSetupAndMotionController : public AbstractController
{
    Q_OBJECT
public:
    explicit DriveSetupAndMotionController(QSharedPointer<BusinessStateMachineContext> context, QSharedPointer<CommandDispatcher> commandDispatcher_,
                                           std::shared_ptr<spdlog::logger> gsLogger_, QObject *parent = nullptr);
    QSharedPointer<CurrentIdentificationController> currentIdentificationController() const;
    sp<CurrentLimitsController> currentLimitsController() const;
private:
    void InitilazeSubClasses();
    QSharedPointer<CurrentIdentificationController> m_currentIdentificationController;
    sp<CurrentLimitsController> m_currentLimitsController;
    void stateMachineConnects();
};

#endif // DRIVESETUPANDMOTIONCONTROLLER_H
