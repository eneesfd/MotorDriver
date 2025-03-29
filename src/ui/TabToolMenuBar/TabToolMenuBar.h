#ifndef TABTOOLMENUBAR_H
#define TABTOOLMENUBAR_H

#include <QWidget>
#include <TabToolbar/TabToolbar.h>
#include <TabToolbar/Page.h>
#include <TabToolbar/Group.h>
#include <TabToolbar/SubGroup.h>
#include <TabToolbar/StyleTools.h>
#include <TabToolbar/Builder.h>
#include <spdlog/spdlog.h>
#include "Constants.h"

class UIStateMachineController;
namespace spdlog {
class logger;
}
class TTBuilder;

namespace Ui {
class TabToolMenuBar;
}

class TabToolMenuBar : public QWidget
{
    Q_OBJECT
public:
    explicit TabToolMenuBar(QWidget *parent = nullptr);
    ~TabToolMenuBar();
    QSharedPointer<tt::TabToolbar> getTabToolbar();
    void setLogger(std::shared_ptr<spdlog::logger> logger);
    void setUIStateMachine(QSharedPointer<UIStateMachineController> stateMachine);

    void setVisibleTabToolBarPages(const QStringList &, Constants::TabToolBarConstants::TabToolVisibility);

signals:
    void SpecialTabClicked();
private slots:
    void onDisconnectStateTabToolBar();
    void onConnectedStateTabToolBar();
    void onQuickTuningModeEntered();
    void onExpertTuningModeEntered();
    void onMotionSingleAxisModeEntered();
    void onMotionSingleAxisModeExited();
    void onQuickTuningModeExited();
    void onExpertTuningModeExited();
    void onSystemConfigurationStateEntered();
    void onSystemConfigurationStateExited();
private:
    Ui::TabToolMenuBar *ui;
    QScopedPointer<tt::Builder> ttb;
    QSharedPointer<tt::TabToolbar> tabToolbar;
    QSharedPointer<UIStateMachineController> m_stateMachine;
    std::shared_ptr<spdlog::logger> m_Logger;
    void disconnectStateSystemConfigurationPage();
    void disconnectStateParametersPage();
    void disableTabToolMenuBarGroup(const QStringList &group);
    void disconnectStateUploadAndDownloadPage();
    void disconnectStateQuickTunnigPage();
    void disconnectStateExpertTunnigPage();
    void connectStateParametersPage();
    void connectStateSystemConfigurationPage();
    void connectStateQuickTunnigPage();
    void connectStateUploadAndDownloadPage();
    void connectStateExpertTunnigPage();
    void enableTabToolMenuBarGroup(const QStringList &group);
    void hideAllTTBarPages();
};

#endif // TABTOOLMENUBAR_H
