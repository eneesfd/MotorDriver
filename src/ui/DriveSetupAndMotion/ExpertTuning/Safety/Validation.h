#ifndef VALIDATION_H
#define VALIDATION_H

#include <QWidget>

namespace Ui {
class Validation;
}

class Validation : public QWidget
{
    Q_OBJECT

public:
    explicit Validation(QWidget *parent = nullptr);
    ~Validation();

private:
    Ui::Validation *ui;
};

#endif // VALIDATION_H
