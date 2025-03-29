#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "LogManager.h"

#include <EventAggregator.cpp>
#include <QDockWidget>
#include "SettingsManager.h"
#include "../test/TestCommunicationWidget/TestCommunicationWidget.h"

//LOGGER İSİMLENDİRME İÇİN
#define LOG_MAINWINDOW 0
#define NAME_MAINWINDOW "MainWindow"

#define LOG_UI_STATE_MACHINE 1
#define NAME_UI_STATE_MACHINE "UIStateMachine"
//LOGGER İSİMLENDİRME İÇİN

//PAGE NUMARALARI İÇİN İSİMLENDİRME
#define PAGE_FILE 0
#define PAGE_MAINWINDOW 1
//PAGE NUMARALARI İÇİN İSİMLENDİRME

MainWindow::MainWindow(QSharedPointer<ApplicationController> appController_, QSharedPointer<UIStateMachineController> uiStateMachine_, QWidget *parent)
    : QMainWindow(parent)
    , m_applicationController(appController_)
    , m_uiStateMachineController(uiStateMachine_)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_logManagerCollection.append(std::make_shared<LogManager>(NAME_MAINWINDOW));
    this->m_logManagerCollection.at(LOG_MAINWINDOW)->addConsoleSink();
    this->m_logManagerCollection.at(LOG_MAINWINDOW)->setLogLevel(spdlog::level::trace);
    m_loggerRegistry.insert(NAME_MAINWINDOW, m_logManagerCollection.at(LOG_MAINWINDOW)->start());

    ui->wgtDriveSetupAndMotion->setLogManager(m_loggerRegistry[NAME_MAINWINDOW]);

    initializeAdvancedStyleSheet();
    SpdlogInfo(m_loggerRegistry[NAME_MAINWINDOW], "UI Yaratıldı");

    setupConnections();

    ui->wgtTabToolMenuBar->setUIStateMachine(m_uiStateMachineController);
    ui->wgtTabToolMenuBar->setLogger(m_loggerRegistry[NAME_MAINWINDOW]);

    ui->wgtDriveSetupAndMotion->setUIStateMachine(m_uiStateMachineController);
    ui->wgtVerticalMenuBar->setUIStateMachine(m_uiStateMachineController);

    QLabel *statusLabel = new QLabel("System Configuration Activity");
    statusLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed); // Çizgiyi engeller
    statusBar()->setContentsMargins(0, 0, 0, 0);
    statusLabel->setStyleSheet("padding-left: 10px; color: white; background-color: transparent; ");  // Soldan 10px boşluk ekler
    statusBar()->addWidget(statusLabel);    
    setupDockLightTest();

    setupControllers();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupDockLightTest()
{
    // Test widget oluştur
    auto testWidget = new TestCommunicationWidget(m_applicationController);

    // Dock widget oluştur
    QDockWidget* dockWidget = new QDockWidget("Docklight Test", this);
    dockWidget->setWidget(testWidget);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    // Ana pencereye ekle
    addDockWidget(Qt::RightDockWidgetArea, dockWidget);
}

void MainWindow::setupConnections()
{
    //TODO BUNLARI AYRI METODA AL
    connect(m_uiStateMachineController.data(),&UIStateMachineController::systemConfigurationStateEntered, this, [=, this](){
        //TODO bunu applicationcontroller taşı
        // m_applicationController->createVerticalMenuBar(ui->wgtVerticalMenuBar);
        //bunu applicationcontroller taşı
        ui->stckWgtMainMenuList->setCurrentIndex(static_cast<int>(VerticalMenuBarConstants::MainMenuPages::SYSTEM_CONFIGURATION));
        SettingsManager::getInstance().setLastOpenedWindow(static_cast<int>(VerticalMenuBarConstants::MainMenuPages::SYSTEM_CONFIGURATION));
    });
    connect(m_uiStateMachineController.data(), &UIStateMachineController::systemConfigurationStateExited, this, [=, this](){
    });

    connect(m_uiStateMachineController.data(), &UIStateMachineController::driveSetupAndMotionStateEntered, this, [=, this](){
        ui->stckWgtMainMenuList->setCurrentIndex(static_cast<int>(VerticalMenuBarConstants::MainMenuPages::DRIVE_SETUP_AND_MOTION));
        SettingsManager::getInstance().setLastOpenedWindow(static_cast<int>(VerticalMenuBarConstants::MainMenuPages::DRIVE_SETUP_AND_MOTION));
    });
    connect(m_uiStateMachineController.data(), &UIStateMachineController::driveSetupAndMotionStateExited, this, [=, this](){
    });

    // BUNLARI AYRI METODA AL

    connect(ui->wgtPreferences, &Preferences::backButtonClicked, this, &MainWindow::onPreferencesButtonClicked);

    connect(ui->wgtTabToolMenuBar, &TabToolMenuBar::SpecialTabClicked, this, &MainWindow::onSpecialTabClicked);

    connect(&EventAggregator::instance(), &EventAggregator::changeApplicationTheme, this, [=, this](const QString &theme){
        m_advancedStyleSheet.data()->setCurrentTheme(theme);
        m_advancedStyleSheet.data()->updateStylesheet();
        this->setStyleSheet(m_advancedStyleSheet->styleSheet());
    });

    connect(ui->wgtVerticalMenuBar, &VerticalMenuBar::workSpaceParentItemClicked, ui->wgtSystemConfiguration, &SystemConfiguration::onWorkSpaceParentItemSelected);
    connect(ui->wgtVerticalMenuBar, &VerticalMenuBar::workSpaceChildItemClicked, ui->wgtSystemConfiguration, &SystemConfiguration::onWorkSpaceChildItemSelected);

}

void MainWindow::setupControllers()
{
    ui->wgtDriveSetupAndMotion->setControllers(m_applicationController->getDriveSetupAndMotionController());
}

void MainWindow::initializeAdvancedStyleSheet()
{
    QString AppDir = qApp->applicationDirPath();
    QString StylesDir = AppDir + "/styles";

    m_advancedStyleSheet.reset(new acss::QtAdvancedStylesheet(this));
    m_advancedStyleSheet->setStylesDirPath(StylesDir);
    m_advancedStyleSheet->setOutputDirPath(AppDir + "/output");
    m_advancedStyleSheet->setCurrentStyle("motorDriver_material");
    m_advancedStyleSheet->setCurrentTheme("ast_colors");
    m_advancedStyleSheet->updateStylesheet();
    qApp->setStyleSheet(m_advancedStyleSheet->styleSheet());
}

void MainWindow::onSpecialTabClicked()
{
    ui->stckWgtMainWindow->setCurrentIndex(PAGE_FILE);
}

void MainWindow::onPreferencesButtonClicked()
{
    ui->stckWgtMainWindow->setCurrentIndex(1);
}
