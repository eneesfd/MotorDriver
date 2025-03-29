#ifndef STOPPINGOPTIONS_H
#define STOPPINGOPTIONS_H

#include <QWidget>

namespace Ui {
class StoppingOptions;
}

class StoppingOptions : public QWidget
{
    Q_OBJECT

public:
    explicit StoppingOptions(QWidget *parent = nullptr);
    ~StoppingOptions();

private:
    Ui::StoppingOptions *ui;
};

#endif // STOPPINGOPTIONS_H
