#include "InputAndOutputs.h"
#include "ui_InputAndOutputs.h"

InputAndOutputs::InputAndOutputs(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InputAndOutputs)
{
    ui->setupUi(this);
}

InputAndOutputs::~InputAndOutputs()
{
    delete ui;
}
