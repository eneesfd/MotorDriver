#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QMap>
#include <QString>
#include <QComboBox>
#include <memory>
#include "BaseController/AbstractController.h"
#include "BusinessStateMachineController.h"
#include "UIStateMachineController.h"
#include "LogManager.h"
#include "../core/CommunicationManager/CommunicationManager.h"
#include "MotorDriverManager.h"
#include "VerticalMenuBarController.h"
#include "DriveSetupAndMotionController.h"
#include "TabToolMenuBarController.h"
#include "PreferencesController.h"
#include "../controllers/StateMachine/Communication/StateCommunicationManager.h"


class MainWindow;
class VerticalMenuBar;
class BusinessStateMachineController;

class ApplicationController : public AbstractController
{
    Q_OBJECT
public:
    explicit ApplicationController(QSharedPointer<BusinessStateMachineController> businessStateController_,
                                   QSharedPointer<UIStateMachineController> uistateMachineController_,
                                   QObject *parent = nullptr);
    ~ApplicationController() override = default;

    void createVerticalMenuBar(VerticalMenuBar* verticalMenuBar);

    // Seri port bağlantı ve komut fonksiyonları
    void connectToSerialPort(const QString& portName, int baudRate);
    void disconnectFromSerialPort();
    QStringList getAvailablePorts();

    // CommunicationManager erişimcisi
    QSharedPointer<CommunicationManager> getCommunicationManager() const {
        return m_communicationManager;
    }
    QSharedPointer<DriveSetupAndMotionController> getDriveSetupAndMotionController();

private slots:
    void onConnectionStatusChanged(bool connected);
    // void onMotorStatusChanged(const QVariantMap& status);
    void onCommunicationError(const QString& error);

private:
    void setupConnections();
    void setupCommunicationManager();

    QSharedPointer<BusinessStateMachineController> m_businessStateController;
    QSharedPointer<UIStateMachineController> m_uiStateMachineController;

    QVector<std::shared_ptr<LogManager>> m_logManagerCollection;
    QMap<QString, std::shared_ptr<spdlog::logger>> m_loggerRegistry;

    QScopedPointer<VerticalMenuBarController> m_menuBarController;
    QSharedPointer<DriveSetupAndMotionController> m_driveSetupAndMotionController;
    QScopedPointer<TabToolMenuBarController> m_tabToolMenuBarController;
    QScopedPointer<PreferencesController> m_preferencesController;
    QScopedPointer<MotorDriver::MotorDriverManager> m_motorDriverManager;

    // Communication manager
    QSharedPointer<CommunicationManager> m_communicationManager;

    QSharedPointer<CommandDispatcher> m_commandDispatcher;
    sp<StateCommunicationManager> m_stateCommunicationManager;
};
