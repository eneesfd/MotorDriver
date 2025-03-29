#ifndef DRIVEUSERUNITS_H
#define DRIVEUSERUNITS_H

#include <QWidget>

namespace Ui {
class DriveUserUnits;
}

class DriveUserUnits : public QWidget
{
    Q_OBJECT

public:
    explicit DriveUserUnits(QWidget *parent = nullptr);
    ~DriveUserUnits();

private:
    Ui::DriveUserUnits *ui;
};

#endif // DRIVEUSERUNITS_H
