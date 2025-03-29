#ifndef MAINMENULISTWIDGET_H
#define MAINMENULISTWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <qpainter.h>
#include "Constants.h"

class UIStateMachineController;

namespace Ui {
class MainMenuListWidget;
}

class MainMenuListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenuListWidget(QWidget *parent = nullptr);
    ~MainMenuListWidget();
    Constants::VerticalMenuBarConstants::MainMenuPages getMainMenuIndex() const;
    void setUIStateMachine(QSharedPointer<UIStateMachineController> stateMachine);

signals:
    void currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
private slots:
    void on_treewgtMainMenuList_itemClicked(QTreeWidgetItem *item, int column);
private:
    Ui::MainMenuListWidget *ui;
    QHash<QTreeWidgetItem*, Constants::VerticalMenuBarConstants::MainMenuPages> m_mainMenuIndex;
    void createItemToPageMap(Ui::MainMenuListWidget *ui);
    void applyPageIndices(QHash<QTreeWidgetItem*, Constants::VerticalMenuBarConstants::MainMenuPages>& itemToPageMap);
    void selectTreeWidgetItemForPage(Constants::VerticalMenuBarConstants::MainMenuPages page);
    void setupConnections();
    QSharedPointer<UIStateMachineController> m_StateMachine;
};


#include <QStyledItemDelegate>

class MainMenuListWidgetItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    MainMenuListWidgetItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {};
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
        if (!index.parent().isValid()) {
            // Parent öğeler için: 14px, bold
            font.setPixelSize(14);
            font.setWeight(QFont::Bold);
        } else {
            // Child öğeler için: 13px, medium
            font.setPixelSize(13);
            font.setWeight(QFont::Medium);
        }
        option->font = font;
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        // 1. Durum bilgilerini hesapla
        bool isParent = !index.parent().isValid();
        QColor backgroundColor;
        QColor textColor;

        if (option.state & QStyle::State_Selected) {
            if (isParent) {
                backgroundColor = QColor("#1A1A1A");
                textColor       = QColor("#FDD700");
            } else {
                backgroundColor = QColor("#E6E6E6");
                textColor       = QColor("#000000");
            }
        } else {
            if (isParent) {
                backgroundColor = QColor("#1A1A1A");
                textColor       = QColor("#FFFFFF");
            } else {
                backgroundColor = QColor("#555555");
                textColor       = QColor("#FFFFFF");
            }
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

        // Son öğe kontrolü
        bool isLastItem = false;
        if (index.parent().isValid()) {
            isLastItem = (index.row() == index.model()->rowCount(index.parent()) - 1);
        } else {
            isLastItem = (index.row() == index.model()->rowCount(QModelIndex()) - 1);
        }
        if (!isLastItem) {
            painter->setPen(QPen(QColor("#E6E6E6"), 0.5));
            painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
        }
        painter->restore();

        // 4. Varsayılan item çizimini uygula (metin, simgeler vs.)
        QStyledItemDelegate::paint(painter, opt, index);
    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        // Parent öğeler için 40px, child öğeler için 30px yükseklik belirle
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        if (!index.parent().isValid()) {
            size.setHeight(40); // Parent yüksekliği            
        } else {
            size.setHeight(30); // Child yüksekliği
        }
        return size;
    }
};

#endif // MAINMENULISTWIDGET_H
