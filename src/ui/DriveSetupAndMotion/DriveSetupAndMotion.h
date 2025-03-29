#ifndef DRIVESETUPANDMOTION_H
#define DRIVESETUPANDMOTION_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QStackedWidget>
#include "Constants.h"
#include <spdlog/logger.h>

namespace Ui {
class DriveSetupAndMotion;
}

class UIStateMachineController;
class DriveSetupAndMotionController;


class DriveSetupAndMotion : public QWidget
{
    Q_OBJECT

public:
    explicit DriveSetupAndMotion(QWidget *parent = nullptr);
    ~DriveSetupAndMotion();
    void setLogManager(std::shared_ptr<spdlog::logger> log_);
    void setUIStateMachine(QSharedPointer<UIStateMachineController> stateMachine);
    void setControllers(QSharedPointer<DriveSetupAndMotionController> controller_);
private slots:
    void on_treewgtQuickTuning_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_treewgtExpertTuning_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    Ui::DriveSetupAndMotion *ui;
    void expandAllTreeWidgetItems(QTreeWidgetItem *item);

    void createItemToQuickTuningPageMap();
    void createItemToExpertTuningPageMap();

    void applyPageIndices(QHash<QTreeWidgetItem*, Constants::DriveSetupAndMotionContants::QuickTuningConstants::QuickTuningPages>& itemToPageMap);
    void applyPageIndices(QHash<QTreeWidgetItem*, Constants::DriveSetupAndMotionContants::ExpertTuningConstants::ExpertTuningPages>& itemToPageMap);

    QHash<QTreeWidgetItem*, Constants::DriveSetupAndMotionContants::QuickTuningConstants::QuickTuningPages> m_treeWidgetQuickTuningIndex;
    QHash<QTreeWidgetItem*, Constants::DriveSetupAndMotionContants::ExpertTuningConstants::ExpertTuningPages> m_treeWidgetExpertTuningIndex;

    std::shared_ptr<spdlog::logger> m_logManager;

    void selectTreeWidgetItemForPage(Constants::DriveSetupAndMotionContants::QuickTuningConstants::QuickTuningPages);
    void selectTreeWidgetItemForPage(Constants::DriveSetupAndMotionContants::ExpertTuningConstants::ExpertTuningPages);

    QSharedPointer<UIStateMachineController> m_stateMachine;
    void navigatorDriveSetupAndMotionPages();
    void expandTreeWidget();
    void setupConnections();
    int m_quickTuningLogicalIndex = 0;
    Constants::DriveSetupAndMotionContants::TuningMode m_currentTuningMode;
    QSharedPointer<DriveSetupAndMotionController> m_controller;
};

#endif // DRIVESETUPANDMOTION_H
