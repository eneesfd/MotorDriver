#ifndef UISTATEMACHINECONTROLLER_H
#define UISTATEMACHINECONTROLLER_H

#include <QEventTransition>
#include <QObject>
#include <QSignalTransition>
#include <QStateMachine>
#include "ConnectedState.h"
#include "DriveSetupandMotionState.h"
#include "ExpertTuningState.h"
#include "MotionSingleAxisState.h"
#include "QuickTuningState.h"
#include "SplashScreenState.h"
#include "DisconnectState.h"
#include "LogManager.h"
#include "SystemConfigurationState.h"
#include "UIStateMachineContext.h"

class UIStateMachineController : public QObject
{
    Q_OBJECT
public:
    explicit UIStateMachineController(QObject *parent = nullptr);
    ~UIStateMachineController();
    QSharedPointer<UIStateMachineContext> getContext() const;
    void start();
    void stop();
signals:
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
private:
    QSharedPointer<QStateMachine> m_StateMachine;
    QSharedPointer<QState> m_CompositeState;
    QSharedPointer<QState> m_ConnectionRegion;

    std::shared_ptr<LogManager> m_Logger;
    QSharedPointer<QState> m_NavigationRegion;
    QSharedPointer<DriveSetupandMotionState> m_DriveSetupAndMotionState;
    QSharedPointer<QuickTuningState> m_QuickTuningState;
    QSharedPointer<ExpertTuningState> m_ExpertTuningState;
    QSharedPointer<MotionSingleAxisState> m_MotionSingleAxisState;
    QSharedPointer<ConnectedState> m_ConnectedState;
    QSharedPointer<SplashScreenState> m_SplashScreenState;
    QSharedPointer<DisconnectState> m_DisconnectState;
    QSharedPointer<SystemConfigurationState> m_SystemConfigurationState;
    void setupStateMachine();
    void initialStateTransition();
    void setupConnections();
    QSharedPointer<UIStateMachineContext> m_Context;
signals:
};

#endif // UISTATEMACHINECONTROLLER_H
