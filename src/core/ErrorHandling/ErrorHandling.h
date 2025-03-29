#ifndef ERRORHANDLING_H
#define ERRORHANDLING_H

#include <QDialog>

namespace Ui {
class ErrorHandling;
}

class ErrorHandling : public QDialog
{
    Q_OBJECT

public:
    explicit ErrorHandling(QWidget *parent = nullptr);
    ~ErrorHandling();
    void addError(const QString &key, const QString &value);
    void showErrors();
    void removeError(const QString &key);
    void clearErrors();
private:
    Ui::ErrorHandling *ui;
    QHash<QString,QString> errors;
};

#endif // ERRORHANDLING_H
