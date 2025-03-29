#ifndef AUTOMATICTUNING_H
#define AUTOMATICTUNING_H

#include <QWidget>

namespace Ui {
class AutomaticTuning;
}

class AutomaticTuning : public QWidget
{
    Q_OBJECT

public:
    explicit AutomaticTuning(QWidget *parent = nullptr);
    ~AutomaticTuning();

private:
    Ui::AutomaticTuning *ui;
};

#endif // AUTOMATICTUNING_H
