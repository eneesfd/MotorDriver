#ifndef FEEDBACKSETTINGSCONTROLLERSTATE_H
#define FEEDBACKSETTINGSCONTROLLERSTATE_H

#include <QState>

namespace spdlog {
class logger;
}

class FeedBackSettingsControllerState : public QState
{
public:
    explicit FeedBackSettingsControllerState(std::shared_ptr<spdlog::logger>, QState* parent = nullptr);
signals:
    void feedBackSettingsControllerStateEntered();
    void feedBackSettingsControllerStateExited();
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
private:
    std::shared_ptr<spdlog::logger> m_Logger;
};

#endif // FEEDBACKSETTINGSCONTROLLERSTATE_H
