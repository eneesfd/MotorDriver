#include "ApplicationController.h"
#include <EventAggregator.cpp>
#include <iostream>
#include "../core/CommunicationManager/CommunicationManager.h"
#include "../core/CommandDispatcher/CommandDispatcher.h"
#include "../core/CommandDispatcher/PeriodicQueryManager.h"

#define LOG_APPLICATION_CONTROLLER 0
#define NAME_APPLICATION_CONTROLLER "ApplicationController"
#define LOG_MENU_BAR_CONTROLLER 1
#define NAME_MENU_BAR_CONTROLLER "MenuBarController"
#define LOG_DRIVE_SETUP_AND_MOTION_CONTROLLER 2
#define NAME_DRIVE_SETUP_AND_MOTION_CONTROLLER "DriveSetupAndMotionController"
#define LOG_TAB_TOOL_MENU_BAR_CONTROLLER 3
#define NAME_TAB_TOOL_MENU_BAR_CONTROLLER "TabToolMenuBarController"
#define LOG_PREFERENCES_CONTROLLER 4
#define NAME_PREFERENCES_CONTROLLER "PreferencesController"
#define LOG_COMMUNICATION_MANAGER 5
#define NAME_COMMUNICATION_MANAGER "CommunicationManager"

ApplicationController::ApplicationController(QSharedPointer<BusinessStateMachineController> businessStateController_, QSharedPointer<UIStateMachineController> uistateMachineController_, QObject *parent)
    : AbstractController(parent),
    m_businessStateController{businessStateController_},
    m_uiStateMachineController{uistateMachineController_}
{
    try {
        m_logManagerCollection.push_back(std::make_shared<LogManager>(NAME_APPLICATION_CONTROLLER));
        // CommunicationManager için logger ekleme
        m_logManagerCollection.push_back(std::make_shared<LogManager>(NAME_COMMUNICATION_MANAGER));
    }
    catch(std::exception& e) {
        std::cerr << "Exception caught in ApplicationController constructor: " << e.what() << std::endl;
    }

    this->m_logManagerCollection.at(LOG_APPLICATION_CONTROLLER)->addConsoleSink();
    this->m_logManagerCollection.at(LOG_APPLICATION_CONTROLLER)->setLogLevel(spdlog::level::trace);
    m_loggerRegistry.insert(NAME_APPLICATION_CONTROLLER, m_logManagerCollection.at(LOG_APPLICATION_CONTROLLER)->start());

    // // CommunicationManager için logger'ı yapılandırma
    // this->m_logManagerCollection.at(LOG_COMMUNICATION_MANAGER)->addConsoleSink();
    // this->m_logManagerCollection.at(LOG_COMMUNICATION_MANAGER)->setLogLevel(spdlog::level::debug);
    // m_loggerRegistry.insert(NAME_COMMUNICATION_MANAGER, m_logManagerCollection.at(LOG_COMMUNICATION_MANAGER)->start());

    SpdlogInfo(m_loggerRegistry[NAME_APPLICATION_CONTROLLER], NAME_APPLICATION_CONTROLLER);

    try {
        m_stateCommunicationManager = QSharedPointer<StateCommunicationManager>::create(
            m_uiStateMachineController->getContext(),
            m_businessStateController->context(),
            this);
    } catch (std::exception& e) {
        std::cerr << "Exception caught in ApplicationController constructor: " << e.what() << std::endl;
    }

    // CommunicationManager'ı yapılandır
    setupCommunicationManager();
    setupConnections();
}

