/*
    TabToolbar - a small utility library for Qt, providing tabbed toolbars
	Copyright (C) 2018 Oleksii Sierov
	
    TabToolbar is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TabToolbar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with TabToolbar.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TAB_TOOLBAR_H
#define TAB_TOOLBAR_H
#include <QTabWidget>
#include <QToolBar>
#include <QList>
#include <QTimer>
#include <memory>
#include <TabToolbar/API.h>

class QToolButton;
class QFrame;
class QMenu;

namespace tt
{
class Page;
class StyleParams;

class TT_API TabToolbar : public QToolBar
{
    Q_OBJECT
public:
    explicit TabToolbar(QWidget* parent = nullptr, unsigned _groupMaxHeight = 75, unsigned _groupRowCount = 3);
    virtual ~TabToolbar();

    void     SetSpecialTabEnabled(bool enabled);
    Page*    AddPage(const QString& pageName);
    QAction* HideAction();
    void     AddCornerAction(QAction* action);
    void     SetStyle(const QString& styleName);
    QString  GetStyle() const;
    unsigned RowCount() const;
    unsigned GroupMaxHeight() const;
    int      CurrentTab() const;
    void     SetCurrentTab(int index);

signals:
    void     Minimized();
    void     Maximized();
    void     SpecialTabClicked();
    void     StyleChanged();

private slots:
    void     FocusChanged(QWidget* old, QWidget* now);
    void     TabClicked(int index);
    void     CurrentTabChanged(int index);
    void     HideAt(int index = -1);
    void     HideTab(int index);
    void     ShowTab(int index);

protected:
    bool     event(QEvent* event) override;

private:
    void     AdjustVerticalSize(unsigned vSize);

    const unsigned groupRowCount;
    const unsigned groupMaxHeight;
    bool           hasSpecialTab = false;
    int            currentIndex = 0;
    unsigned       maxHeight = QWIDGETSIZE_MAX;
    QFrame*        cornerActions = nullptr;
    QAction*       hideAction = nullptr;
    QToolButton*   hideButton = nullptr;
    QAction*       tabBarHandle = nullptr;
    QTabWidget*    tabBar = nullptr;
    bool           ignoreStyleEvent = false;
    bool           isMinimized = false;
    bool           isShown = true;
    QTimer         tempShowTimer;
    std::unique_ptr<StyleParams> style;

    friend class Page;
};

TabToolbar* _FindTabToolbarParent(QWidget& startingWidget);

}

#include <QTabBar>
#include <QPainter>
#include <QLinearGradient>
#include <QPaintEvent>

// Animasyonsuz TabBar Tasarımı, animasyon istenmediği durumda kullanılması üzerine yorum olarak bırakılmıştır

// class CustomTabBar : public QTabBar {
//     Q_OBJECT
// public:
//     explicit CustomTabBar(QWidget *parent = nullptr) : QTabBar(parent) {}

// protected:
//     void paintEvent(QPaintEvent *event) override {
//         QTabBar::paintEvent(event);

//         QPainter painter(this);
//         painter.setRenderHint(QPainter::Antialiasing, true);

//         int currentIdx = this->currentIndex();
//         if (currentIdx >= 0) {
//             QRect tabR = tabRect(currentIdx);
//             // Alt kenarda 2px yüksekliğinde çizgi
//             // Çizgiyi metinle daha iyi ortalamak için sol ve genişlik değerlerinde ayarlama yapıyoruz.
//             QRect gradientRect(tabR.left(), tabR.bottom() - 2, tabR.width(), 2);
//             // Örneğin, 1px sola kaydırıp genişliği 2px arttırıyoruz:
//             gradientRect.setX(tabR.left() - 1);
//             gradientRect.setWidth(tabR.width() - 15);

//             QLinearGradient grad(gradientRect.topLeft(), gradientRect.topRight());
//             // %0: FDD700, 0 opacity
//             grad.setColorAt(0.0, QColor(253, 215, 0, 0));
//             // %50: FDD700, 100 opacity
//             grad.setColorAt(0.5, QColor(253, 215, 0, 255));
//             // %100: FDD700, 0 opacity
//             grad.setColorAt(1.0, QColor(253, 215, 0, 0));

//             painter.fillRect(gradientRect, grad);
//         }
//     }
// };

#include <QPropertyAnimation>

class CustomTabBar : public QTabBar {
    Q_OBJECT
    Q_PROPERTY(QRect underlineRect READ underlineRect WRITE setUnderlineRect NOTIFY underlineRectChanged)
public:
    explicit CustomTabBar(QWidget *parent = nullptr)
        : QTabBar(parent)
    {
        // İlk başta mevcut tab varsa underlineRect'i ayarlıyoruz
        if (count() > 0)
            m_underlineRect = tabRect(currentIndex());

        // currentChanged sinyaline animasyonu tetiklemek için bağlanıyoruz
        connect(this, &QTabBar::currentChanged, this, &CustomTabBar::animateUnderline);
    }

    QRect underlineRect() const { return m_underlineRect; }

    void setUnderlineRect(const QRect &rect) {
        if (m_underlineRect != rect) {
            m_underlineRect = rect;
            emit underlineRectChanged();
            update(); // Yeniden çizim için
        }
    }

signals:
    void underlineRectChanged();

protected:
    void paintEvent(QPaintEvent *event) override {
        QTabBar::paintEvent(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        // Eğer hiçbir tab seçilmemişse varsayılan olarak ilk tabı kullan
        int idx = currentIndex();
        if (idx < 0 && count() > 0)
            idx = 0;

        if (idx >= 0) {
            QRect tabR = tabRect(idx);

            // Eğer underlineRect henüz ayarlanmamışsa, ilk tabın rect'ini ata.
            if (m_underlineRect.isNull())
                m_underlineRect = tabR;

            // Gradient çizgi için dikdörtgeni hesaplıyoruz.
            // Burada çizgiyi tabın alt sınırının 2 piksel aşağısına çekiyoruz.
            int underlineY = tabR.bottom() - 2; // 2px aşağıya kaydırdık
            QRect gradientRect(m_underlineRect.left(), underlineY,
                               m_underlineRect.width(), 2);

            // Metinle daha iyi hizalama için küçük ayarlamalar (isteğe bağlı)
            gradientRect.setX(gradientRect.left() - 1);
            gradientRect.setWidth(gradientRect.width() - 15
                                  );

            QLinearGradient grad(gradientRect.topLeft(), gradientRect.topRight());
            // %0 - FDD700, 0 opacity
            grad.setColorAt(0.0, QColor(253, 215, 0, 0));
            // %50 - FDD700, 100 opacity
            grad.setColorAt(0.5, QColor(253, 215, 0, 255));
            // %100 - FDD700, 0 opacity
            grad.setColorAt(1.0, QColor(253, 215, 0, 0));

            painter.fillRect(gradientRect, grad);
        }
    }

private slots:
    void animateUnderline(int index) {
        if (index < 0 || index >= count())
            return;

        QRect targetRect = tabRect(index);
        // Animasyon nesnesi oluşturup, "underlineRect" property'sini animasyonla değiştiriyoruz.
        QPropertyAnimation* anim = new QPropertyAnimation(this, "underlineRect");
        anim->setDuration(300); // Animasyon süresi (ms)
        anim->setStartValue(m_underlineRect);
        anim->setEndValue(targetRect);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

private:
    QRect m_underlineRect;
};


#include <QTabWidget>

class CustomTabWidget : public QTabWidget {
    Q_OBJECT
public:
    explicit CustomTabWidget(QWidget* parent = nullptr) : QTabWidget(parent) {
        // Protected setTabBar() burada erişilebilir.
        setTabBar(new CustomTabBar(this));
    }
};

#endif
