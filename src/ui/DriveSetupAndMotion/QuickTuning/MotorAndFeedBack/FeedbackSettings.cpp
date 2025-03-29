#include "FeedbackSettings.h"
#include "ui_FeedbackSettings.h"

FeedbackSettings::FeedbackSettings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FeedbackSettings)
{
    ui->setupUi(this);
}

FeedbackSettings::~FeedbackSettings()
{
    delete ui;
}
