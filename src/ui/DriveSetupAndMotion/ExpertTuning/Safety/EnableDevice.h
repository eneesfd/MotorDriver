#ifndef ENABLEDEVICE_H
#define ENABLEDEVICE_H

#include <QWidget>

namespace Ui {
class EnableDevice;
}

class EnableDevice : public QWidget
{
    Q_OBJECT

public:
    explicit EnableDevice(QWidget *parent = nullptr);
    ~EnableDevice();

private:
    Ui::EnableDevice *ui;
};

#endif // ENABLEDEVICE_H
