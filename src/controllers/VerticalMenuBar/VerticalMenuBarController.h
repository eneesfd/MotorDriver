#ifndef VERTICALMENUBARCONTROLLER_H
#define VERTICALMENUBARCONTROLLER_H

#include <QObject>
#include <QPointer>
#include <spdlog/logger.h>
#include "VerticalMenuBar.h"

class LogManager;
class VerticalMenuBar;

class VerticalMenuBarController : public QObject
{
    Q_OBJECT
public:
    explicit VerticalMenuBarController(std::shared_ptr<spdlog::logger> logger_, VerticalMenuBar*, QObject *parent = nullptr);
signals:
    void addGoldDrive(int index);
    void removeSelectedDrive(int index);
    void parentItemClicked(QTreeWidgetItem *item);
    void childItemClicked(QTreeWidgetItem *item);
private:
    std::shared_ptr<spdlog::logger> m_Logger;
    void setupConnections();
    QPointer<VerticalMenuBar> m_VerticalMenuBar;
};

#endif // VERTICALMENUBARCONTROLLER_H
