#ifndef EXPERTTUNINGSTATE_H
#define EXPERTTUNINGSTATE_H

#include <QObject>
#include <QState>

namespace spdlog {
class logger;
}

class ExpertTuningState : public QState
{
    Q_OBJECT
public:
    ExpertTuningState(std::shared_ptr<spdlog::logger> logger_, QState* parent = nullptr);
private:
    std::shared_ptr<spdlog::logger> m_Logger;
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
signals:
    void expertTuningModeEntered();
    void expertTuningModeExited();
};

#endif // EXPERTTUNINGSTATE_H
