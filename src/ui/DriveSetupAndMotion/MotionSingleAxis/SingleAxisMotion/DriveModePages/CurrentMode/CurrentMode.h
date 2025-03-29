#ifndef CURRENTMODE_H
#define CURRENTMODE_H

#include <QWidget>

namespace Ui {
class CurrentMode;
}

class CurrentMode : public QWidget
{
    Q_OBJECT

public:
    explicit CurrentMode(QWidget *parent = nullptr);
    ~CurrentMode();

private:
    Ui::CurrentMode *ui;
};

#endif // CURRENTMODE_H
