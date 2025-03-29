#include "Scheduling.h"
#include "ui_Scheduling.h"

Scheduling::Scheduling(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Scheduling)
{
    ui->setupUi(this);
}

Scheduling::~Scheduling()
{
    delete ui;
}
