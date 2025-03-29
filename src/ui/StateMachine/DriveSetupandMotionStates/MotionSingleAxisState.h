#ifndef MOTIONSINGLEAXISSTATE_H
#define MOTIONSINGLEAXISSTATE_H

#include <QState>

namespace spdlog {
class logger;
}

class MotionSingleAxisState : public QState
{
    Q_OBJECT
public:
    MotionSingleAxisState(std::shared_ptr<spdlog::logger> logger_, QState* parent = nullptr);
private:
    std::shared_ptr<spdlog::logger> m_Logger;
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
signals:
    void motionSingleAxisModeEntered();
    void motionSingleAxisModeExited();
};

#endif // MOTIONSINGLEAXISSTATE_H
