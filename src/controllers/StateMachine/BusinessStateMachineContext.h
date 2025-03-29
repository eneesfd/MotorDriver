#ifndef BUSINESSSTATEMACHINECONTEXT_H
#define BUSINESSSTATEMACHINECONTEXT_H

#include <QObject>

class BusinessStateMachineContext : public QObject
{
    Q_OBJECT
public:
    explicit BusinessStateMachineContext(QObject *parent = nullptr);
signals:
    void navigateToSystemConfiguration();
    void navigateToDriveSetupAndMotion();
    void navigateToQuickTuning();
    void navigateToExpertTuning();
    void navigateToMotionSingleAxis();
    void connectionEstablished();
    void connectionLost();

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

#endif // BUSINESSSTATEMACHINECONTEXT_H
