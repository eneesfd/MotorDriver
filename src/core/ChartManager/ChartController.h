#ifndef CHARTCONTROLLER_H
#define CHARTCONTROLLER_H

#include <QObject>
#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_magnifier.h"
#include "qwt_plot_panner.h"
#include "qwt_plot_zoomer.h"


namespace spdlog
{
class logger;
}

class CurveController
{
public:
    CurveController(const QString& id, QwtPlot* plot,
                    std::shared_ptr<spdlog::logger> logger_,
                    const QString& title = QString());
    ~CurveController();

    CurveController(CurveController&&) noexcept = default;
    CurveController& operator=(CurveController&&) noexcept = default;
    CurveController(const CurveController&) = delete;
    CurveController& operator=(const CurveController&) = delete;

    QString getId() const noexcept;
    QSharedPointer<QwtPlotCurve> getCurve() const noexcept;

    void setTitle(const QString& title);
    void setSymbol(std::unique_ptr<QwtSymbol> symbol);
    void addDataPoints(const QVector<double>& x, const QVector<double>& y);
    void clearDataPoints();

private:
    QString m_id;
    QwtPlot* m_plot;
    QSharedPointer<QwtPlotCurve> m_curve;
    std::shared_ptr<spdlog::logger> m_logger;
};

class ChartController : public QObject
{
    Q_OBJECT
public:
    explicit ChartController(QwtPlot* plot, std::shared_ptr<spdlog::logger> logger_, QObject* parent = nullptr);
    ~ChartController();

    QString addCurve(const QString& title);
    bool removeCurve(const QString& id);
    QSharedPointer<CurveController> getCurve(const QString& id) const;

    void addDataPoints(const QString& curveId, const QVector<double>& x, const QVector<double>& y);
    void clearDataPoints(const QString& curveId);

    void setLogScale(int axis, bool enabled);
    void setTitle(const QString& title);
    void setBackground(const QBrush& brush);
    void setAutoReplot(bool enabled);
    void setAxisAutoScale(bool enabled);
    void setLegend(bool visible, QwtPlot::LegendPosition position = QwtPlot::BottomLegend);
    void setGridVisible(bool visible);
    void setGridProperties(const QColor& color, qreal width = 1, const Qt::PenStyle &style = Qt::PenStyle::DotLine);
    void setGridWidth(int width);
    void setAxisTitles(const QString& xTitle, const QString& yTitle);

    void enablePanning(bool enabled);
    void enableZooming(bool enabled);
    void enableRubberBand(bool enabled);
    void setRubberBandProperties(const QColor& color, qreal width = 1, const Qt::PenStyle &style = Qt::PenStyle::DotLine);
    void enableMagnifier(bool enabled, double wheelFactor = 1.1);

    void exportToFile(const QString& filePath);
    void print();

private:
    QwtPlot* m_plot;
    QHash<QString, QSharedPointer<CurveController>> m_curves;

    std::unique_ptr<QwtPlotGrid> m_grid;
    std::unique_ptr<QwtPlotPanner> m_panner;
    std::unique_ptr<QwtPlotZoomer> m_zoomer;
    std::unique_ptr<QwtPlotMagnifier> m_magnifier;
    std::shared_ptr<spdlog::logger> m_logger;
    void initializePlot();
    int m_nextCurveIndex = 0; // Yeni curve eklerken kullanılacak sayaç
    QColor getNextColor();
};

#endif // CHARTCONTROLLER_H
