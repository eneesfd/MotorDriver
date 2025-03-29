#ifndef CURRENTLIMITSSTATE_H
#define CURRENTLIMITSSTATE_H

#include <QState>

namespace spdlog {
class logger;
}

class CurrentLimitsState : public QState
{
    Q_OBJECT
public:
    explicit CurrentLimitsState(std::shared_ptr<spdlog::logger> logger_ , QState * parent = nullptr);
signals:
    void currentLimitsStateEntered();
    void currentLimitsStateExited();
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
private:
    std::shared_ptr<spdlog::logger> m_logger;
};

#endif // CURRENTLIMITSSTATE_H
