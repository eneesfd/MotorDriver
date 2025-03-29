#pragma once

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTextEdit>
#include <memory>

class ApplicationController;

class TestCommunicationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestCommunicationWidget(QSharedPointer<ApplicationController> appController, QWidget* parent = nullptr);
    ~TestCommunicationWidget() override = default;
private slots:
    void refreshPorts();
    void connect();
    void disconnect();
    void sendCommand();
    void clearLog();
private:
    void setupUI();

    QSharedPointer<ApplicationController> m_appController;

    // UI elemanları
    QComboBox* m_portSelector;
    QComboBox* m_baudRateSelector;
    QPushButton* m_connectButton;
    QPushButton* m_disconnectButton;
    QPushButton* m_refreshButton;

    QLineEdit* m_commandInput;
    QLineEdit* m_valueInput;
    QLineEdit* m_indexInput;
    QPushButton* m_sendButton;

    QTextEdit* m_logOutput;
    void setupContextMenu();
};
