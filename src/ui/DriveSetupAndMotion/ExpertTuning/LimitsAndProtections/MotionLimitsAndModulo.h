#ifndef MOTIONLIMITSANDMODULO_H
#define MOTIONLIMITSANDMODULO_H

#include <QWidget>

namespace Ui {
class MotionLimitsAndModulo;
}

class MotionLimitsAndModulo : public QWidget
{
    Q_OBJECT

public:
    explicit MotionLimitsAndModulo(QWidget *parent = nullptr);
    ~MotionLimitsAndModulo();

private:
    Ui::MotionLimitsAndModulo *ui;
};

#endif // MOTIONLIMITSANDMODULO_H
