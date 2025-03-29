#include "AutomaticTuning.h"
#include "ui_AutomaticTuning.h"

AutomaticTuning::AutomaticTuning(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AutomaticTuning)
{
    ui->setupUi(this);
}

AutomaticTuning::~AutomaticTuning()
{
    delete ui;
}
