#include "UIStateMachineController.h"
#include "GuardedSignalTransition.h"
#include <QEventTransition>
#include <QHistoryState>
#include <QSignalTransition>
#include <SettingsManager.h>
#include <iostream>

#define LOG_TAG "UIStateMachineController"

using namespace Constants::UIStateMachineConstants;

UIStateMachineController::UIStateMachineController(QObject *parent)
    : m_StateMachine{new QStateMachine{this}},
    QObject{parent}
{
    try {
        m_Logger = std::make_shared<LogManager>(LOG_TAG);
    }
    catch(std::exception& e) {
        std::cerr << "Exception caught in UIStateMachineController constructor: " << e.what() << std::endl;
    }

    m_Logger->addConsoleSink();
    m_Logger->setLogLevel(spdlog::level::trace);

    try {
        m_Context.reset(new UIStateMachineContext());
    }
    catch(std::exception& e) {
        std::cerr << "Exception caught in UIStateMachineController constructor: " << e.what() << std::endl;
    }

    setupStateMachine();
    setupConnections();
    initialStateTransition();
}

UIStateMachineController::~UIStateMachineController()
{

}

QSharedPointer<UIStateMachineContext> UIStateMachineController::getContext() const
{
    return m_Context;
}

void UIStateMachineController::start()
{
    m_StateMachine->start();
}

void UIStateMachineController::stop()
{
    m_StateMachine->stop();
}

void UIStateMachineController::setupStateMachine()
{
    std::shared_ptr<spdlog::logger> logger(m_Logger->start());

    m_CompositeState.reset(new QState());
    m_CompositeState->setChildMode(QState::ParallelStates);

    m_ConnectionRegion.reset(new QState(m_CompositeState.data()));

    m_ConnectedState.reset(new ConnectedState{logger, m_ConnectionRegion.data()});
    m_DisconnectState.reset(new DisconnectState{logger, m_ConnectionRegion.data()});

    m_ConnectionRegion->setInitialState(m_DisconnectState.data());


    m_NavigationRegion.reset(new QState(m_CompositeState.data()));

    m_SplashScreenState.reset(new SplashScreenState{logger, m_NavigationRegion.data()});
    m_SystemConfigurationState.reset(new SystemConfigurationState{logger, m_NavigationRegion.data()});
    m_DriveSetupAndMotionState.reset(new DriveSetupandMotionState{logger, m_NavigationRegion.data()});

    m_QuickTuningState.reset(new QuickTuningState{logger, m_DriveSetupAndMotionState.data()});
    m_ExpertTuningState.reset(new ExpertTuningState{logger, m_DriveSetupAndMotionState.data()});
    m_MotionSingleAxisState.reset(new MotionSingleAxisState{logger, m_DriveSetupAndMotionState.data()});

    auto history = new QHistoryState(QHistoryState::ShallowHistory, m_DriveSetupAndMotionState.data());
    switch (SettingsManager::getInstance().getLastOpenedSubMenu()) {
    case Constants::VerticalMenuBarConstants::WindowsListPages::QUICK_TUNING:
        history->setDefaultState(m_QuickTuningState.data());
        break;
    case Constants::VerticalMenuBarConstants::WindowsListPages::EXPERT_TUNING:
        history->setDefaultState(m_ExpertTuningState.data());
        break;
    case Constants::VerticalMenuBarConstants::WindowsListPages::MOTION_SINGLE_AXIS:
        history->setDefaultState(m_MotionSingleAxisState.data());
        break;
    default:
        history->setDefaultState(m_QuickTuningState.data());
        break;
    }
    m_DriveSetupAndMotionState->setInitialState(std::move(history));

    switch (SettingsManager::getInstance().getLastOpenedWindow()) {
    case Constants::VerticalMenuBarConstants::MainMenuPages::SYSTEM_CONFIGURATION:
        m_NavigationRegion->setInitialState(m_SystemConfigurationState.data());
        break;
    case Constants::VerticalMenuBarConstants::MainMenuPages::DRIVE_SETUP_AND_MOTION:
        m_NavigationRegion->setInitialState(m_DriveSetupAndMotionState.data());
        break;
    default:
        m_NavigationRegion->setInitialState(m_SystemConfigurationState.data());
        break;
    }

    m_StateMachine->addState(m_CompositeState.data());
    m_StateMachine->setInitialState(m_CompositeState.data());
}

