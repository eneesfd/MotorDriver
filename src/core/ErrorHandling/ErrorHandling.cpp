#include "ErrorHandling.h"
#include "ui_ErrorHandling.h"

ErrorHandling::ErrorHandling(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ErrorHandling)
    , errors{}
{
    ui->setupUi(this);
    this->setModal(true);
}

ErrorHandling::~ErrorHandling()
{
    delete ui;
}

void ErrorHandling::addError(const QString &key, const QString &value)
{
    errors.insert(key, value);
}

void ErrorHandling::showErrors()
{
    ui->listWidget->clear();
    for (auto it = errors.begin(); it != errors.end(); ++it)
    {
        ui->listWidget->addItem(it.key() + ": " + it.value());
    }
    show();
}

void ErrorHandling::removeError(const QString &key)
{
    errors.remove(key);
}

void ErrorHandling::clearErrors()
{
    errors.clear();
}
