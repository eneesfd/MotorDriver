#ifndef WORKSPACEWIDGET_H
#define WORKSPACEWIDGET_H

#include <QWidget>
#include <QMenu>
#include <QTreeWidgetItem>
#include <QPainter>

namespace Ui {
class WorkspaceWidget;
}

class WorkspaceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WorkspaceWidget(QWidget *parent = nullptr);
    ~WorkspaceWidget();

    void showContextMenu(const QPoint &pos);
    void generateWorkspaceActions();
    void generateGoldDriveActions();
    void onItemClicked(QTreeWidgetItem *item, int column);
signals:
    void openWorkspaceFolder();
    void createNewWorkspace();
    void openWorkspace();
    void saveWorkspaceAs();
    void clearWorkspace();
    void addGoldDrive(int index);
    void removeSelectedDrive(int index);
    void workSpaceParentItemClicked(QTreeWidgetItem *item);
    void workSpaceChildItemClicked(QTreeWidgetItem *item);
private:
    Ui::WorkspaceWidget *ui;
    QMenu *contextMenu;
    QMenu *addDriveMenu;

    QAction *openWorkspaceFolderAction;
    QAction *newWorkspaceAction;
    QAction *openWorkspaceAction;
    QAction *saveWorkspaceAction;
    QAction *saveAsWorksapceAction;
    QAction *clearWorkspaceAction;
    QAction *addGoldDriveAction;

    QAction *saveGoldDriveWorkspaceAction;
    QAction *removeTargetAction;
    QAction *connectAction;
};

#include <QStyledItemDelegate>

class WorkspacetWidgetItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    WorkspacetWidgetItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {};
    using QStyledItemDelegate::QStyledItemDelegate;

protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override {
        QStyledItemDelegate::initStyleOption(option, index);

        // Parent öğelerin ikonlarını sağda, child öğelerin ikonlarını solda hizala
        if (!index.parent().isValid()) {
            option->decorationPosition = QStyleOptionViewItem::Right; // Parent: Sağda
        } else {
            option->decorationPosition = QStyleOptionViewItem::Left; // Child: Solda
        }

        // Parent ve child öğeler için font ayarları
        QFont font = option->font;
        font.setFamily("Helvatica");
        font.setStyleStrategy(QFont::PreferQuality); // Daha kaliteli render için
        font.setStyleHint(QFont::Helvetica);
        font.setLetterSpacing(QFont::AbsoluteSpacing, -0.41); // 2 piksel aralık
        if (!index.parent().isValid()) {
            // Parent öğeler için: 13px
            font.setPixelSize(13);
            font.setWeight(QFont::Medium);
        } else {
            // Child öğeler için: 11px
            font.setPixelSize(11);
            font.setWeight(QFont::Medium);
        }
        option->font = font;
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        // 1. Durum bilgilerini hesapla
        QColor backgroundColor;
        QColor textColor;

        if (option.state & QStyle::State_Selected) {
            backgroundColor = QColor("#E6E6E6");
            textColor       = QColor("#000000");
        } else {
            backgroundColor = QColor("#555555");
            textColor       = QColor("#FFFFFF");
        }

        // 2. Varsayılan çizim ayarlarını hazırla
        QStyleOptionViewItem opt(option);
        initStyleOption(&opt, index);
        opt.palette.setColor(QPalette::Text, textColor);
        // Metin için sağ ve soldan padding veriyoruz
        opt.rect.adjust(16, 0, -16, 0);

        // 3. Arka plan ve alt çizgiyi manuel çiz
        painter->save();
        painter->fillRect(option.rect, backgroundColor);
        painter->restore();

        // 4. Varsayılan item çizimini uygula (metin, simgeler vs.)
        QStyledItemDelegate::paint(painter, opt, index);
    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        // Parent öğeler için 40px, child öğeler için 30px yükseklik belirle
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        if (!index.parent().isValid()) {
            size.setHeight(48); // Parent yüksekliği
        } else {
            size.setHeight(30); // Child yüksekliği
        }
        return size;
    }
};

#endif // WORKSPACEWIDGET_H