void ApplicationController::setupCommunicationManager()
{
    // CommunicationManager oluştur
    m_communicationManager = QSharedPointer<CommunicationManager>::create(
        m_loggerRegistry[NAME_APPLICATION_CONTROLLER],this);

    m_commandDispatcher = QSharedPointer<CommandDispatcher>::create(
        m_loggerRegistry[NAME_APPLICATION_CONTROLLER],this);

    m_commandDispatcher->setCommunicationManager(m_communicationManager);
    setAll(m_businessStateController->context(), m_loggerRegistry[NAME_APPLICATION_CONTROLLER], m_commandDispatcher);
    PeriodicQueryManager::initialize(commandDispatcher(), logger());
    // Sinyalleri bağla
    connect(m_communicationManager.get(), &CommunicationManager::connectionStatusChanged,
            this, &ApplicationController::onConnectionStatusChanged);

    // connect(m_communicationManager.get(), &CommunicationManager::motorStatusChanged,
    //         this, &ApplicationController::onMotorStatusChanged);

    connect(m_communicationManager.get(), &CommunicationManager::errorOccurred,
            this, &ApplicationController::onCommunicationError);

    // Otomatik durum güncellemeyi aktifleştir
    m_communicationManager->enableStatusPolling(true);
    m_communicationManager->setStatusPollingInterval(100); // 100ms

    // Otomatik yeniden bağlanmayı aktifleştir
    m_communicationManager->enableAutoReconnect(false);

    // Trafik loglamayı aktifleştir
    m_communicationManager->enableTrafficLogging(true);

    SpdlogInfo(m_loggerRegistry[NAME_APPLICATION_CONTROLLER], "CommunicationManager initialized");
}

void ApplicationController::connectToSerialPort(const QString& portName, int baudRate)
{
    SpdlogInfo(m_loggerRegistry[NAME_APPLICATION_CONTROLLER],
               fmt::format("Connecting to serial port {} with baud rate {}",
                           portName.toStdString(), baudRate));

    // SerialPort bağlantı parametrelerini ayarla
    QVariantMap channelParams;
    channelParams["portName"] = portName;
    channelParams["baudRate"] = baudRate;
    channelParams["dataBits"] = 8;
    channelParams["parity"] = 0;  // NoParity
    channelParams["stopBits"] = 1;  // OneStop
    channelParams["flowControl"] = 0;  // NoFlowControl

    // SimpleIQ protokol parametrelerini ayarla
    QVariantMap protocolParams;
    protocolParams["commandDelayMs"] = 5;
    protocolParams["responseTimeoutMs"] = 1000;
    protocolParams["maxRetryCount"] = 3;
    protocolParams["autoRetry"] = true;
    protocolParams["terminationChar"] = "\r";  // SimpleIQ için CR karakteri

    // CommunicationManager'ı yapılandır
    bool configResult = m_communicationManager->configure(
        "SerialPort", "SimpleIQ", channelParams, protocolParams);

    if (!configResult) {
        SpdlogError(m_loggerRegistry[NAME_APPLICATION_CONTROLLER],
                    "Failed to configure communication: " +
                        m_communicationManager->lastError().toStdString());
        return;
    }

    // Bağlantıyı aç
    bool connectResult = m_communicationManager->connect();

    if (connectResult) {
        SpdlogInfo(m_loggerRegistry[NAME_APPLICATION_CONTROLLER], "Connected to serial port");
    } else {
        SpdlogError(m_loggerRegistry[NAME_APPLICATION_CONTROLLER],
                    "Failed to connect: " + m_communicationManager->lastError().toStdString());
    }
}

void ApplicationController::disconnectFromSerialPort()
{
    SpdlogInfo(m_loggerRegistry[NAME_APPLICATION_CONTROLLER], "Disconnecting from serial port");

    if (m_communicationManager) {
        m_communicationManager->disconnect();
    }
}

// void ApplicationController::sendCommand(const QString& commandName, double value, int index = -1)
// {
//     if (!m_communicationManager || !m_communicationManager->isConnected()) {
//         SpdlogError(m_loggerRegistry[NAME_APPLICATION_CONTROLLER], "Not connected to serial port");
//         return;
//     }
//     // Komut oluştur
//     auto command = std::make_shared<Command>(value, commandName, index, CommandType::Query);
//     // Komutu gönder
//     SpdlogInfo(m_loggerRegistry[NAME_APPLICATION_CONTROLLER],
//                fmt::format("Sending command: {}", command->toString().toStdString()));
    // auto response = m_communicationManager->sendCommand(command, 10000);

