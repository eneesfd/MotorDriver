#include "MainMenuListWidget.h"
#include "ui_MainMenuListWidget.h"
#include "UIStateMachineController.h"

#include <SettingsManager.h>

using namespace Constants::VerticalMenuBarConstants;

MainMenuListWidget::MainMenuListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainMenuListWidget)
{
    ui->setupUi(this);
    ui->treewgtMainMenuList->setItemDelegate(new MainMenuListWidgetItemDelegate(ui->treewgtMainMenuList));

    createItemToPageMap(ui);
    applyPageIndices(m_mainMenuIndex);
    setupConnections();
    selectTreeWidgetItemForPage(SettingsManager::getInstance().getLastOpenedWindow());
}

MainMenuListWidget::~MainMenuListWidget()
{
    delete ui;
}

void MainMenuListWidget::setUIStateMachine(QSharedPointer<UIStateMachineController> stateMachine)
{
    // TODO: Buraya context get edilmeli, state machine kendisi getirilmiş.
    m_StateMachine = stateMachine;
    connect(stateMachine.data(), &UIStateMachineController::navigationRegionEntered, this, [=, this] () {
        selectTreeWidgetItemForPage(SettingsManager::getInstance().getLastOpenedWindow());
    });
}

void MainMenuListWidget::selectTreeWidgetItemForPage(Constants::VerticalMenuBarConstants::MainMenuPages page)
{
    for (auto it = m_mainMenuIndex.begin(); it != m_mainMenuIndex.end(); ++it) {
        if (it.value() == page) {
            ui->treewgtMainMenuList->setCurrentItem(it.key());
            ui->treewgtMainMenuList->setFocus();
            return;
        }
    }
}

void MainMenuListWidget::setupConnections()
{
    connect(ui->treewgtMainMenuList, &QTreeWidget::currentItemChanged,this,&MainMenuListWidget::currentItemChanged);
    connect(ui->treewgtMainMenuList, &QTreeWidget::itemClicked, this, [=, this](QTreeWidgetItem* item, int column) {
        if (item->childCount() > 0) {
            QModelIndex index = ui->treewgtMainMenuList->indexFromItem(item);
            bool expanded = ui->treewgtMainMenuList->isExpanded(index);
            ui->treewgtMainMenuList->setExpanded(index, !expanded);
        }
    });

}

MainMenuPages MainMenuListWidget::getMainMenuIndex() const
{
    return m_mainMenuIndex.value(ui->treewgtMainMenuList->currentItem());
}

void MainMenuListWidget::createItemToPageMap(Ui::MainMenuListWidget *ui)
{
    m_mainMenuIndex.insert(ui->treewgtMainMenuList->topLevelItem(0),            MainMenuPages::SYSTEM_CONFIGURATION);
    m_mainMenuIndex.insert(ui->treewgtMainMenuList->topLevelItem(1),            MainMenuPages::DRIVE_SETUP_AND_MOTION);
    m_mainMenuIndex.insert(ui->treewgtMainMenuList->topLevelItem(1)->child(0),  MainMenuPages::QUICK_TUNING);
    m_mainMenuIndex.insert(ui->treewgtMainMenuList->topLevelItem(1)->child(1),  MainMenuPages::EXPERT_TUNING);
    m_mainMenuIndex.insert(ui->treewgtMainMenuList->topLevelItem(1)->child(2),  MainMenuPages::MOTION_SINGLE_AXIS);
    m_mainMenuIndex.insert(ui->treewgtMainMenuList->topLevelItem(2),            MainMenuPages::DRIVE_PROGRAMMING);
    m_mainMenuIndex.insert(ui->treewgtMainMenuList->topLevelItem(3),            MainMenuPages::MAESTRO_SETUP_AND_MOTION);
}

void MainMenuListWidget::applyPageIndices(QHash<QTreeWidgetItem *, Constants::VerticalMenuBarConstants::MainMenuPages> &itemToPageMap)
{
    for (auto it = itemToPageMap.constBegin(); it != itemToPageMap.constEnd(); ++it) {
        QTreeWidgetItem* item = it.key();
        MainMenuPages pageIndex = it.value();
        if(item) {
            item->setData(0, Qt::UserRole, static_cast<int>(pageIndex));
        }
    }
}

void MainMenuListWidget::on_treewgtMainMenuList_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    switch (static_cast<MainMenuPages>(item->data(0, Qt::UserRole).toInt())) {
    case MainMenuPages::SYSTEM_CONFIGURATION:
        emit m_StateMachine->getContext()->navigateToSystemConfiguration();
        break;
    case MainMenuPages::DRIVE_SETUP_AND_MOTION:
        emit m_StateMachine->getContext()->navigateToDriveSetupAndMotion();
        break;
    case MainMenuPages::QUICK_TUNING:
        emit m_StateMachine->getContext()->navigateToQuickTuning();
        break;
    case MainMenuPages::EXPERT_TUNING:
        emit m_StateMachine->getContext()->navigateToExpertTuning();
        break;
    case MainMenuPages::MOTION_SINGLE_AXIS:
        emit m_StateMachine->getContext()->navigateToMotionSingleAxis();
        break;
    default:
        break;
    }
}

