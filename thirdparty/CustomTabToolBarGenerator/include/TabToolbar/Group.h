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
#ifndef TT_GROUP_H
#define TT_GROUP_H
#include <QFrame>
#include <QString>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVariant>
#include <TabToolbar/API.h>
#include <TabToolbar/SubGroup.h>

class QAction;
class QMenu;
class GradientFrame;

namespace tt
{

class TT_API Group : public QFrame
{
    Q_OBJECT
public:
    explicit Group(const QString& name, QWidget* parent = nullptr);

    void        AddAction(QToolButton::ToolButtonPopupMode type, QAction* action, QMenu* menu = nullptr);
    void        AddSeparator();
    SubGroup*   AddSubGroup(SubGroup::Align align);
    void        AddWidget(QWidget* widget);

private:
    QFrame*     CreateSeparator();

    QHBoxLayout* innerLayout;
    GradientFrame* innerFrame;
};

}

#include <QFrame>
#include <QPainter>
#include <QPen>
#include <QLinearGradient>

class GradientFrame : public QFrame {
public:
    explicit GradientFrame(QWidget* parent = nullptr)
        : QFrame(parent), borderWidth(2), radius(5)
    {
        // Varsayılan çerçeve stilini kapatıyoruz.
        setFrameStyle(QFrame::NoFrame);
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        // Önce QFrame’in normal içeriğini çiziyoruz.
        QFrame::paintEvent(event);

        QPainter painter(this);
        // Yuvarlatılmış köşeler için anti-aliasing aktif ediliyor.
        painter.setRenderHint(QPainter::Antialiasing);

        QRect rect = this->rect();
        // Border genişliğini göz önünde bulundurarak dikdörtgeni ayarlıyoruz.
        QRect adjustedRect(rect.left() + borderWidth / 2,
                           rect.top() + borderWidth / 2,
                           rect.width() - borderWidth,
                           rect.height() - borderWidth);

        // Yukarıdan aşağı doğru gradient: üstte kırmızı, altta mavi olacak şekilde.
        QLinearGradient gradient(adjustedRect.topLeft(), adjustedRect.bottomLeft());
        gradient.setColorAt(0.0, QColor("#CCCCCC"));
        gradient.setColorAt(0.65, QColor("#666666"));

        QPen pen;
        pen.setWidth(borderWidth);
        pen.setBrush(gradient);
        painter.setPen(pen);

        // Yuvarlatılmış dikdörtgen çizimi
        painter.drawRoundedRect(adjustedRect, radius, radius);
    }

private:
    int borderWidth;
    int radius;
};

#endif
