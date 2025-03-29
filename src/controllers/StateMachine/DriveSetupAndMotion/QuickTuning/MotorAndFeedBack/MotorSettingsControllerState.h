#ifndef MOTORSETTINGSCONTROLLERSTATE_H
#define MOTORSETTINGSCONTROLLERSTATE_H

#include <QState>

namespace spdlog {
class logger;
}

class MotorSettingsControllerState : public QState
{
public:
    MotorSettingsControllerState(std::shared_ptr<spdlog::logger>, QState* parent = nullptr);
signals:
    void motorSettingsControllerStateEntered();
    void motorSettingsControllerStateExited();
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
private:
    std::shared_ptr<spdlog::logger> m_Logger;
};

#endif // MOTORSETTINGSCONTROLLERSTATE_H
