#ifndef UISTATEMACHINECONTEXT_H
#define UISTATEMACHINECONTEXT_H

#include <QObject>

class UIStateMachineContext : public QObject
{
    Q_OBJECT
public:
    explicit UIStateMachineContext(QObject *parent = nullptr);
signals:
    void connectionEstablished();
    void connectionLost();

    void navigateToSystemConfiguration();
    void navigateToDriveSetupAndMotion();

    void navigateToQuickTuning();
    void navigateToExpertTuning();
    void navigateToMotionSingleAxis();


    void disconnectedModeEntered();
    void disconnectStateExited();
    void connectedModeEntered();
    void connectedModeExited();
    void quickTuningModeEntered();
    void expertTuningModeEntered();
    void motionSingleAxisModeEntered();
    void quickTuningModeExited();
    void expertTuningModeExited();
    void motionSingleAxisModeExited();
    void systemConfigurationStateEntered();
    void systemConfigurationStateExited();
    void driveSetupAndMotionStateEntered();
    void driveSetupAndMotionStateExited();
    void splashScreenStateEntered();
    void splashScreenStateExited();
    void navigationRegionEntered();
    void navigationRegionExited();
};

#endif // UISTATEMACHINECONTEXT_H
