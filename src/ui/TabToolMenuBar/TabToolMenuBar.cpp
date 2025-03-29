#include "TabToolMenuBar.h"
#include "ui_TabToolMenuBar.h"
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QMessageBox>
#include <QListWidget>
#include <QLineEdit>
#include <QRadioButton>
#include "UIStateMachineController.h"

using namespace Constants::TabToolBarConstants;

TabToolMenuBar::TabToolMenuBar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TabToolMenuBar)
{
    ui->setupUi(this);
    ttb.reset(new tt::Builder(this));
    ttb->SetCustomWidgetCreator("textEdit", []() { return new QTextEdit(); });
    ttb->SetCustomWidgetCreator("label", []() { return new QLabel("Test"); });
    ttb->SetCustomWidgetCreator("checkBox", []() { return new QCheckBox("Test"); });
    ttb->SetCustomWidgetCreator("pushButton", []() { return new QPushButton(); });
    ttb->SetCustomWidgetCreator("comboBox", []() { return new QComboBox(); });
    ttb->SetCustomWidgetCreator("listWidget", []() {return new QListWidget(); });
    ttb->SetCustomWidgetCreator("lineEdit", []() { return new QLineEdit(); });
    ttb->SetCustomWidgetCreator("radioButton", [] () { return new QRadioButton(); });

    tabToolbar.reset(ttb->CreateTabToolbar(":/tt/tabtoolbar.json"));
    ui->verticalLayout->addWidget(tabToolbar.get());
    hideAllTTBarPages();
    connect(tabToolbar.data(), &tt::TabToolbar::SpecialTabClicked, this, &TabToolMenuBar::SpecialTabClicked);
}

TabToolMenuBar::~TabToolMenuBar()
{
    delete ui;
}

void TabToolMenuBar::setLogger(std::shared_ptr<spdlog::logger> logger)
{
    m_Logger = logger;
}

void TabToolMenuBar::setUIStateMachine(QSharedPointer<UIStateMachineController> stateMachine_)
{
    m_stateMachine = stateMachine_;
    connect(m_stateMachine.data(), &UIStateMachineController::disconnectedModeEntered, this, &TabToolMenuBar::onDisconnectStateTabToolBar);
    connect(m_stateMachine.data(), &UIStateMachineController::connectedModeEntered, this, &TabToolMenuBar::onConnectedStateTabToolBar);
    connect(m_stateMachine.data(), &UIStateMachineController::quickTuningModeEntered, this, &TabToolMenuBar::onQuickTuningModeEntered);
    connect(m_stateMachine.data(), &UIStateMachineController::expertTuningModeEntered, this, &TabToolMenuBar::onExpertTuningModeEntered);
    connect(m_stateMachine.data(), &UIStateMachineController::motionSingleAxisModeEntered, this, &TabToolMenuBar::onMotionSingleAxisModeEntered);
    connect(m_stateMachine.data(), &UIStateMachineController::motionSingleAxisModeExited, this, &TabToolMenuBar::onMotionSingleAxisModeExited);
    connect(m_stateMachine.data(), &UIStateMachineController::expertTuningModeExited, this, &TabToolMenuBar::onExpertTuningModeExited);
    connect(m_stateMachine.data(), &UIStateMachineController::quickTuningModeExited, this, &TabToolMenuBar::onQuickTuningModeExited);
    connect(m_stateMachine.data(), &UIStateMachineController::systemConfigurationStateEntered, this, &TabToolMenuBar::onSystemConfigurationStateEntered);
    connect(m_stateMachine.data(), &UIStateMachineController::systemConfigurationStateExited, this, &TabToolMenuBar::onSystemConfigurationStateExited);
}

void TabToolMenuBar::onDisconnectStateTabToolBar()
{
    disconnectStateParametersPage();
    disconnectStateSystemConfigurationPage();
    disconnectStateUploadAndDownloadPage();
    disconnectStateQuickTunnigPage();
    disconnectStateExpertTunnigPage();
}

void TabToolMenuBar::onConnectedStateTabToolBar()
{
    connectStateParametersPage();
    connectStateSystemConfigurationPage();
    connectStateUploadAndDownloadPage();
    connectStateQuickTunnigPage();
    connectStateExpertTunnigPage();
}

void TabToolMenuBar::onQuickTuningModeEntered()
{
    setVisibleTabToolBarPages(TTBAR_QUICK_TUNING_STATE, TabToolVisibility::Show);
}

void TabToolMenuBar::onQuickTuningModeExited()
{
    setVisibleTabToolBarPages(TTBAR_QUICK_TUNING_STATE, TabToolVisibility::Hide);
}

void TabToolMenuBar::onExpertTuningModeEntered()
{
    setVisibleTabToolBarPages(TTBAR_EXPERT_TUNING_STATE, TabToolVisibility::Show);
}

