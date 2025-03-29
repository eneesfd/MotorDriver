#ifndef CURRENTIDENTIFICATION_H
#define CURRENTIDENTIFICATION_H

#include <QWidget>

namespace Ui {
class CurrentIdentification;
}
class CurrentIdentificationController;

class CurrentIdentification : public QWidget
{
    Q_OBJECT

public:
    explicit CurrentIdentification(QWidget *parent = nullptr);
    ~CurrentIdentification();
    void setControllers(QSharedPointer<CurrentIdentificationController> controller_);
private slots:
    void on_pbIdentify_clicked();
signals:
    void sendIdentifyMessage();
    private:
              Ui::CurrentIdentification *ui;
    QSharedPointer<CurrentIdentificationController> m_controller;
};

#endif // CURRENTIDENTIFICATION_H
