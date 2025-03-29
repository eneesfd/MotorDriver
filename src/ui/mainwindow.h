#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ApplicationController.h"
#include <QtAdvancedStylesheet.h>
#include "UIStateMachineController.h"
#include "BusinessStateMachineController.h"

class VerticalMenuBar;
class DriveSetupAndMotion;
class LogManager;
class Preferences;
class SystemConfiguration;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QSharedPointer<ApplicationController> , QSharedPointer<UIStateMachineController>, QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    QVector<std::shared_ptr<LogManager>> m_logManagerCollection;
    QMap<QString, std::shared_ptr<spdlog::logger>> m_loggerRegistry;
    QSharedPointer<ApplicationController> m_applicationController;
    QSharedPointer<acss::QtAdvancedStylesheet> m_advancedStyleSheet;
    QSharedPointer<UIStateMachineController> m_uiStateMachineController;
    void initializeAdvancedStyleSheet();
public slots:
    void onSpecialTabClicked();
    void onPreferencesButtonClicked();
    void setupConnections();
    void setupControllers();
    void setupDockLightTest();
};
#endif // MAINWINDOW_H
