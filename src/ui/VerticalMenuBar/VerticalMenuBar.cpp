#include "VerticalMenuBar.h"
#include "SettingsManager.h"
#include "ui_VerticalMenuBar.h"

#include <UIStateMachineController.h>



VerticalMenuBar::VerticalMenuBar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VerticalMenuBar)
{
    ui->setupUi(this);
    applyLastOpenedWindowConfiguration();
    setupConnections();
    // ui->wgtWindowsList->hide();
}

VerticalMenuBar::~VerticalMenuBar()
{
    delete ui;
}

void VerticalMenuBar::applyLastOpenedWindowConfiguration()
{
    switch (SettingsManager::getInstance().getLastOpenedWindow()) {
    case Constants::VerticalMenuBarConstants::MainMenuPages::SYSTEM_CONFIGURATION:
        // ui->wgtWindowsList->hide();
        break;
    case Constants::VerticalMenuBarConstants::MainMenuPages::DRIVE_SETUP_AND_MOTION:
        // ui->wgtWindowsList->show();
        break;
    case Constants::VerticalMenuBarConstants::MainMenuPages::DRIVE_PROGRAMMING:
        break;
    case Constants::VerticalMenuBarConstants::MainMenuPages::MAESTRO_SETUP_AND_MOTION:
        break;
    default:
        break;
    }
}

WindowsListWidget *VerticalMenuBar::getWindowsListWidget() const
{
    // return ui->wgtWindowsList;
}

MainMenuListWidget *VerticalMenuBar::getMainMenuListWidget() const
{
    return ui->wgtMainMenuList;
}

WorkspaceWidget* VerticalMenuBar::getWorkspaceWidget() const
{
    return ui->wgtWorkspace;
}

void VerticalMenuBar::setupConnections()
{
    connect(ui->wgtWorkspace, &WorkspaceWidget::workSpaceParentItemClicked, this, &VerticalMenuBar::workSpaceParentItemClicked);
    connect(ui->wgtWorkspace, &WorkspaceWidget::workSpaceChildItemClicked, this, &VerticalMenuBar::workSpaceChildItemClicked);
};

void VerticalMenuBar::setUIStateMachine(QSharedPointer<UIStateMachineController> stateMachine)
{
    connect(stateMachine.data(), &UIStateMachineController::driveSetupAndMotionStateEntered, this, [=, this] () {
        // ui->wgtWindowsList->show();
    });
    connect(stateMachine.data(), &UIStateMachineController::driveSetupAndMotionStateExited, this, [=, this] () {
        // ui->wgtWindowsList->hide();
    });

    ui->wgtMainMenuList->setUIStateMachine(stateMachine);
    // ui->wgtWindowsList->setUIStateMachine(stateMachine);
}