//     if (response.has_value()) {
//         std::shared_ptr<Response> resp = response.value();

//         // Yanıt tipine göre uygun loglama yap
//         if (resp->type() == ResponseType::Text) {
//             // Metin yanıtı için parametre olarak kaydedilen text değerini al
//             auto textParam = resp->getParameterValue("text");
//             if (textParam.has_value() && std::holds_alternative<QString>(textParam.value())) {
//                 QString textValue = std::get<QString>(textParam.value());
//                 SpdlogInfo(m_loggerRegistry[NAME_APPLICATION_CONTROLLER],
//                            fmt::format("Text response: {}", textValue.toStdString()));
//             } else {
//                 // Alternatif olarak ham yanıtı kullan
//                 SpdlogInfo(m_loggerRegistry[NAME_APPLICATION_CONTROLLER],
//                            fmt::format("Raw response: {}", resp->rawResponse().toStdString()));
//             }
//         } else {
//             // Sayısal yanıt için value() değerini kullan
//             SpdlogInfo(m_loggerRegistry[NAME_APPLICATION_CONTROLLER],
//                        fmt::format("Numeric response: {}", resp->value()));
//         }
//     } else {
//         SpdlogError(m_loggerRegistry[NAME_APPLICATION_CONTROLLER],
//                     "No response received: " + m_communicationManager->lastError().toStdString());
//     }
// }

QStringList ApplicationController::getAvailablePorts()
{
    QStringList portNames;

    if (m_communicationManager) {
        QVariantList ports = m_communicationManager->availableChannels();

        for (const QVariant& port : std::as_const(ports)) {
            QVariantMap portInfo = port.toMap();
            QString portName = portInfo["portName"].toString();
            portNames.append(portName);
        }
    }

    return portNames;
}

QSharedPointer<DriveSetupAndMotionController> ApplicationController::getDriveSetupAndMotionController()
{
    return m_driveSetupAndMotionController;
}

// Durum değişikliği sinyallerini işleyen slot'lar
void ApplicationController::onConnectionStatusChanged(bool connected)
{
    SpdlogInfo(m_loggerRegistry[NAME_APPLICATION_CONTROLLER],
               fmt::format("Connection status changed: {}", connected ? "Connected" : "Disconnected"));
}

// void ApplicationController::onMotorStatusChanged(const QVariantMap& status)
// {
//     bool enabled = status["enabled"].toBool();
//     bool moving = status["moving"].toBool();
//     bool error = status["error"].toBool();

//     SpdlogInfo(m_loggerRegistry[NAME_APPLICATION_CONTROLLER],
//                fmt::format("Motor status changed: Enabled={}, Moving={}, Error={}",
//                            enabled, moving, error));
// }

void ApplicationController::onCommunicationError(const QString& error)
{
    SpdlogError(m_loggerRegistry[NAME_APPLICATION_CONTROLLER],
                fmt::format("Communication error: {}", error.toStdString()));
}

void ApplicationController::setupConnections()
{
    m_driveSetupAndMotionController.reset(new DriveSetupAndMotionController(m_businessStateController->context(),
                                                                            m_commandDispatcher,
                                                                            m_loggerRegistry[NAME_APPLICATION_CONTROLLER],
                                                                            this));
    m_tabToolMenuBarController.reset(new TabToolMenuBarController(m_loggerRegistry[NAME_APPLICATION_CONTROLLER], this));
    m_preferencesController.reset(new PreferencesController(m_loggerRegistry[NAME_APPLICATION_CONTROLLER], this));
    m_motorDriverManager.reset(new MotorDriver::MotorDriverManager(m_loggerRegistry[NAME_APPLICATION_CONTROLLER]));
}
