#ifndef BUSINESSSTATEMACHINECONTROLLER_H
#define BUSINESSSTATEMACHINECONTROLLER_H

#include <QObject>
#include <QStateMachine>
#include "BusinessStateMachineContext.h"
#include "ConnectedControllerState.h"
#include "DisconnectControllerState.h"
#include "DriveSetupAndMotion/DriveSetupAndMotionControllerState.h"
#include "DriveSetupAndMotion/QuickTuning/ApplicationSettings/BrakeControllerState.h"
#include "DriveSetupAndMotion/QuickTuning/AutomaticTuning/AutomaticTuningControllerState.h"
#include "DriveSetupAndMotion/QuickTuning/AxisConfiguration/AxisConfigurationsControllerState.h"
#include "DriveSetupAndMotion/QuickTuning/MotorAndFeedBack/FeedBackSettingsControllerState.h"
#include "DriveSetupAndMotion/QuickTuning/MotorAndFeedBack/MotorSettingsControllerState.h"
#include "LogManager.h"
#include "DriveSetupAndMotion/ExpertTuning/LimitsAndProtections/CurrentLimitsState.h"
#include "SystemConfigurationStates/SystemConfigurationControllerState.h"

class BusinessStateMachineController : public QObject
{
    Q_OBJECT
public:
    explicit BusinessStateMachineController(QObject *parent = nullptr);
    ~BusinessStateMachineController();
    void start();
    void stop();
    QSharedPointer<BusinessStateMachineContext> context() const;
signals:

private:
    QSharedPointer<QStateMachine> m_StateMachine;
    std::shared_ptr<LogManager> m_Logger;
    void setupStateMachine();


    QSharedPointer<DisconnectControllerState> m_DisconnectControllerState;
    QSharedPointer<ConnectedControllerState> m_ConnectedControllerState;
    QSharedPointer<DriveSetupAndMotionControllerState> m_DriveSetupAndMotionControllerState;
    QSharedPointer<QState> m_QuickTuningControllerState;
    QSharedPointer<QState> m_ExpertTuningControllerState;
    QSharedPointer<AxisConfigurationsControllerState> m_AxisConfigurationsControllerState;
    QSharedPointer<FeedBackSettingsControllerState> m_FeedBackSettingsControllerState;
    QSharedPointer<MotorSettingsControllerState> m_MotorSettingsControllerState;
    QSharedPointer<AutomaticTuningControllerState> m_AutomaticTuningControllerState;
    QSharedPointer<BrakeControllerState> m_BrakeControllerState;
    QSharedPointer<CurrentLimitsState> m_CurrentLimitsState;
    QSharedPointer<SystemConfigurationControllerState> m_SystemConfigurationControllerState;

    QSharedPointer<BusinessStateMachineContext> m_Context;

    void initialStateTransition();
    void setupConnections();
};

#endif // BUSINESSSTATEMACHINECONTROLLER_H
