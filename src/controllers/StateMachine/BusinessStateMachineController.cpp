#include "BusinessStateMachineController.h"
#include "GuardedSignalTransition.h"
#include <iostream>

#define LOG_TAG "BusinessStateMachineController"

BusinessStateMachineController::BusinessStateMachineController(QObject *parent)
    : QObject{parent},
      m_StateMachine{new QStateMachine{this}}
{
    try {
        m_Logger = std::make_shared<LogManager>(LOG_TAG);
    } catch (std::exception &e) {
        std::cerr << "Exception caught in BusinessStateMachineController constructor: " << e.what() << std::endl;
    }

    m_Logger->addConsoleSink();
    m_Logger->setLogLevel(spdlog::level::trace);

    try {
        m_Context = QSharedPointer<BusinessStateMachineContext>::create();
    } catch (std::exception &e) {
        std::cerr << "Exception caught in BusinessStateMachineController constructor: " << e.what() << std::endl;
    }

    setupStateMachine();
    initialStateTransition();
}

BusinessStateMachineController::~BusinessStateMachineController()
{

}

void BusinessStateMachineController::start()
{
    m_StateMachine->start();
}

void BusinessStateMachineController::stop()
{
    m_StateMachine->stop();
}

QSharedPointer<BusinessStateMachineContext> BusinessStateMachineController::context() const
{
    return m_Context;
}

void BusinessStateMachineController::setupStateMachine()
{
    std::shared_ptr<spdlog::logger> logger(m_Logger->start());

    m_DisconnectControllerState.reset(new DisconnectControllerState(logger, m_StateMachine.data()));
    m_ConnectedControllerState.reset(new ConnectedControllerState(logger, m_StateMachine.data()));

    m_SystemConfigurationControllerState.reset(new SystemConfigurationControllerState(logger, m_StateMachine.data()));

    m_DriveSetupAndMotionControllerState.reset(new DriveSetupAndMotionControllerState(logger, m_StateMachine.data()));

    m_QuickTuningControllerState.reset(new QState(m_DriveSetupAndMotionControllerState.data()));
    m_ExpertTuningControllerState.reset(new QState(m_DriveSetupAndMotionControllerState.data()));


    m_AxisConfigurationsControllerState.reset(new AxisConfigurationsControllerState(logger, m_QuickTuningControllerState.data()));
    m_FeedBackSettingsControllerState.reset(new FeedBackSettingsControllerState(logger, m_QuickTuningControllerState.data()));
    m_MotorSettingsControllerState.reset(new MotorSettingsControllerState(logger, m_QuickTuningControllerState.data()));
    m_AutomaticTuningControllerState.reset(new AutomaticTuningControllerState(logger, m_QuickTuningControllerState.data()));
    m_BrakeControllerState.reset(new BrakeControllerState(logger, m_QuickTuningControllerState.data()));

    m_CurrentLimitsState.reset(new CurrentLimitsState(logger, m_ExpertTuningControllerState.data()));

    //TODO: QSettings parametrelerine göre initial state ayarlanmalı.
    m_DriveSetupAndMotionControllerState->setInitialState(m_QuickTuningControllerState.data());

    m_QuickTuningControllerState->setInitialState(m_AxisConfigurationsControllerState.data());

    m_StateMachine->setInitialState(m_DriveSetupAndMotionControllerState.data());
}

void BusinessStateMachineController::initialStateTransition()
{
    //TODO: Diğer transtation metotları eklenmeli. Örnek: Quick tuning, expert tuning, motion single axis.
    auto systemConfigurationTransition = new GuardedSignalTransition(
        m_Context.data(),
        &BusinessStateMachineContext::navigateToSystemConfiguration,
        m_SystemConfigurationControllerState.data());
    systemConfigurationTransition->setTransitionType(QAbstractTransition::InternalTransition);
    m_DriveSetupAndMotionControllerState->addTransition(systemConfigurationTransition);

    auto driveSetupAndMotionTransition = new GuardedSignalTransition(
        m_Context.data(),
        &BusinessStateMachineContext::navigateToDriveSetupAndMotion,
        m_DriveSetupAndMotionControllerState.data());
    driveSetupAndMotionTransition->setTransitionType(QAbstractTransition::InternalTransition);
    m_SystemConfigurationControllerState->addTransition(driveSetupAndMotionTransition);
}
