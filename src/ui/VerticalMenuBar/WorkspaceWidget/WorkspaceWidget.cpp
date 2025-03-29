#include "WorkspaceWidget.h"
#include "ui_WorkspaceWidget.h"

WorkspaceWidget::WorkspaceWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WorkspaceWidget)
    , contextMenu(new QMenu(this))
    , addDriveMenu(new QMenu(this))
{
    ui->setupUi(this);
    generateWorkspaceActions();
    generateGoldDriveActions();
    ui->treeWidgetWorkspace->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeWidgetWorkspace->setItemDelegate(new WorkspacetWidgetItemDelegate(ui->treeWidgetWorkspace));
    connect(ui->treeWidgetWorkspace, &QTreeWidget::customContextMenuRequested, this, &WorkspaceWidget::showContextMenu);
    connect(ui->treeWidgetWorkspace, &QTreeWidget::itemClicked, this, &WorkspaceWidget::onItemClicked);
}

WorkspaceWidget::~WorkspaceWidget()
{
    delete ui;
}

void WorkspaceWidget::showContextMenu(const QPoint &pos) {
    QTreeWidgetItem *item = ui->treeWidgetWorkspace->itemAt(pos);
    if (!item)
        return;

    bool isChild = (item->parent() != nullptr);

    if (!isChild) {
        // Ana öğe menüsü
        const QAction *selectedAction = contextMenu->exec(mapToGlobal(pos));

        if (selectedAction == openWorkspaceFolderAction) {

        } else if (selectedAction == newWorkspaceAction) {

        } else if (selectedAction == addGoldDriveAction) {
            QTreeWidgetItem *newChild = new QTreeWidgetItem(item);
            newChild->setText(0, "Gold Drive");
            item->setExpanded(true);  // Eklenen child hemen görünsün
            emit addGoldDrive(item->childCount() - 1);
        }
    } else {
        const QAction *selectedAction = addDriveMenu->exec(mapToGlobal(pos));

        if (selectedAction == saveGoldDriveWorkspaceAction) {

        } else if (selectedAction == removeTargetAction) {
            //Get selected child item
            QTreeWidgetItem *parent = item->parent();
            int index = parent->indexOfChild(item);
            emit removeSelectedDrive(index);
            delete item;

        } else if (selectedAction == connectAction) {

        }
        return;
    }
}

void WorkspaceWidget::onItemClicked(QTreeWidgetItem *item, int column)
{
    if (item->parent()) {
        // Child item'a tıklandı
        emit workSpaceChildItemClicked(item);
    } else {
        // Parent item'a tıklandı
        emit workSpaceParentItemClicked(item);
    }
}


void WorkspaceWidget::generateWorkspaceActions()
{
    openWorkspaceFolderAction   = contextMenu->addAction("Open Workspace Folder");
    contextMenu->addSeparator(); // Bölücü Çizgi
    newWorkspaceAction          = contextMenu->addAction("New Workspace");
    openWorkspaceAction         = contextMenu->addAction("Open Workspace");
    saveWorkspaceAction         = contextMenu->addAction("Save Workspace");
    saveAsWorksapceAction       = contextMenu->addAction("Save Workspace as...");
    clearWorkspaceAction        = contextMenu->addAction("Clear Workspace");
    contextMenu->addSeparator(); // Bölücü Çizgi
    addGoldDriveAction          = contextMenu->addAction("Add Gold Drive");
}

void WorkspaceWidget::generateGoldDriveActions()
{
    saveGoldDriveWorkspaceAction    = addDriveMenu->addAction("Save Workspace");
    addDriveMenu->addSeparator(); // Bölücü Çizgi
    removeTargetAction              = addDriveMenu->addAction("Remove Target");
    addDriveMenu->addSeparator(); // Bölücü Çizgi
    connectAction                   = addDriveMenu->addAction("Connect");
}
