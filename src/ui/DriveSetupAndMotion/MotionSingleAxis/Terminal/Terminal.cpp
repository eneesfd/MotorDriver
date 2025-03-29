#include "Terminal.h"
#include "ui_Terminal.h"

Terminal::Terminal(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Terminal)
{
    ui->setupUi(this);
}

Terminal::~Terminal()
{
    delete ui;
}
