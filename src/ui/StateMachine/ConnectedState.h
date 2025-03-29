#ifndef CONNECTEDSTATE_H
#define CONNECTEDSTATE_H

#include <QState>

namespace spdlog {
class logger;
}

class ConnectedState : public QState
{
    Q_OBJECT
public:
    ConnectedState(std::shared_ptr<spdlog::logger> logger_, QState* parent = nullptr);
signals:
    void connectedModeEntered();
    void connectedModeExited();
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
private:
    std::shared_ptr<spdlog::logger> m_Logger;
};

#endif // CONNECTEDSTATE_H
