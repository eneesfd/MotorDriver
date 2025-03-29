#ifndef SUMMARY_H
#define SUMMARY_H

#include <QWidget>

namespace Ui {
class Summary;
}

class Summary : public QWidget
{
    Q_OBJECT

public:
    explicit Summary(QWidget *parent = nullptr);
    ~Summary();

private:
    Ui::Summary *ui;
};

#endif // SUMMARY_H