void TabToolMenuBar::onExpertTuningModeExited()
{
    setVisibleTabToolBarPages(TTBAR_EXPERT_TUNING_STATE, TabToolVisibility::Hide);
}

void TabToolMenuBar::onMotionSingleAxisModeEntered()
{
    setVisibleTabToolBarPages(TTBAR_MOTION_SINGLE_AXIS_STATE, TabToolVisibility::Show);
}

void TabToolMenuBar::onMotionSingleAxisModeExited()
{
    setVisibleTabToolBarPages(TTBAR_MOTION_SINGLE_AXIS_STATE, TabToolVisibility::Hide);
}

void TabToolMenuBar::onSystemConfigurationStateEntered()
{
    setVisibleTabToolBarPages(TTBAR_SYSTEM_CONFIGURATION_STATE, TabToolVisibility::Show);
}

void TabToolMenuBar::onSystemConfigurationStateExited()
{
    setVisibleTabToolBarPages(TTBAR_SYSTEM_CONFIGURATION_STATE,TabToolVisibility::Hide);
}


void TabToolMenuBar::setVisibleTabToolBarPages(const QStringList &pageList_, TabToolVisibility visibility_)
{
    try {
        for (const auto& pageName : pageList_) {
            tt::Page* page = (tt::Page*)(*ttb)[pageName];
            if (page == nullptr) {
                SpdlogError(m_Logger, "Page not found: {}");
                continue;
            }
            if (visibility_ == TabToolVisibility::Show)
                page->show();
            else if (visibility_ == TabToolVisibility::Hide)
                page->hide();
        }
    }
    catch (const std::exception& e)
    {
        SpdlogError(m_Logger, e.what());
    }
}


void TabToolMenuBar::disableTabToolMenuBarGroup(const QStringList &group)
{
    try
    {
        for (const auto& groupName : group)
        {
            tt::Group* newGroup = (tt::Group*)(*ttb)[groupName];
            if (newGroup == nullptr)
            {
                SpdlogError(m_Logger, "Group not found: {}");
                continue;
            }
            newGroup->setEnabled(false);
        }
    }
    catch (const std::exception& e)
    {
        SpdlogError(m_Logger, e.what());
    }
}

void TabToolMenuBar::enableTabToolMenuBarGroup(const QStringList &group)
{
    try
    {
        for (const auto& groupName : group)
        {
            tt::Group* newGroup = (tt::Group*)(*ttb)[groupName];
            if (newGroup == nullptr)
            {
                SpdlogError(m_Logger, "Group not found: {}");
                continue;
            }
            newGroup->setEnabled(true);
        }
    }
    catch (const std::exception& e)
    {
        SpdlogError(m_Logger, e.what());
    }
}

void TabToolMenuBar::hideAllTTBarPages()
{
    try {
        for (const auto& pageName : TTBAR_ALL_PAGES) {
            tt::Page* page = (tt::Page*)(*ttb)[pageName];
            if (page == nullptr) {
                SpdlogError(m_Logger, "Page not found: {}");
                continue;
            }
            page->hide();
        }
    }
    catch (const std::exception& e)
    {
        SpdlogError(m_Logger, e.what());
    }
}

void TabToolMenuBar::disconnectStateParametersPage()
{
    disableTabToolMenuBarGroup(PARAMETERS_GROUPS);
}

void TabToolMenuBar::disconnectStateSystemConfigurationPage()
{
    disableTabToolMenuBarGroup(SYSTEM_CONFIGURATION_GROUPS);
    ui->actionSave->setEnabled(false);
}

void TabToolMenuBar::disconnectStateUploadAndDownloadPage()
{
    disableTabToolMenuBarGroup(UPLOAD_AND_DOWNLOAD_GROUPS);
}

void TabToolMenuBar::disconnectStateQuickTunnigPage()
{
    disableTabToolMenuBarGroup(QUICK_TUNING_GROUPS);
}

void TabToolMenuBar::disconnectStateExpertTunnigPage()
{
    disableTabToolMenuBarGroup(EXPERT_TUNING_GROUPS);
}

void TabToolMenuBar::connectStateParametersPage()
{
    enableTabToolMenuBarGroup(PARAMETERS_GROUPS);
}

void TabToolMenuBar::connectStateSystemConfigurationPage()
{
    enableTabToolMenuBarGroup(SYSTEM_CONFIGURATION_GROUPS);
    ui->actionSave->setEnabled(true);
}

void TabToolMenuBar::connectStateQuickTunnigPage()
{
    enableTabToolMenuBarGroup(QUICK_TUNING_GROUPS);
}

void TabToolMenuBar::connectStateUploadAndDownloadPage()
{
    enableTabToolMenuBarGroup(UPLOAD_AND_DOWNLOAD_GROUPS);
}

void TabToolMenuBar::connectStateExpertTunnigPage()
{
    enableTabToolMenuBarGroup(EXPERT_TUNING_GROUPS);
}



