#include "DriveSetupAndMotion.h"
#include "ui_DriveSetupAndMotion.h"
#include "UIStateMachineController.h"
#include "DriveSetupAndMotionController.h"

#include <SettingsManager.h>

using namespace Constants::DriveSetupAndMotionContants;
using namespace Constants::VerticalMenuBarConstants;
using namespace Constants::PanelHeader;

DriveSetupAndMotion::DriveSetupAndMotion(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DriveSetupAndMotion)
{
    ui->setupUi(this);
    createItemToQuickTuningPageMap();
    createItemToExpertTuningPageMap();
    expandTreeWidget();
    navigatorDriveSetupAndMotionPages();
}

DriveSetupAndMotion::~DriveSetupAndMotion()
{
    if (m_currentTuningMode == DriveSetupAndMotionContants::TuningMode::QUICK_TUNING) {
        SettingsManager::getInstance().setLastOpenedQuickTuningPage(ui->stckwgtDriveSetupAndMotionPages->currentIndex());
    } else if (m_currentTuningMode == DriveSetupAndMotionContants::TuningMode::EXPERT_TUNING) {
        SettingsManager::getInstance().setLastOpenedExpertTuningPage(ui->stckwgtDriveSetupAndMotionPages->currentIndex());
    }
    delete ui;
}

void DriveSetupAndMotion::expandTreeWidget()
{
    for (int i = 0; i < ui->treewgtQuickTuning->topLevelItemCount(); ++i) {
        expandAllTreeWidgetItems(ui->treewgtQuickTuning->topLevelItem(i));
    }
    for (int i = 0; i < ui->treewgtExpertTuning->topLevelItemCount(); ++i) {
        expandAllTreeWidgetItems(ui->treewgtExpertTuning->topLevelItem(i));
    }
}

void DriveSetupAndMotion::setupConnections()
{
    //TODO Sabit değerleri constant değerlere ekle
    connect(m_stateMachine.data(), &UIStateMachineController::quickTuningModeEntered, this, [=, this] () {
        m_currentTuningMode = Constants::DriveSetupAndMotionContants::TuningMode::QUICK_TUNING;
        ui->wgtPanelHeader->getWidgetTitleLabel()->setText(tr(QUICK_TUNING));
        ui->stckwgtDriveSetupAndMotion->setCurrentIndex(0);
        ui->stckwgtPageNavigator->setCurrentIndex(0);
        ui->stckwgtDriveSetupAndMotionPages->setCurrentIndex(SettingsManager::getInstance().getLastOpenedQuickTuningPage());
        selectTreeWidgetItemForPage(static_cast<Constants::DriveSetupAndMotionContants::QuickTuningConstants::QuickTuningPages>(ui->stckwgtDriveSetupAndMotionPages->currentIndex()));
        SettingsManager::getInstance().setLastOpenedSubMenu(WindowsListPages::QUICK_TUNING);
    });
    connect(m_stateMachine.data(), &UIStateMachineController::quickTuningModeExited, this, [=,this] () {
        int quickTuningIndex = ui->stckwgtDriveSetupAndMotionPages->currentIndex();
        SettingsManager::getInstance().setLastOpenedQuickTuningPage(quickTuningIndex);
    });

    connect(m_stateMachine.data(), &UIStateMachineController::expertTuningModeEntered, this , [=, this] () {
        m_currentTuningMode = Constants::DriveSetupAndMotionContants::TuningMode::EXPERT_TUNING;
        ui->wgtPanelHeader->getWidgetTitleLabel()->setText(tr(EXPERT_TUNING));
        ui->stckwgtDriveSetupAndMotion->setCurrentIndex(0);
        ui->stckwgtPageNavigator->setCurrentIndex(1);
        ui->stckwgtDriveSetupAndMotionPages->setCurrentIndex(static_cast<int>(SettingsManager::getInstance().getLastOpenedExpertTuningPage()));
        selectTreeWidgetItemForPage(static_cast<Constants::DriveSetupAndMotionContants::ExpertTuningConstants::ExpertTuningPages>(ui->stckwgtDriveSetupAndMotionPages->currentIndex()));
        SettingsManager::getInstance().setLastOpenedSubMenu(WindowsListPages::EXPERT_TUNING);
    });
    connect(m_stateMachine.data(), &UIStateMachineController::expertTuningModeExited, this, [=,this] () {
        int expertTuningIndex = ui->stckwgtDriveSetupAndMotionPages->currentIndex();
        SettingsManager::getInstance().setLastOpenedExpertTuningPage(expertTuningIndex);
    });

    connect(m_stateMachine.data(), &UIStateMachineController::motionSingleAxisModeEntered, this, [=, this] () {
        ui->stckwgtDriveSetupAndMotion->setCurrentIndex(1);
        SettingsManager::getInstance().setLastOpenedSubMenu(WindowsListPages::MOTION_SINGLE_AXIS);
    });
}

void DriveSetupAndMotion::setLogManager(std::shared_ptr<spdlog::logger> log_)
{
    m_logManager = log_;
}

