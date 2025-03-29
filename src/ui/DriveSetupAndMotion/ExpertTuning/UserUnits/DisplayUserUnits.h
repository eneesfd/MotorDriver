#ifndef DISPLAYUSERUNITS_H
#define DISPLAYUSERUNITS_H

#include <QWidget>

namespace Ui {
class DisplayUserUnits;
}

class DisplayUserUnits : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayUserUnits(QWidget *parent = nullptr);
    ~DisplayUserUnits();

private:
    Ui::DisplayUserUnits *ui;
};

#endif // DISPLAYUSERUNITS_H
