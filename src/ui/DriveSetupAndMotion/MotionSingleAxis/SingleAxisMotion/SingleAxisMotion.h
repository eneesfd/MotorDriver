#ifndef SINGLEAXISMOTION_H
#define SINGLEAXISMOTION_H

#include <QWidget>

class UIStateMachineController;
namespace spdlog { class logger; }

namespace Ui {
class SingleAxisMotion;
}

class SingleAxisMotion : public QWidget
{
    Q_OBJECT

public:
    explicit SingleAxisMotion(QWidget *parent = nullptr);
    ~SingleAxisMotion();
    void setUIStateMachine(QSharedPointer<UIStateMachineController> stateMachine);
    void setLogManager(std::shared_ptr<spdlog::logger> log_);
private:
    Ui::SingleAxisMotion *ui;
    std::shared_ptr<spdlog::logger> m_logManager;
};

#endif // SINGLEAXISMOTION_H