void UIStateMachineController::initialStateTransition()
{
    auto quickTuningTransition = new GuardedSignalTransition(
        m_Context.data(),
        &UIStateMachineContext::navigateToQuickTuning,
        m_QuickTuningState.data(),
        m_StateMachine.data());
    quickTuningTransition->setTransitionType(QAbstractTransition::InternalTransition);
    quickTuningTransition->setTargetState(m_QuickTuningState.data());
    m_DriveSetupAndMotionState->addTransition(quickTuningTransition);

    auto expertTuningTransition = new GuardedSignalTransition(
        m_Context.data(),
        &UIStateMachineContext::navigateToExpertTuning,
        m_ExpertTuningState.data(),
        m_StateMachine.data());
    expertTuningTransition->setTransitionType(QAbstractTransition::InternalTransition);
    expertTuningTransition->setTargetState(m_ExpertTuningState.data());
    m_DriveSetupAndMotionState->addTransition(expertTuningTransition);

    auto moxionSingleAxisTransition = new GuardedSignalTransition(
        m_Context.data(),
        &UIStateMachineContext::navigateToMotionSingleAxis,
        m_MotionSingleAxisState.data(),
        m_StateMachine.data());
    moxionSingleAxisTransition->setTransitionType(QAbstractTransition::InternalTransition);
    moxionSingleAxisTransition->setTargetState(m_MotionSingleAxisState.data());
    m_DriveSetupAndMotionState->addTransition(moxionSingleAxisTransition);

    auto systemConfigurationTransition = new GuardedSignalTransition(
        m_Context.data(),
        &UIStateMachineContext::navigateToSystemConfiguration,
        m_SystemConfigurationState.data(),
        m_StateMachine.data());
    systemConfigurationTransition->setTransitionType(QAbstractTransition::InternalTransition);
    systemConfigurationTransition->setTargetState(m_SystemConfigurationState.data());
    m_NavigationRegion->addTransition(systemConfigurationTransition);

    auto driveSetupAndMotionTransition = new GuardedSignalTransition(
        m_Context.data(),
        &UIStateMachineContext::navigateToDriveSetupAndMotion,
        m_DriveSetupAndMotionState.data(),
        m_StateMachine.data());
    driveSetupAndMotionTransition->setTransitionType(QAbstractTransition::InternalTransition);
    driveSetupAndMotionTransition->setTargetState(m_DriveSetupAndMotionState.data());
    m_NavigationRegion->addTransition(driveSetupAndMotionTransition);
}

