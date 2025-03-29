#ifndef DISCONNECTCONTROLLERSTATE_H
#define DISCONNECTCONTROLLERSTATE_H

#include <QState>

namespace spdlog {
class logger;
}

class DisconnectControllerState : public QState
{
public:
    explicit DisconnectControllerState(std::shared_ptr<spdlog::logger> logger_, QState* parent = nullptr);
signals:
    void disconnectControllerStateEntered();
    void disconnectControllerStateExited();
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
private:
    std::shared_ptr<spdlog::logger> m_Logger;
};

#endif // DISCONNECTCONTROLLERSTATE_H
