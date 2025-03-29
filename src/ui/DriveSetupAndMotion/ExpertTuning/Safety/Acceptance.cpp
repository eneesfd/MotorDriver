#include "Acceptance.h"
#include "ui_Acceptance.h"

Acceptance::Acceptance(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Acceptance)
{
    ui->setupUi(this);
}

Acceptance::~Acceptance()
{
    delete ui;
}