void DriveSetupAndMotion::setUIStateMachine(QSharedPointer<UIStateMachineController> stateMachine)
{
    m_stateMachine = stateMachine;
    setupConnections();
    ui->wgtPageMotionSingleAxis->setUIStateMachine(m_stateMachine);
}

void DriveSetupAndMotion::setControllers(QSharedPointer<DriveSetupAndMotionController> controller_)
{
    m_controller = controller_;
    ui->wgtpageCurrentIdentifcation->setControllers(m_controller->currentIdentificationController());
    ui->wgtPageCurrentLimits->setControllers(m_controller->currentLimitsController());
}

void DriveSetupAndMotion::navigatorDriveSetupAndMotionPages()
{
    connect(ui->btnIncreaseExpertTuning, &QPushButton::clicked, this, [this]() {
        int currentIndex = ui->stckwgtDriveSetupAndMotionPages->currentIndex();
        selectTreeWidgetItemForPage(static_cast<ExpertTuningConstants::ExpertTuningPages>(currentIndex + 1));

    });
    
    connect(ui->btnDecreaseExpertTuning, &QPushButton::clicked, this, [this]() {
        int currentIndex = ui->stckwgtDriveSetupAndMotionPages->currentIndex();
        selectTreeWidgetItemForPage(static_cast<ExpertTuningConstants::ExpertTuningPages>(currentIndex - 1));
    });

    connect(ui->btnIncreaseIndexQuickTuning, &QPushButton::clicked, this, [this]() {
        ++m_quickTuningLogicalIndex;
        if (m_quickTuningLogicalIndex > 4) {
            m_quickTuningLogicalIndex = 4;
        }
        auto page = Constants::DriveSetupAndMotionContants::QuickTuningConstants::fromLogicalIndex(m_quickTuningLogicalIndex);
        selectTreeWidgetItemForPage(page);
    });

    connect(ui->btnDecreaseQuickTuning, &QPushButton::clicked, this, [this]() {
        --m_quickTuningLogicalIndex;
        if (m_quickTuningLogicalIndex < 0) {
            m_quickTuningLogicalIndex = 0;
        }
        auto page = Constants::DriveSetupAndMotionContants::QuickTuningConstants::fromLogicalIndex(m_quickTuningLogicalIndex);
        selectTreeWidgetItemForPage(page);
    });

}

void DriveSetupAndMotion::expandAllTreeWidgetItems(QTreeWidgetItem *item)
{
    if (!item) return;
    item->setExpanded(true);
    for (int i = 0; i < item->childCount(); ++i) {
        expandAllTreeWidgetItems(item->child(i));
    }
}

void DriveSetupAndMotion::createItemToQuickTuningPageMap()
{
    m_treeWidgetQuickTuningIndex.insert(ui->treewgtQuickTuning->topLevelItem(0),               QuickTuningConstants::QuickTuningPages::AXIS_CONFIGURATIONS);
    m_treeWidgetQuickTuningIndex.insert(ui->treewgtQuickTuning->topLevelItem(1)->child(0),     QuickTuningConstants::QuickTuningPages::MOTOR_SETTINGS);
    m_treeWidgetQuickTuningIndex.insert(ui->treewgtQuickTuning->topLevelItem(1)->child(1),     QuickTuningConstants::QuickTuningPages::FEEDBACK_SETTINGS);
    m_treeWidgetQuickTuningIndex.insert(ui->treewgtQuickTuning->topLevelItem(2)->child(0),     QuickTuningConstants::QuickTuningPages::BRAKE);
    m_treeWidgetQuickTuningIndex.insert(ui->treewgtQuickTuning->topLevelItem(3),               QuickTuningConstants::QuickTuningPages::AUTOMATIC_TUNING);
    applyPageIndices(m_treeWidgetQuickTuningIndex);
}

