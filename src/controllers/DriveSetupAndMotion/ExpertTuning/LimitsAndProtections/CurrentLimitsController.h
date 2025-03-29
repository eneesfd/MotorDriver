#ifndef CURRENTLIMITSCONTROLLER_H
#define CURRENTLIMITSCONTROLLER_H

#include "BaseController/AbstractController.h"
#include "../core/CommandDispatcher/SequentialCommandProcessor.h"

class CurrentLimitsController : public AbstractController
{
    Q_OBJECT
public:
    explicit CurrentLimitsController(sp<BusinessStateMachineContext> context, sp<CommandDispatcher> commandDispatcher_,
                                     std::shared_ptr<spdlog::logger> logger_, QObject* parent = nullptr);
    void queryParameters();
signals:
    void changedPeakCurrentValue(QString value);
    void changedPeakCurrentDuration(QString value);
    void changedPeakContinuousCurrent(QString value);
    void changedPeakPWMOutputDutyCycle(QString value);
    void changedPeakMaxVoltageBV(QString value);
    void changedPeakIntegralLimit(QString value);
private:
    void setupConnections();
    QScopedPointer<SequentialCommandProcessor> m_sequentialProcessor;
};

#endif // CURRENTLIMITSCONTROLLER_H
