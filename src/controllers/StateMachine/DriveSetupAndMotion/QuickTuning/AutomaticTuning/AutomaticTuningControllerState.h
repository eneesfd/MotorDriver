#ifndef AUTOMATICTUNINGCONTROLLERSTATE_H
#define AUTOMATICTUNINGCONTROLLERSTATE_H

#include <QState>

namespace spdlog {
class logger;
}

class AutomaticTuningControllerState : public QState
{
public:
    explicit AutomaticTuningControllerState(std::shared_ptr<spdlog::logger>, QState* parent = nullptr);
signals:
    void automaticTuningControllerStateEntered();
    void automaticTuningControllerStateExited();
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
private:
    std::shared_ptr<spdlog::logger> m_Logger;
};

#endif // AUTOMATICTUNINGCONTROLLERSTATE_H
