#ifndef PROTECTIONS_H
#define PROTECTIONS_H

#include <QWidget>

namespace Ui {
class Protections;
}

class Protections : public QWidget
{
    Q_OBJECT

public:
    explicit Protections(QWidget *parent = nullptr);
    ~Protections();

private:
    Ui::Protections *ui;
};

#endif // PROTECTIONS_H
