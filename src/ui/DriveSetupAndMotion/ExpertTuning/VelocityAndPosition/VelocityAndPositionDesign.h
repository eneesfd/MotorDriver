#ifndef VELOCITYANDPOSITIONDESIGN_H
#define VELOCITYANDPOSITIONDESIGN_H

#include <QWidget>

namespace Ui {
class VelocityAndPositionDesign;
}

class VelocityAndPositionDesign : public QWidget
{
    Q_OBJECT

public:
    explicit VelocityAndPositionDesign(QWidget *parent = nullptr);
    ~VelocityAndPositionDesign();

private:
    Ui::VelocityAndPositionDesign *ui;
};

#endif // VELOCITYANDPOSITIONDESIGN_H
