#include "GantryYawDesign.h"
#include "ui_GantryYawDesign.h"

GantryYawDesign::GantryYawDesign(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GantryYawDesign)
{
    ui->setupUi(this);
}

GantryYawDesign::~GantryYawDesign()
{
    delete ui;
}
