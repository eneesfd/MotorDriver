#ifndef FEEDBACKSETTINGSCONTROLLER_H
#define FEEDBACKSETTINGSCONTROLLER_H

#include <QObject>

class FeedBackSettingsController : public QObject
{
    Q_OBJECT
public:
    explicit FeedBackSettingsController(QObject *parent = nullptr);

signals:
};

#endif // FEEDBACKSETTINGSCONTROLLER_H
