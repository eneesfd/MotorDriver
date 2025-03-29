#ifndef CURRENTVERIFICATIONTIME_H
#define CURRENTVERIFICATIONTIME_H

#include <QWidget>

namespace Ui {
class CurrentVerificationTime;
}

class CurrentVerificationTime : public QWidget
{
    Q_OBJECT

public:
    explicit CurrentVerificationTime(QWidget *parent = nullptr);
    ~CurrentVerificationTime();

private:
    Ui::CurrentVerificationTime *ui;
};

#endif // CURRENTVERIFICATIONTIME_H
