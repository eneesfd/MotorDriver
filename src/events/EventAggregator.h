#ifndef EVENTAGGREGATOR_H
#define EVENTAGGREGATOR_H

#include <QObject>

class EventAggregator : public QObject {
    Q_OBJECT
public:
    static EventAggregator& instance() {
        static EventAggregator instance;
        return instance;
    }

signals:
    void changeApplicationTheme(const QString& theme);
private:
    EventAggregator() {};
    EventAggregator(const EventAggregator&) = delete;
    EventAggregator& operator=(const EventAggregator&) = delete;
};

#endif // EVENTAGGREGATOR_H
