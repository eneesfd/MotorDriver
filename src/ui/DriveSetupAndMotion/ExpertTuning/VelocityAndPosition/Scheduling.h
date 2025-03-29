#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <QWidget>

namespace Ui {
class Scheduling;
}

class Scheduling : public QWidget
{
    Q_OBJECT

public:
    explicit Scheduling(QWidget *parent = nullptr);
    ~Scheduling();

private:
    Ui::Scheduling *ui;
};

#endif // SCHEDULING_H
