#ifndef SYSTEMCONFIGURATIONSTATE_H
#define SYSTEMCONFIGURATIONSTATE_H

#include <QObject>
#include <QState>

namespace spdlog {
class logger;
}

class SystemConfigurationState : public QState
{
    Q_OBJECT
public:
    explicit SystemConfigurationState(std::shared_ptr<spdlog::logger> logger_, QState* parent = nullptr);
protected:
    void onEntry(QEvent *) override;
    void onExit(QEvent *) override;
private:
    std::shared_ptr<spdlog::logger> m_Logger;
signals:
    void systemConfigurationStateEntered();
    void systemConfigurationStateExited();
};

#endif // SYSTEMCONFIGURATIONSTATE_H
