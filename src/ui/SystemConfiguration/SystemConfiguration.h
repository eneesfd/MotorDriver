#ifndef SYSTEMCONFIGURATION_H
#define SYSTEMCONFIGURATION_H

#include <QWidget>
#include <QTreeWidgetItem>

namespace Ui {
class SystemConfiguration;
}

class SystemConfiguration : public QWidget
{
    Q_OBJECT

public:
    explicit SystemConfiguration(QWidget *parent = nullptr);
    ~SystemConfiguration();

private:
    Ui::SystemConfiguration *ui;
public slots:
    void onWorkSpaceParentItemSelected(QTreeWidgetItem *item);
    void onWorkSpaceChildItemSelected(QTreeWidgetItem *item);
};

#endif // SYSTEMCONFIGURATION_H
