#ifndef CURRENTDESIGN_H
#define CURRENTDESIGN_H

#include <QWidget>

namespace Ui {
class CurrentDesign;
}

class CurrentDesign : public QWidget
{
    Q_OBJECT

public:
    explicit CurrentDesign(QWidget *parent = nullptr);
    ~CurrentDesign();

private:
    Ui::CurrentDesign *ui;
};

#endif // CURRENTDESIGN_H
