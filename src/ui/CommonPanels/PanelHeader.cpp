#include "PanelHeader.h"
#include "ui_PanelHeader.h"
#include "Constants.h"

using namespace Constants::PanelHeader;

PanelHeader::PanelHeader(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PanelHeader)
{
    ui->setupUi(this);
}

PanelHeader::~PanelHeader()
{
    delete ui;
}

QLabel *PanelHeader::getWidgetTitleLabel() const
{
    return ui->lblWindowName;
}

QLabel *PanelHeader::getPageTitleLabel() const
{
    return ui->lblPageName;
}