void UIStateMachineController::setupConnections()
{
    connect(m_DisconnectState.data(), &DisconnectState::disconnectStateEntered,this,
            &UIStateMachineController::disconnectedModeEntered);
    connect(m_DisconnectState.data(), &DisconnectState::disconnectStateExited,this,
            &UIStateMachineController::disconnectStateExited);

    connect(m_ConnectedState.data(), &ConnectedState::connectedModeEntered, this,
            &UIStateMachineController::connectedModeEntered);
    connect(m_ConnectedState.data(), &ConnectedState::connectedModeExited, this,
            &UIStateMachineController::connectedModeExited);

    connect(m_QuickTuningState.data(), &QuickTuningState::quickTuningModeEntered, this,
            &UIStateMachineController::quickTuningModeEntered);
    connect(m_QuickTuningState.data(), &QuickTuningState::quickTuningModeExited, this,
            &UIStateMachineController::quickTuningModeExited);

    connect(m_ExpertTuningState.data(), &ExpertTuningState::expertTuningModeEntered, this,
            &UIStateMachineController::expertTuningModeEntered);
    connect(m_ExpertTuningState.data(), &ExpertTuningState::expertTuningModeExited, this,
            &UIStateMachineController::expertTuningModeExited);

    connect(m_MotionSingleAxisState.data(), &MotionSingleAxisState::motionSingleAxisModeEntered, this,
            &UIStateMachineController::motionSingleAxisModeEntered);
    connect(m_MotionSingleAxisState.data(), &MotionSingleAxisState::motionSingleAxisModeExited, this,
            &UIStateMachineController::motionSingleAxisModeExited);

    connect(m_SystemConfigurationState.data(), &SystemConfigurationState::systemConfigurationStateEntered, this,
            &UIStateMachineController::systemConfigurationStateEntered);
    connect(m_SystemConfigurationState.data(), &SystemConfigurationState::systemConfigurationStateExited, this,
            &UIStateMachineController::systemConfigurationStateExited);

    connect(m_DriveSetupAndMotionState.data(), &DriveSetupandMotionState::driveSetupAndMotionStateEntered, this,
            &UIStateMachineController::driveSetupAndMotionStateEntered);
    connect(m_DriveSetupAndMotionState.data(), &DriveSetupandMotionState::driveSetupAndMotionStateExited, this,
            &UIStateMachineController::driveSetupAndMotionStateExited);

    connect(m_SplashScreenState.data(), &SplashScreenState::splashScreenStateEntered, this,
            &UIStateMachineController::splashScreenStateEntered);
    connect(m_SplashScreenState.data(), &SplashScreenState::splashScreenStateExited, this,
            &UIStateMachineController::splashScreenStateExited);

    //************************************************************************************************************

    connect(m_DisconnectState.data(), &DisconnectState::disconnectStateEntered,m_Context.get(),
            &UIStateMachineContext::disconnectedModeEntered);
    connect(m_DisconnectState.data(), &DisconnectState::disconnectStateExited,m_Context.get(),
            &UIStateMachineContext::disconnectStateExited);

    connect(m_ConnectedState.data(), &ConnectedState::connectedModeEntered, m_Context.get(),
            &UIStateMachineContext::connectedModeEntered);
    connect(m_ConnectedState.data(), &ConnectedState::connectedModeExited, m_Context.get(),
            &UIStateMachineContext::connectedModeExited);

    connect(m_QuickTuningState.data(), &QuickTuningState::quickTuningModeEntered, m_Context.get(),
            &UIStateMachineContext::quickTuningModeEntered);
    connect(m_QuickTuningState.data(), &QuickTuningState::quickTuningModeExited, m_Context.get(),
            &UIStateMachineContext::quickTuningModeExited);

    connect(m_ExpertTuningState.data(), &ExpertTuningState::expertTuningModeEntered, m_Context.get(),
            &UIStateMachineContext::expertTuningModeEntered);
    connect(m_ExpertTuningState.data(), &ExpertTuningState::expertTuningModeExited, m_Context.get(),
            &UIStateMachineContext::expertTuningModeExited);

    connect(m_MotionSingleAxisState.data(), &MotionSingleAxisState::motionSingleAxisModeEntered, m_Context.get(),
            &UIStateMachineContext::motionSingleAxisModeEntered);
    connect(m_MotionSingleAxisState.data(), &MotionSingleAxisState::motionSingleAxisModeExited, m_Context.get(),
            &UIStateMachineContext::motionSingleAxisModeExited);

    connect(m_SystemConfigurationState.data(), &SystemConfigurationState::systemConfigurationStateEntered, m_Context.get(),
            &UIStateMachineContext::systemConfigurationStateEntered);
    connect(m_SystemConfigurationState.data(), &SystemConfigurationState::systemConfigurationStateExited, m_Context.get(),
            &UIStateMachineContext::systemConfigurationStateExited);

    connect(m_DriveSetupAndMotionState.data(), &DriveSetupandMotionState::driveSetupAndMotionStateEntered, m_Context.get(),
            &UIStateMachineContext::driveSetupAndMotionStateEntered);
    connect(m_DriveSetupAndMotionState.data(), &DriveSetupandMotionState::driveSetupAndMotionStateExited, m_Context.get(),
            &UIStateMachineContext::driveSetupAndMotionStateExited);

    connect(m_SplashScreenState.data(), &SplashScreenState::splashScreenStateEntered, m_Context.get(),
            &UIStateMachineContext::splashScreenStateEntered);
    connect(m_SplashScreenState.data(), &SplashScreenState::splashScreenStateExited, m_Context.get(),
            &UIStateMachineContext::splashScreenStateExited);
}


