#include "CurrentIdentification.h"
#include "ui_CurrentIdentification.h"
#include "CurrentIdentificationController.h"

CurrentIdentification::CurrentIdentification(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CurrentIdentification)
{
    ui->setupUi(this);
}

CurrentIdentification::~CurrentIdentification()
{
    delete ui;
}

void CurrentIdentification::setControllers(QSharedPointer<CurrentIdentificationController> controller_)
{
    m_controller = controller_;

    QObject::connect(this, &CurrentIdentification::sendIdentifyMessage, m_controller.get(), &CurrentIdentificationController::identify);
}

void CurrentIdentification::on_pbIdentify_clicked()
{
    emit sendIdentifyMessage();
}

