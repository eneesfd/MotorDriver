#ifndef VELOCITYANDPOSITIONVERIFICATIONTIME_H
#define VELOCITYANDPOSITIONVERIFICATIONTIME_H

#include <QWidget>

namespace Ui {
class VelocityAndPositionVerificationTime;
}

class VelocityAndPositionVerificationTime : public QWidget
{
    Q_OBJECT

public:
    explicit VelocityAndPositionVerificationTime(QWidget *parent = nullptr);
    ~VelocityAndPositionVerificationTime();

private:
    Ui::VelocityAndPositionVerificationTime *ui;
};

#endif // VELOCITYANDPOSITIONVERIFICATIONTIME_H
