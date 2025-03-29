#ifndef BRAKECONTROLLERSTATE_H
#define BRAKECONTROLLERSTATE_H

#include <QState>

namespace spdlog {
class logger;
}

class BrakeControllerState : public QState
{
public:
    explicit BrakeControllerState(std::shared_ptr<spdlog::logger>, QState* parent = nullptr);
signals:
    void brakeControllerStateEntered();
    void brakeControllerStateExited();
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
private:
    std::shared_ptr<spdlog::logger> m_Logger;
};

#endif // BRAKECONTROLLERSTATE_H
