#ifndef VELOCITYMODE_H
#define VELOCITYMODE_H

#include <QWidget>

namespace Ui {
class VelocityMode;
}

class VelocityMode : public QWidget
{
    Q_OBJECT

public:
    explicit VelocityMode(QWidget *parent = nullptr);
    ~VelocityMode();

private:
    Ui::VelocityMode *ui;
};

#endif // VELOCITYMODE_H
