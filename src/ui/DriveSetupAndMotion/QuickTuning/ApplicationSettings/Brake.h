#ifndef BRAKE_H
#define BRAKE_H

#include <QWidget>

namespace Ui {
class Brake;
}

class Brake : public QWidget
{
    Q_OBJECT

public:
    explicit Brake(QWidget *parent = nullptr);
    ~Brake();

private:
    Ui::Brake *ui;
    void setLabelValidetor();
};

#endif // BRAKE_H
