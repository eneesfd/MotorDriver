#ifndef DRIVESETUPANDMOTIONSTATE_H
#define DRIVESETUPANDMOTIONSTATE_H

#include <QState>

namespace spdlog {
class logger;
}

class DriveSetupandMotionState : public QState
{
    Q_OBJECT
public:
    DriveSetupandMotionState(std::shared_ptr<spdlog::logger>, QState* parent = nullptr);
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
signals:
    void driveSetupAndMotionStateEntered();
    void driveSetupAndMotionStateExited();
private:
    std::shared_ptr<spdlog::logger> m_Logger;

};

#endif // DRIVESETUPANDMOTIONSTATE_H
