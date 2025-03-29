#ifndef DRIVESETUPANDMOTIONCONTROLLERSTATE_H
#define DRIVESETUPANDMOTIONCONTROLLERSTATE_H

#include <QState>

namespace spdlog {
class logger;
}

class DriveSetupAndMotionControllerState : public QState
{
public:
    explicit DriveSetupAndMotionControllerState(std::shared_ptr<spdlog::logger> logger, QState* parent = nullptr);
signals:
    void driveSetupAndMotionControllerStateEntered();
    void driveSetupAndMotionControllerStateExited();
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
private:
    std::shared_ptr<spdlog::logger> m_Logger;
};

#endif // DRIVESETUPANDMOTIONCONTROLLERSTATE_H
