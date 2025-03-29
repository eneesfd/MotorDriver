#ifndef WINDOWSLISTWIDGET_H
#define WINDOWSLISTWIDGET_H

#include "Constants.h"
#include <QTreeWidgetItem>
#include <QWidget>

class UIStateMachineController;

namespace Ui {
class WindowsListWidget;
}

class WindowsListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WindowsListWidget(QWidget *parent = nullptr);
    ~WindowsListWidget();
    QTreeWidget* getTreeWidget() const;
    void setUIStateMachine(QSharedPointer<UIStateMachineController> stateMachine);

signals:
    void currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
private slots:
    void on_treewgtWindowsList_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::WindowsListWidget *ui;
    QHash<QTreeWidgetItem*, Constants::VerticalMenuBarConstants::WindowsListPages> m_windowsListIndex;
    void createItemToPageMap(Ui::WindowsListWidget *ui);
    void applyPageIndices(QHash<QTreeWidgetItem*, Constants::VerticalMenuBarConstants::WindowsListPages>& itemToPageMap);
    void selectTreeWidgetItemForPage(Constants::VerticalMenuBarConstants::WindowsListPages page);
    QSharedPointer<UIStateMachineController> m_StateMachine;
};

#endif // WINDOWSLISTWIDGET_H
