#ifndef AXISCONFIGURATIONSCONTROLLERSTATE_H
#define AXISCONFIGURATIONSCONTROLLERSTATE_H

#include <QState>

namespace spdlog {
class logger;
}

class AxisConfigurationsControllerState : public QState
{
public:
    explicit AxisConfigurationsControllerState(std::shared_ptr<spdlog::logger>, QState* parent = nullptr);
signals:
    void axisConfigurationsControllerStateEntered();
    void axisConfigurationsControllerStateExited();
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
private:
    std::shared_ptr<spdlog::logger> m_Logger;
};

#endif // AXISCONFIGURATIONSCONTROLLERSTATE_H
