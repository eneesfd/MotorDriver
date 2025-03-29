#include "Protections.h"
#include "ui_Protections.h"

Protections::Protections(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Protections)
{
    ui->setupUi(this);
}

Protections::~Protections()
{
    delete ui;
}
