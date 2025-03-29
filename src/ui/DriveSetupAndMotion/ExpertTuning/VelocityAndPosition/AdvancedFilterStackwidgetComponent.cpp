#include "AdvancedFilterStackwidgetComponent.h"
#include "ui_AdvancedFilterStackwidgetComponent.h"

AdvancedFilterStackwidgetComponent::AdvancedFilterStackwidgetComponent(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdvancedFilterStackwidgetComponent)
{
    ui->setupUi(this);
    createSignalsAndSlots();
}

AdvancedFilterStackwidgetComponent::~AdvancedFilterStackwidgetComponent()
{
    delete ui;
}

Ui::AdvancedFilterStackwidgetComponent *AdvancedFilterStackwidgetComponent::getUi() const
{
    return ui;
}

void AdvancedFilterStackwidgetComponent::createSignalsAndSlots()
{
    // connect(ui->cmbFilterType, &QComboBox::currentIndexChanged, ui->stckwgt, &QStackedWidget::setCurrentIndex);
    connect(ui->cmbFilterType, &QComboBox::currentIndexChanged, [&](int index){
        ui->stckwgt->setCurrentIndex(index);
        ui->stckwgt->resize(ui->stckwgt->widget(index)->sizeHint());

        // switch (index) {
        // case 0:
        //     ui->stckwgt->setFixedHeight(0);
        //     break;
        // case 2:
        //     ui->stckwgt->setFixedHeight(50);
        // case 3:
        //     ui->stckwgt->setFixedHeight(50);
        // case 4:
        //     ui->stckwgt->setFixedHeight(50);
        // case 5:
        //     ui->stckwgt->setFixedHeight(50);
        // default:
        //     break;
        // }
    });
}
