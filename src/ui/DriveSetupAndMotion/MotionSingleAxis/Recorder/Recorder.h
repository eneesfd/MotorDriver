#ifndef RECORDER_H
#define RECORDER_H

#include <QWidget>

namespace Ui {
class Recorder;
}

class Recorder : public QWidget
{
    Q_OBJECT

public:
    explicit Recorder(QWidget *parent = nullptr);
    ~Recorder();

private:
    Ui::Recorder *ui;
};

#endif // RECORDER_H
