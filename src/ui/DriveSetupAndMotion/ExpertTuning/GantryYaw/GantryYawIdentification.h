#ifndef GANTRYYAWIDENTIFICATION_H
#define GANTRYYAWIDENTIFICATION_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class GantryYawIdentification;
}

class GantryYawIdentification : public QWidget
{
    Q_OBJECT

public:
    explicit GantryYawIdentification(QWidget *parent = nullptr);
    ~GantryYawIdentification();

private:
    Ui::GantryYawIdentification *ui;
    QPushButton *btnShowHideTabs;
    void initializeFormSettings();
    void createSignalsAndSlots();
};

#endif // GANTRYYAWIDENTIFICATION_H
