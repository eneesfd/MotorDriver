#ifndef COMUTATION_H
#define COMUTATION_H

#include <QWidget>

namespace Ui {
class Comutation;
}

class Comutation : public QWidget
{
    Q_OBJECT

public:
    explicit Comutation(QWidget *parent = nullptr);
    ~Comutation();

private:
    Ui::Comutation *ui;
};

#endif // COMUTATION_H
