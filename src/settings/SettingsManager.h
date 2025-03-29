#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>
#include <QString>
#include <QVariant>
#include "Constants.h"

using namespace Constants;

class SettingsManager {
public:
    static SettingsManager& getInstance() {
        static SettingsManager instance;
        return instance;
    }

    inline void setLastOpenedQuickTuningPage(int page) {
        settings.beginGroup(SettingsManagerConstants::L1_PAGE_GROUP);
        settings.setValue(SettingsManagerConstants::LAST_QUICK_PAGE, QString::number(page));
        settings.endGroup();
    }

    inline int getLastOpenedQuickTuningPage() {
        settings.beginGroup(SettingsManagerConstants::L1_PAGE_GROUP);
        int quickTuningPage = settings.value(SettingsManagerConstants::LAST_QUICK_PAGE, 0).toInt();
        settings.endGroup();
        return quickTuningPage;
    }

    inline void setLastOpenedExpertTuningPage(int page) {
        settings.beginGroup(SettingsManagerConstants::L1_PAGE_GROUP);
        settings.setValue(SettingsManagerConstants::LAST_EXPERT_PAGE, QString::number(page));
        settings.endGroup();
    }

    inline DriveSetupAndMotionContants::ExpertTuningConstants::ExpertTuningPages getLastOpenedExpertTuningPage() {
        settings.beginGroup(SettingsManagerConstants::L1_PAGE_GROUP);
        DriveSetupAndMotionContants::ExpertTuningConstants::ExpertTuningPages expertTuningPage = static_cast<DriveSetupAndMotionContants::ExpertTuningConstants::ExpertTuningPages>
            (settings.value(SettingsManagerConstants::LAST_EXPERT_PAGE, 0).toInt());
        settings.endGroup();
        return expertTuningPage;
    }

    inline void setLastOpenedWindow(int tab) {
        settings.beginGroup(SettingsManagerConstants::L1_PAGE_GROUP);
        settings.setValue(SettingsManagerConstants::LAST_OPENED_WINDOW, QString::number(tab));
        settings.endGroup();
    }

    inline VerticalMenuBarConstants::MainMenuPages getLastOpenedWindow() {
        settings.beginGroup(SettingsManagerConstants::L1_PAGE_GROUP);
        VerticalMenuBarConstants::MainMenuPages lastOpenedWindow = static_cast<VerticalMenuBarConstants::MainMenuPages>
            (settings.value(SettingsManagerConstants::LAST_OPENED_WINDOW, 0).toInt());
        settings.endGroup();
        return lastOpenedWindow;
    }

    inline void setLastOpenedSubMenu(VerticalMenuBarConstants::WindowsListPages subMenu) {
        settings.beginGroup(SettingsManagerConstants::L1_PAGE_GROUP);
        settings.setValue(SettingsManagerConstants::LAST_OPENED_SUB_MENU, QString::number(static_cast<int>(subMenu)));
        settings.endGroup();
    }
    inline void setLastOpenedSubMenu(int subMenu) {
        settings.beginGroup(SettingsManagerConstants::L1_PAGE_GROUP);
        settings.setValue(SettingsManagerConstants::LAST_OPENED_SUB_MENU, QString::number(subMenu));
        settings.endGroup();
    }

    inline VerticalMenuBarConstants::WindowsListPages getLastOpenedSubMenu() {
        settings.beginGroup(SettingsManagerConstants::L1_PAGE_GROUP);
        VerticalMenuBarConstants::WindowsListPages lastOpenedSubmenu = static_cast<VerticalMenuBarConstants::WindowsListPages>
            (settings.value(SettingsManagerConstants::LAST_OPENED_SUB_MENU, 0).toInt());
        settings.endGroup();
        return lastOpenedSubmenu;
    }

    inline void setWorkspace(const QString &workspace) {
        settings.beginGroup(SettingsManagerConstants::L1_WORKSPACE_GROUP);
        settings.setValue(SettingsManagerConstants::L1_WORKSPACE_GROUP, "Default");
        settings.endGroup();
    }

    inline const QString getWorkspace() {
        settings.beginGroup(SettingsManagerConstants::L1_WORKSPACE_GROUP);
        QString workspaceName = settings.value(SettingsManagerConstants::L1_WORKSPACE_GROUP).toString();
        settings.endGroup();
        return workspaceName;
    }


private:
    QSettings settings;

    SettingsManager() : settings(SettingsManagerConstants::COMPANY_NAME, SettingsManagerConstants::APPLICATION_NAME) {}

    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;
};

#endif // SETTINGSMANAGER_H
