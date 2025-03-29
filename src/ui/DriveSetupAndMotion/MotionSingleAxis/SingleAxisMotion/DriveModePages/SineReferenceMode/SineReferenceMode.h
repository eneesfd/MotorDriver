#ifndef SINEREFERENCEMODE_H
#define SINEREFERENCEMODE_H

#include <QWidget>

namespace Ui {
class SineReferenceMode;
}

class SineReferenceMode : public QWidget
{
    Q_OBJECT

public:
    explicit SineReferenceMode(QWidget *parent = nullptr);
    ~SineReferenceMode();

private:
    Ui::SineReferenceMode *ui;
};

#endif // SINEREFERENCEMODE_H
