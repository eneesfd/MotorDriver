#ifndef MOTIONSINGLEAXIS_H
#define MOTIONSINGLEAXIS_H

#include <QWidget>


class UIStateMachineController;

namespace spdlog { class logger; }
namespace Ui {
class MotionSingleAxis;
}

class MotionSingleAxis : public QWidget
{
    Q_OBJECT

public:
    explicit MotionSingleAxis(QWidget *parent = nullptr);
    ~MotionSingleAxis();
    void setUIStateMachine(QSharedPointer<UIStateMachineController> stateMachine);
    void setLogManager(std::shared_ptr<spdlog::logger> log_);

private:
    Ui::MotionSingleAxis *ui;
    std::shared_ptr<spdlog::logger> m_logManager;
};

#endif // MOTIONSINGLEAXIS_H
