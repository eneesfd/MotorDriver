#ifndef ACCEPTANCE_H
#define ACCEPTANCE_H

#include <QWidget>

namespace Ui {
class Acceptance;
}

class Acceptance : public QWidget
{
    Q_OBJECT

public:
    explicit Acceptance(QWidget *parent = nullptr);
    ~Acceptance();

private:
    Ui::Acceptance *ui;
};

#endif // ACCEPTANCE_H
