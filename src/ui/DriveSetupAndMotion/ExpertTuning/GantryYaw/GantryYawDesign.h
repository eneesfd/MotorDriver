#ifndef GANTRYYAWDESIGN_H
#define GANTRYYAWDESIGN_H

#include <QWidget>

namespace Ui {
class GantryYawDesign;
}

class GantryYawDesign : public QWidget
{
    Q_OBJECT

public:
    explicit GantryYawDesign(QWidget *parent = nullptr);
    ~GantryYawDesign();

private:
    Ui::GantryYawDesign *ui;
};

#endif // GANTRYYAWDESIGN_H
