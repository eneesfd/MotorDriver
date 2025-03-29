#ifndef PANELHEADER_H
#define PANELHEADER_H

#include <QLabel>
#include <QWidget>

namespace Ui {
class PanelHeader;
}

class PanelHeader : public QWidget
{
    Q_OBJECT

public:
    explicit PanelHeader(QWidget *parent = nullptr);
    ~PanelHeader();
    QLabel* getWidgetTitleLabel() const;
    QLabel* getPageTitleLabel() const;
private:
    Ui::PanelHeader *ui;

};

#endif // PANELHEADER_H
