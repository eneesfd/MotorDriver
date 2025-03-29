#include "SineReferenceMode.h"
#include "ui_SineReferenceMode.h"

SineReferenceMode::SineReferenceMode(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SineReferenceMode)
{
    ui->setupUi(this);
}

SineReferenceMode::~SineReferenceMode()
{
    delete ui;
}
