#ifndef STEPPERCLOSEDLOOP_H
#define STEPPERCLOSEDLOOP_H

#include <QWidget>

namespace Ui {
class StepperClosedLoop;
}

class StepperClosedLoop : public QWidget
{
    Q_OBJECT

public:
    explicit StepperClosedLoop(QWidget *parent = nullptr);
    ~StepperClosedLoop();

private:
    Ui::StepperClosedLoop *ui;
};

#endif // STEPPERCLOSEDLOOP_H
