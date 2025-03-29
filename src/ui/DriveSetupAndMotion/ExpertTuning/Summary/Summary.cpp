#include "Summary.h"
#include "ui_Summary.h"

Summary::Summary(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Summary)
{
    ui->setupUi(this);
}

Summary::~Summary()
{
    delete ui;
}
