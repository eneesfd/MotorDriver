#ifndef ADVANCEDFILTERSTACKWIDGETCOMPONENT_H
#define ADVANCEDFILTERSTACKWIDGETCOMPONENT_H

#include <QWidget>

namespace Ui {
class AdvancedFilterStackwidgetComponent;
}

class AdvancedFilterStackwidgetComponent : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedFilterStackwidgetComponent(QWidget *parent = nullptr);
    ~AdvancedFilterStackwidgetComponent();
    Ui::AdvancedFilterStackwidgetComponent *getUi() const;

private:
    Ui::AdvancedFilterStackwidgetComponent *ui;
    void createSignalsAndSlots();
};

#endif // ADVANCEDFILTERSTACKWIDGETCOMPONENT_H
