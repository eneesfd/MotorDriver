#ifndef VELOCITYANDPOSITIONIDENTIFICATION_H
#define VELOCITYANDPOSITIONIDENTIFICATION_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class VelocityAndPositionIdentification;
}

class VelocityAndPositionIdentification : public QWidget
{
    Q_OBJECT

public:
    explicit VelocityAndPositionIdentification(QWidget *parent = nullptr);
    ~VelocityAndPositionIdentification();

private:
    Ui::VelocityAndPositionIdentification *ui;
    QPushButton *btnShowHideTabs;
    void initializeFormSettings();
    void createSignalsAndSlots();
};

#endif // VELOCITYANDPOSITIONIDENTIFICATION_H
