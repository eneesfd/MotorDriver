#ifndef FEEDBACKSETTINGS_H
#define FEEDBACKSETTINGS_H

#include <QWidget>

namespace Ui {
class FeedbackSettings;
}

class FeedbackSettings : public QWidget
{
    Q_OBJECT

public:
    explicit FeedbackSettings(QWidget *parent = nullptr);
    ~FeedbackSettings();

private:
    Ui::FeedbackSettings *ui;
};

#endif // FEEDBACKSETTINGS_H
