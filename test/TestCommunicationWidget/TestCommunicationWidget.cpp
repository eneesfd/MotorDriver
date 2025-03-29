#include "TestCommunicationWidget.h"
#include "ApplicationController.h"
#include <qmenu.h>

TestCommunicationWidget::TestCommunicationWidget(QSharedPointer<ApplicationController> appController, QWidget* parent)
    : QWidget(parent),
    m_appController(appController)
{
    setupUI();
    refreshPorts();
    setupContextMenu();
}

void TestCommunicationWidget::setupUI()
{
    // Ana layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Bağlantı grubu
    QGroupBox* connectionGroup = new QGroupBox("Serial Port Connection", this);
    QHBoxLayout* connectionLayout = new QHBoxLayout(connectionGroup);

    // Port seçici
    QLabel* portLabel = new QLabel("Port:", this);
    m_portSelector = new QComboBox(this);

    // Baud rate seçici
    QLabel* baudLabel = new QLabel("Baud Rate:", this);
    m_baudRateSelector = new QComboBox(this);
    m_baudRateSelector->addItems({"9600", "19200", "38400", "57600", "115200"});
    m_baudRateSelector->setCurrentText("115200");

    // Bağlantı butonları
    m_connectButton = new QPushButton("Connect", this);
    m_disconnectButton = new QPushButton("Disconnect", this);
    m_refreshButton = new QPushButton("Refresh", this);

    m_disconnectButton->setEnabled(false);

    // Connection layout'a ekle
    connectionLayout->addWidget(portLabel);
    connectionLayout->addWidget(m_portSelector);
    connectionLayout->addWidget(baudLabel);
    connectionLayout->addWidget(m_baudRateSelector);
    connectionLayout->addWidget(m_connectButton);
    connectionLayout->addWidget(m_disconnectButton);
    connectionLayout->addWidget(m_refreshButton);

    // Komut grubu
    QGroupBox* commandGroup = new QGroupBox("Send Command", this);
    QHBoxLayout* commandLayout = new QHBoxLayout(commandGroup);

    // Komut girişi
    QLabel* commandLabel = new QLabel("Command:", this);
    m_commandInput = new QLineEdit(this);
    m_commandInput->setPlaceholderText("PA, VR, SR, etc.");

    // Değer girişi
    QLabel* valueLabel = new QLabel("Value:", this);
    m_valueInput = new QLineEdit(this);
    m_valueInput->setPlaceholderText("0.0");

    // İndeks girişi
    QLabel* indexLabel = new QLabel("Index:", this);
    m_indexInput = new QLineEdit(this);
    m_indexInput->setPlaceholderText("-1 (no index)");

    // Gönder butonu
    m_sendButton = new QPushButton("Send", this);

    // Command layout'a ekle
    commandLayout->addWidget(commandLabel);
    commandLayout->addWidget(m_commandInput);
    commandLayout->addWidget(valueLabel);
    commandLayout->addWidget(m_valueInput);
    commandLayout->addWidget(indexLabel);
    commandLayout->addWidget(m_indexInput);
    commandLayout->addWidget(m_sendButton);

    // Log çıktısı
    QGroupBox* logGroup = new QGroupBox("Communication Log", this);
    QVBoxLayout* logLayout = new QVBoxLayout(logGroup);

    m_logOutput = new QTextEdit(this);
    m_logOutput->setReadOnly(true);

    logLayout->addWidget(m_logOutput);

    // Ana layout'a ekle
    mainLayout->addWidget(connectionGroup);
    mainLayout->addWidget(commandGroup);
    mainLayout->addWidget(logGroup);

    // Sinyalleri bağla
    QObject::connect(m_connectButton, &QPushButton::clicked, this, &TestCommunicationWidget::connect);
    QObject::connect(m_disconnectButton, &QPushButton::clicked, this, &TestCommunicationWidget::disconnect);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &TestCommunicationWidget::refreshPorts);
    QObject::connect(m_sendButton, &QPushButton::clicked, this, &TestCommunicationWidget::sendCommand);

    // CommunicationManager sinyallerini bağla
    auto commManager = m_appController->getCommunicationManager();
    if (commManager) {
        QObject::connect(commManager.get(), &CommunicationManager::rawDataReceived,
                         this, [this](const QByteArray& data) {
                             m_logOutput->append(QString("[RX] %1").arg(QString::fromUtf8(data)));
                         });

        QObject::connect(commManager.get(), &CommunicationManager::rawDataSent,
                         this, [this](const QByteArray& data) {
                             m_logOutput->append(QString("[TX] %1").arg(QString::fromUtf8(data)));
                         });

        QObject::connect(commManager.get(), &CommunicationManager::errorOccurred,
                         this, [this](const QString& error) {
                             m_logOutput->append(QString("[ERROR] %1").arg(error));
                         });

        QObject::connect(commManager.get(), &CommunicationManager::connectionStatusChanged,
                         this, [this](bool connected) {
                             m_connectButton->setEnabled(!connected);
                             m_disconnectButton->setEnabled(connected);
                             m_sendButton->setEnabled(connected);

                             m_logOutput->append(QString("[STATUS] %1").arg(
                                 connected ? "Connected" : "Disconnected"));
                         });
    }
}

void TestCommunicationWidget::refreshPorts()
{
    m_portSelector->clear();
    m_portSelector->addItems(m_appController->getAvailablePorts());
}

void TestCommunicationWidget::connect()
{
    QString portName = m_portSelector->currentText();
    int baudRate = m_baudRateSelector->currentText().toInt();

    if (portName.isEmpty()) {
        m_logOutput->append("[ERROR] No port selected");
        return;
    }

    m_logOutput->append(QString("[INFO] Connecting to %1 with baud rate %2")
                            .arg(portName).arg(baudRate));

    m_appController->connectToSerialPort(portName, baudRate);
}

void TestCommunicationWidget::disconnect()
{
    m_logOutput->append("[INFO] Disconnecting");
    m_appController->disconnectFromSerialPort();
}

void TestCommunicationWidget::sendCommand()
{
    QString command = m_commandInput->text().trimmed();
    QString valueStr = m_valueInput->text().trimmed();
    QString indexStr = m_indexInput->text().trimmed();

    if (command.isEmpty()) {
        m_logOutput->append("[ERROR] Command cannot be empty");
        return;
    }

    double value = valueStr.isEmpty() ? 0.0 : valueStr.toDouble();
    int index = indexStr.isEmpty() ? -1 : indexStr.toInt();

    //TODO Burası artık Command Dispatcher ile yapılacak.
    // m_appController->sendCommand(command, value, index);
}

void TestCommunicationWidget::setupContextMenu()
{
    m_logOutput->setContextMenuPolicy(Qt::CustomContextMenu);

   QObject::connect(m_logOutput, &QTextEdit::customContextMenuRequested,
            this, [this](const QPoint &pos) {
                QMenu *menu = m_logOutput->createStandardContextMenu();

                menu->addSeparator();

                QAction *clearAction = new QAction("Clear", this);
                QObject::connect(clearAction, &QAction::triggered, this, &TestCommunicationWidget::clearLog);
                menu->addAction(clearAction);

                menu->exec(m_logOutput->mapToGlobal(pos));

                delete menu;
            });
}

void TestCommunicationWidget::clearLog()
{
    m_logOutput->clear();

    m_logOutput->append("[INFO] Log cleared");
}
