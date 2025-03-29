#ifndef FUNCTIONALSAFETY_H
#define FUNCTIONALSAFETY_H

#include <QWidget>

namespace Ui {
class FunctionalSafety;
}

class FunctionalSafety : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionalSafety(QWidget *parent = nullptr);
    ~FunctionalSafety();

private:
    Ui::FunctionalSafety *ui;
};

#endif // FUNCTIONALSAFETY_H
