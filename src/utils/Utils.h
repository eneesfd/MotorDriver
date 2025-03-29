#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QUuid>
#include <QFile>

class Utils : public QObject
{
    Q_OBJECT
public:
    explicit Utils(QObject *parent = nullptr);

    static std::filesystem::path getCurrentDate();

    static std::filesystem::path getCurrentTime();

    static QString generateUniqueId();

    static QVector<QString> generateColorPalette() {
        QVector<QString> colors = {
            "#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd",
            "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22", "#17becf",
            "#393b79", "#637939", "#8c6d31", "#843c39", "#7b4173",
            "#5254a3", "#6b6ecf", "#9c9ede", "#637939", "#8c6d31"
        };
        return colors;
    }
signals:
};

#endif // UTILS_H
