#ifndef QUICKTUNINGSTATE_H
#define QUICKTUNINGSTATE_H

#include <QObject>
#include <QState>

namespace spdlog {
class logger;
}

class QuickTuningState : public QState
{
    Q_OBJECT
public:
    QuickTuningState(std::shared_ptr<spdlog::logger> logger_, QState* parent = nullptr);
private:
    std::shared_ptr<spdlog::logger> m_Logger;
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
signals:
    void quickTuningModeEntered();
    void quickTuningModeExited();
};

#endif // QUICKTUNINGSTATE_H
