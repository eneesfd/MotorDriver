#ifndef SYSTEMCONFIGURATIONCONTROLLERSTATE_H
#define SYSTEMCONFIGURATIONCONTROLLERSTATE_H

#include <QState>

namespace spdlog {
class logger;
}

class SystemConfigurationControllerState : public QState
{
public:
    SystemConfigurationControllerState(std::shared_ptr<spdlog::logger> logger, QState *parent = nullptr);
signals:
    void systemConfigurationControllerEntered();
    void systemConfigurationControllerExited();
protected:
    void onEntry(QEvent *) override;
    void onExit(QEvent *) override;
private:
    std::shared_ptr<spdlog::logger> m_Logger;
};

#endif // SYSTEMCONFIGURATIONCONTROLLERSTATE_H
