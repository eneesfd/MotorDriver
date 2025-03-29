#ifndef CONNECTEDCONTROLLERSTATE_H
#define CONNECTEDCONTROLLERSTATE_H

#include <QState>

namespace spdlog {
class logger;
}

class ConnectedControllerState : public QState
{
public:
    explicit ConnectedControllerState(std::shared_ptr<spdlog::logger>, QState* parent = nullptr);
signals:
    void connectedControllerStateEntered();
    void connectedControllerStateExited();
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
private:
    std::shared_ptr<spdlog::logger> m_Logger;
};

#endif // CONNECTEDCONTROLLERSTATE_H
