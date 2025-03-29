#ifndef VERTICALMENUBAR_H
#define VERTICALMENUBAR_H

#include <QTreeWidgetItem>
#include <QWidget>

class UIStateMachineController;

namespace Ui {
class VerticalMenuBar;
}

class WindowsListWidget;
class MainMenuListWidget;
class WorkspaceWidget;

class VerticalMenuBar : public QWidget
{
    Q_OBJECT

public:
    explicit VerticalMenuBar(QWidget *parent = nullptr);
    ~VerticalMenuBar();
    WindowsListWidget* getWindowsListWidget() const;
    MainMenuListWidget * getMainMenuListWidget() const;
    void setUIStateMachine(QSharedPointer<UIStateMachineController> stateMachine);
    WorkspaceWidget *getWorkspaceWidget() const;
    void setupConnections();
signals:
    void workSpaceParentItemClicked(QTreeWidgetItem *item);
    void workSpaceChildItemClicked(QTreeWidgetItem *item);
private:
    Ui::VerticalMenuBar *ui;
    void applyLastOpenedWindowConfiguration();
};

#endif // VERTICALMENUBAR_H
