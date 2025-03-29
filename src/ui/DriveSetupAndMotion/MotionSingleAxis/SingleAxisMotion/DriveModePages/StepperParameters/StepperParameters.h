#ifndef STEPPERPARAMETERS_H
#define STEPPERPARAMETERS_H

#include <QWidget>

namespace Ui {
class StepperParameters;
}

class StepperParameters : public QWidget
{
    Q_OBJECT

public:
    explicit StepperParameters(QWidget *parent = nullptr);
    ~StepperParameters();

private:
    Ui::StepperParameters *ui;
};

#endif // STEPPERPARAMETERS_H
