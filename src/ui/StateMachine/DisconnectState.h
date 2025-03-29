#ifndef DISCONNECTSTATE_H
#define DISCONNECTSTATE_H

#include <QState>

namespace spdlog {
class logger;
}

class DisconnectState : public QState
{
    Q_OBJECT
public:
    DisconnectState(std::shared_ptr<spdlog::logger>, QState* parent = nullptr);
signals:
    void disconnectStateEntered();
    void disconnectStateExited();
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
private:
    std::shared_ptr<spdlog::logger> m_Logger;
};

#endif // DISCONNECTSTATE_H
