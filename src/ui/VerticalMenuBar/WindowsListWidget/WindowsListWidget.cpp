#include "WindowsListWidget.h"
#include "ui_WindowsListWidget.h"
#include "EventAggregator.h"
#include "UIStateMachineController.h"

#include <SettingsManager.h>

using namespace Constants::VerticalMenuBarConstants;

WindowsListWidget::WindowsListWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WindowsListWidget)
{
    ui->setupUi(this);
    createItemToPageMap(ui);
    applyPageIndices(m_windowsListIndex);
    connect(ui->treewgtWindowsList,&QTreeWidget::currentItemChanged,this,&WindowsListWidget::currentItemChanged);
    selectTreeWidgetItemForPage(SettingsManager::getInstance().getLastOpenedSubMenu());
}

WindowsListWidget::~WindowsListWidget()
{
    delete ui;
}

void WindowsListWidget::setUIStateMachine(QSharedPointer<UIStateMachineController> stateMachine)
{
    m_StateMachine = stateMachine;
    connect(stateMachine.data(), &UIStateMachineController::driveSetupAndMotionStateEntered, this, [=, this] () {

    });
    connect(stateMachine.data(), &UIStateMachineController::driveSetupAndMotionStateExited, this, [=, this] () {
    });
}

void WindowsListWidget::selectTreeWidgetItemForPage(Constants::VerticalMenuBarConstants::WindowsListPages page)
{
    for (auto it = m_windowsListIndex.begin(); it != m_windowsListIndex.end(); ++it) {
        if (it.value() == page) {
            ui->treewgtWindowsList->setCurrentItem(it.key());
            ui->treewgtWindowsList->setFocus();
            return;
        }
    }
}

void WindowsListWidget::createItemToPageMap(Ui::WindowsListWidget *ui)
{
    m_windowsListIndex.insert(ui->treewgtWindowsList->topLevelItem(0), WindowsListPages::QUICK_TUNING);
    m_windowsListIndex.insert(ui->treewgtWindowsList->topLevelItem(1), WindowsListPages::EXPERT_TUNING);
    m_windowsListIndex.insert(ui->treewgtWindowsList->topLevelItem(2), WindowsListPages::MOTION_SINGLE_AXIS);
}

void WindowsListWidget::applyPageIndices(QHash<QTreeWidgetItem *, Constants::VerticalMenuBarConstants::WindowsListPages> &itemToPageMap)
{
    for (auto it = itemToPageMap.constBegin(); it != itemToPageMap.constEnd(); ++it) {
        QTreeWidgetItem* item = it.key();
        WindowsListPages pageIndex = it.value();
        if(item) {
            item->setData(0, Qt::UserRole, static_cast<int>(pageIndex));
        }
    }
}

void WindowsListWidget::on_treewgtWindowsList_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    switch (static_cast<WindowsListPages>(item->data(0, Qt::UserRole).toInt())) {
    case WindowsListPages::QUICK_TUNING:
        emit m_StateMachine->getContext()->navigateToQuickTuning();
        break;
    case WindowsListPages::EXPERT_TUNING:
        emit m_StateMachine->getContext()->navigateToExpertTuning();
        break;
    case WindowsListPages::MOTION_SINGLE_AXIS:
        emit m_StateMachine->getContext()->navigateToMotionSingleAxis();
        break;
    default:
        break;
    }
}

