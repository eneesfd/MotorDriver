#ifndef POSITIONMODE_H
#define POSITIONMODE_H

#include <QWidget>

namespace Ui {
class PositionMode;
}

class PositionMode : public QWidget
{
    Q_OBJECT

public:
    explicit PositionMode(QWidget *parent = nullptr);
    ~PositionMode();

private:
    Ui::PositionMode *ui;
};

#endif // POSITIONMODE_H
