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
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QSize>
#include <QApplication>
#include <QStyle>
#include <QPainter>
#include <QProxyStyle>
#include <QScreen>
#include <QStyleOptionToolButton>
#include <stdexcept>
#include <TabToolbar/Group.h>
#include <TabToolbar/Styles.h>
#include <TabToolbar/StyleTools.h>
#include <TabToolbar/SubGroup.h>
#include <TabToolbar/TabToolbar.h>
#include "CompactToolButton.h"
#include "ToolButtonStyle.h"

#include <QLinearGradient>
#include <QLine>

using namespace tt;

Group::Group(const QString& name, QWidget* parent)
    : QFrame(parent)
{
    // Mevcut ayarlar
    setFrameShape(NoFrame);
    setLineWidth(0);
    // Kenarlık ve üst kısım için biraz boşluk
    setContentsMargins(5, 6, 5, 0);

    // Genişliği layout'a göre değişsin, yüksekliği sabit kalsın
    // (Kodun ilerleyen kısmında min ve max height ayarlanıyor)
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // 1) separatorLayout (QHBoxLayout)
    QHBoxLayout* separatorLayout = new QHBoxLayout(this);
    separatorLayout->setContentsMargins(0, 0, 0, 0);
    separatorLayout->setSpacing(0);
    separatorLayout->setDirection(QBoxLayout::LeftToRight);
    setLayout(separatorLayout);

    // 2) outerLayout (QVBoxLayout)
    QVBoxLayout* outerLayout = new QVBoxLayout();
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);
    outerLayout->setDirection(QBoxLayout::TopToBottom);
    separatorLayout->addLayout(outerLayout);
    // separatorLayout->addWidget(CreateSeparator()); // Mevcut kodunuzda yorum satırı

    // 3) GradientFrame (innerFrame)
    innerFrame = new GradientFrame(this);
    innerFrame->setFrameShape(NoFrame);
    innerFrame->setLineWidth(0);
    // GradientFrame'in iç boşlukları
    innerFrame->setContentsMargins(2, 2, 2, 2);
    innerFrame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    innerFrame->setProperty("TTInnerFrame", QVariant(true));

    // Eğer GradientFrame içinde border ayar fonksiyonlarınız varsa örnek:
    // innerFrame->setBorderWidth(5);
    // innerFrame->setRadius(10);

    // 4) innerLayout (QHBoxLayout) - ToolButton'lar için
    innerLayout = new QHBoxLayout(innerFrame);
    innerLayout->setContentsMargins(8, 8, 8, 8);  // Butonlarla çerçeve arasında boşluk
    innerLayout->setSpacing(5);                   // Butonlar arası boşluk
    innerLayout->setDirection(QBoxLayout::LeftToRight);
    innerFrame->setLayout(innerLayout);

    // innerFrame'i outerLayout'a ekliyoruz
    outerLayout->addWidget(innerFrame);

    outerLayout->addSpacing(5); // 5 piksel üst boşluk

    QFrame* gradientLine = new QFrame(this);
    gradientLine->setObjectName("myGradientLine");
    gradientLine->setFixedHeight(2);
    gradientLine->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    outerLayout->addWidget(gradientLine);

    // Mevcut kodunuz - grup property
    setProperty("TTGroup", QVariant(true));

    // 5) groupName (QLabel)
    QLabel* groupName = new QLabel(name, this);
    groupName->setProperty("TTGroupName", QVariant(true));
    groupName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    groupName->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    groupName->adjustSize();
    outerLayout->addWidget(groupName);

    // Mevcut kodunuz - üst bileşenden boyut hesaplaması
    const auto* parentTT = _FindTabToolbarParent(*this);
    if (!parentTT)
        throw std::runtime_error("Group should be constructed inside TabToolbar!");

    unsigned groupMaxHeight = parentTT->GroupMaxHeight();
    unsigned rowCount       = parentTT->RowCount();
    const unsigned height   = groupMaxHeight + groupName->height() + rowCount - 1;
    setMinimumHeight(height);
    setMaximumHeight(height);
}


SubGroup* Group::AddSubGroup(SubGroup::Align align)
{
    SubGroup* sgrp = new SubGroup(align, this);
    innerLayout->addWidget(sgrp);
    return sgrp;
}

QFrame* Group::CreateSeparator()
{
    QFrame* separator = new QFrame(this);
    separator->setProperty("TTSeparator", QVariant(true));
    separator->setAutoFillBackground(false);
    separator->setFrameShadow(QFrame::Plain);
    separator->setLineWidth(1);
    separator->setMidLineWidth(0);
    separator->setFrameShape(QFrame::VLine);
    return separator;
}

void Group::AddSeparator()
{
    innerLayout->addWidget(CreateSeparator());
}

void Group::AddAction(QToolButton::ToolButtonPopupMode type, QAction* action, QMenu* menu)
{
    if(type == QToolButton::MenuButtonPopup)
    {
        innerLayout->addWidget(new CompactToolButton(action, menu, this));
    }
    else
    {
        const int iconSize = GetPixelMetric(QStyle::PM_LargeIconSize) * GetScaleFactor(*this);
        QToolButton* btn = new QToolButton(innerFrame);
        btn->setFixedSize(55, 50);  // İstediğiniz sabit boyut
        btn->setProperty("TTInternal", QVariant(true));
        btn->setAutoRaise(true);
        btn->setDefaultAction(action);
        btn->setIconSize(QSize(40, 40));
        btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        btn->setPopupMode(type);

        QLabel* btnText = new QLabel(innerFrame);
        btnText->setText(action->text());
        btnText->setAlignment(Qt::AlignCenter);
        btnText->adjustSize();
        btnText->setProperty("TTActionName", QVariant(true));

        QVBoxLayout* toolBtnLayout = new QVBoxLayout();
        // toolBtnLayout->setSpacing(5); // 5 piksel dikey boşluk
        toolBtnLayout->addWidget(btn);
        toolBtnLayout->addWidget(btnText);

        if(menu)
            btn->setMenu(menu);
        innerLayout->addLayout(toolBtnLayout);
    }
}

void Group::AddWidget(QWidget* widget)
{
    widget->setParent(this);
    widget->setProperty("TTInternal", QVariant(true));
    innerLayout->addWidget(widget);
}
