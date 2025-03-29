#ifndef INPUTANDOUTPUTS_H
#define INPUTANDOUTPUTS_H

#include <QWidget>

namespace Ui {
class InputAndOutputs;
}

class InputAndOutputs : public QWidget
{
    Q_OBJECT

public:
    explicit InputAndOutputs(QWidget *parent = nullptr);
    ~InputAndOutputs();

private:
    Ui::InputAndOutputs *ui;
};

#endif // INPUTANDOUTPUTS_H