void DriveSetupAndMotion::createItemToExpertTuningPageMap()
{
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(0),               ExpertTuningConstants::ExpertTuningPages::AXIS_CONFIGURATIONS);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(1),               ExpertTuningConstants::ExpertTuningPages::FUNTIONAL_SAFETY);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(2),               ExpertTuningConstants::ExpertTuningPages::SAFETY_IO_CONFIGURATION);
    
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(3)->child(0),     ExpertTuningConstants::ExpertTuningPages::MOTOR_SETTINGS);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(3)->child(1),     ExpertTuningConstants::ExpertTuningPages::FEEDBACK_SETTINGS);
    
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(4)->child(0),     ExpertTuningConstants::ExpertTuningPages::DRIVE_USER_UNITS);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(4)->child(1),     ExpertTuningConstants::ExpertTuningPages::DISPLAY_USER_UNITS);
    
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(5)->child(0),     ExpertTuningConstants::ExpertTuningPages::CURRENT_LIMITS);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(5)->child(1),     ExpertTuningConstants::ExpertTuningPages::MOTION_LIMITS_AND_MUDOLO);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(5)->child(2),     ExpertTuningConstants::ExpertTuningPages::PROTECTIONS);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(5)->child(3),     ExpertTuningConstants::ExpertTuningPages::STOPPING_OPTIONS);
    
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(6)->child(0),     ExpertTuningConstants::ExpertTuningPages::BRAKE);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(6)->child(1),     ExpertTuningConstants::ExpertTuningPages::SETTING_WINDOWS);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(6)->child(2),     ExpertTuningConstants::ExpertTuningPages::INPUTS_AND_OUTPUTS);
    
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(7)->child(0),     ExpertTuningConstants::ExpertTuningPages::CURRENT_IDENTIFICATION);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(7)->child(1),     ExpertTuningConstants::ExpertTuningPages::CURRENT_DESIGN);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(7)->child(2),     ExpertTuningConstants::ExpertTuningPages::CURRENT_VERIFICATION_TIME);
    
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(8),               ExpertTuningConstants::ExpertTuningPages::COMMUTATION);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(9),               ExpertTuningConstants::ExpertTuningPages::STOPPER_CLOSED_LOOP);
    
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(10)->child(0),    ExpertTuningConstants::ExpertTuningPages::GANTRY_YAW_IDENTIFICATION);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(10)->child(1),    ExpertTuningConstants::ExpertTuningPages::GANTRY_YAW_DESIGN);
    
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(11)->child(0),    ExpertTuningConstants::ExpertTuningPages::VELOCITY_AND_POSITION_IDENTIFICATION);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(11)->child(1),    ExpertTuningConstants::ExpertTuningPages::VELOCITY_AND_POSITION_DESIGN);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(11)->child(2),    ExpertTuningConstants::ExpertTuningPages::VELOCITY_AND_POSITION_SCHEDULING);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(11)->child(3),    ExpertTuningConstants::ExpertTuningPages::VELOCITY_AND_POSITION_VERIFICATION_TIME);
    
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(12)->child(0),    ExpertTuningConstants::ExpertTuningPages::ENABLING_DEVICE);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(12)->child(1),    ExpertTuningConstants::ExpertTuningPages::VALIDATION);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(12)->child(2),    ExpertTuningConstants::ExpertTuningPages::ACCEPTANCE);
    
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(13),              ExpertTuningConstants::ExpertTuningPages::SUMMARY);
    m_treeWidgetExpertTuningIndex.insert(ui->treewgtExpertTuning->topLevelItem(14),              ExpertTuningConstants::ExpertTuningPages::AUTOMATIC_TUNING);
    
    applyPageIndices(m_treeWidgetExpertTuningIndex);
}

void DriveSetupAndMotion::applyPageIndices(QHash<QTreeWidgetItem *, QuickTuningConstants::QuickTuningPages> &itemToPageMap)
{
    for (auto it = itemToPageMap.constBegin(); it != itemToPageMap.constEnd(); ++it) {
        QTreeWidgetItem* item = it.key();
        QuickTuningConstants::QuickTuningPages pageIndex = it.value();
        item->setData(0, Qt::UserRole, static_cast<int>(pageIndex));
    }
}

void DriveSetupAndMotion::applyPageIndices(
    QHash<QTreeWidgetItem *, ExpertTuningConstants::ExpertTuningPages> &itemToPageMap)
{
    for (auto it = itemToPageMap.constBegin(); it != itemToPageMap.constEnd(); ++it) {
        QTreeWidgetItem* item = it.key();
        ExpertTuningConstants::ExpertTuningPages pageIndex = it.value();
        item->setData(0, Qt::UserRole, static_cast<int>(pageIndex));
    }
}

void DriveSetupAndMotion::selectTreeWidgetItemForPage(QuickTuningConstants::QuickTuningPages page)
{
    for (auto it = m_treeWidgetQuickTuningIndex.begin(); it != m_treeWidgetQuickTuningIndex.end(); ++it) {
        if (it.value() == page) {
            ui->treewgtQuickTuning->setCurrentItem(it.key());
            ui->treewgtQuickTuning->setFocus();
            return;
        }
    }
}

void DriveSetupAndMotion::on_treewgtQuickTuning_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (!current)
        return;
    QVariant pageVariant = current->data(0, Qt::UserRole);
    if(pageVariant.isValid()) {
        int pageIndex = pageVariant.toInt();
        ui->stckwgtDriveSetupAndMotionPages->setCurrentIndex(pageIndex);
    }
}

void DriveSetupAndMotion::selectTreeWidgetItemForPage(ExpertTuningConstants::ExpertTuningPages page)
{
    for (auto it = m_treeWidgetExpertTuningIndex.begin(); it != m_treeWidgetExpertTuningIndex.end(); ++it) {
        if (it.value() == page) {
            ui->treewgtExpertTuning->setCurrentItem(it.key());
            ui->treewgtExpertTuning->setFocus();
            return;
        }
    }
}

void DriveSetupAndMotion::on_treewgtExpertTuning_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (!current)
        return;
    QVariant pageVariant = current->data(0, Qt::UserRole);
    if(pageVariant.isValid()) {
        int pageIndex = pageVariant.toInt();
        ui->stckwgtDriveSetupAndMotionPages->setCurrentIndex(pageIndex);
    }
}

