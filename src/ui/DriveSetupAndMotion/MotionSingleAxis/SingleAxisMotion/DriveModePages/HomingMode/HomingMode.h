#ifndef HOMINGMODE_H
#define HOMINGMODE_H

#include <QWidget>

namespace Ui {
class HomingMode;
}

class HomingMode : public QWidget
{
    Q_OBJECT

public:
    explicit HomingMode(QWidget *parent = nullptr);
    ~HomingMode();

private:
    Ui::HomingMode *ui;
};

#endif // HOMINGMODE_H
