#ifndef STEPPERMODE_H
#define STEPPERMODE_H

#include <QWidget>

namespace Ui {
class StepperMode;
}

class StepperMode : public QWidget
{
    Q_OBJECT

public:
    explicit StepperMode(QWidget *parent = nullptr);
    ~StepperMode();

private:
    Ui::StepperMode *ui;
};

#endif // STEPPERMODE_H
