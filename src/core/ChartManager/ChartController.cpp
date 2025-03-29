#include "ChartController.h"
#include "Utils.h"
#include "qwt_legend.h"
#include "qwt_plot_renderer.h"
#include "qwt_scale_engine.h"
#include "qwt_text.h"

#include <QPrinter>
#include <QPrintDialog>
#include <qpen.h>
#include "LogManager.h"
#include "Constants.h"

CurveController::CurveController(const QString &id, QwtPlot *plot,
                                std::shared_ptr<spdlog::logger> logger_,
                                const QString &title)
    : m_id{id}, m_plot{plot}, m_curve{QSharedPointer<QwtPlotCurve>::create(title)}, m_logger{logger_}
{
    m_curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    m_curve->attach(m_plot);
    SpdlogInfo(m_logger, "CurveController constructor called");
}

CurveController::~CurveController()
{
    if (m_curve) {
        m_curve->detach();
    }
}

QString CurveController::getId() const noexcept
{
    return m_id;
}

QSharedPointer<QwtPlotCurve> CurveController::getCurve() const noexcept
{
    return m_curve;
}

void CurveController::setTitle(const QString &title)
{
    m_curve->setTitle(title);
}

void CurveController::setSymbol(std::unique_ptr<QwtSymbol> symbol)
{
    m_curve->setSymbol(symbol.release());
}

void CurveController::addDataPoints(const QVector<double> &x, const QVector<double> &y)
{
    m_curve->setSamples(x.data(), y.data(), x.size());
    //TODO X ekseni veri boyutu belli bir sayıdan fazla ise otomatik olarak ilk dataların silinmesi gerekli mi? Bunu düzgün yaz.
}

void CurveController::clearDataPoints()
{
    //TODO EXCEPTİON fırlatmak gerebilir TRY Catch yaz
    m_curve->setData(nullptr);
}

ChartController::ChartController(QwtPlot *plot,
                                std::shared_ptr<spdlog::logger> logger_,
                                QObject *parent)
    : QObject(parent), m_plot{plot}, m_logger{logger_}, m_curves{}
{
    SpdlogInfo(m_logger, "ChartController constructor called");
    initializePlot();
}

ChartController::~ChartController()
{
    m_curves.clear();
}

QString ChartController::addCurve(const QString &title)
{
    QString id = Utils::generateUniqueId();
    auto curveController = QSharedPointer<CurveController>::create(id, m_plot, m_logger, title);
    QPen pen(getNextColor());
    curveController->getCurve()->setPen(pen);
    m_curves.insert(id, curveController);
    return id;
}

bool ChartController::removeCurve(const QString &id)
{
    auto it = m_curves.find(id);
    if (it != m_curves.end()) {
        m_curves.erase(it);
        m_plot->replot();
        return true;
    }
    return false;
}

QSharedPointer<CurveController> ChartController::getCurve(const QString &id) const
{
    if (m_curves.contains(id)) {
        return m_curves.value(id);
    }
    return QSharedPointer<CurveController>();
}

void ChartController::addDataPoints(const QString &curveId, const QVector<double> &x, const QVector<double> &y)
{
    QSharedPointer<CurveController> curve = getCurve(curveId);
    if (curve) {
        curve->addDataPoints(x, y);
        m_plot->replot();
    }
}

void ChartController::clearDataPoints(const QString &curveId)
{
    QSharedPointer<CurveController> curve = getCurve(curveId);
    if (curve) {
        curve->clearDataPoints();
        m_plot->replot();
    }
}

void ChartController::setLogScale(int axis, bool enabled)
{
    m_plot->setAxisScaleEngine(axis, enabled ?
                                         static_cast<QwtScaleEngine*>(new QwtLogScaleEngine()) :
                                         static_cast<QwtScaleEngine*>(new QwtLinearScaleEngine())
                               );
}

void ChartController::setTitle(const QString &title)
{
    m_plot->setTitle(title);
}

void ChartController::setBackground(const QBrush &brush)
{
    m_plot->setCanvasBackground(brush);
}

void ChartController::setAutoReplot(bool enabled)
{
    m_plot->setAutoReplot(enabled);
}

void ChartController::setAxisAutoScale(bool enabled)
{
    m_plot->setAxisAutoScale(QwtPlot::xBottom,enabled);
}

void ChartController::setLegend(bool visible, QwtPlot::LegendPosition position)
{
    m_plot->insertLegend(new QwtLegend(),position);
}

void ChartController::setGridVisible(bool visible)
{
    m_grid.get()->setVisible(visible);
    m_plot->replot();
}

void ChartController::setGridProperties(const QColor &color, qreal width, const Qt::PenStyle &style)
{
    m_grid.get()->setPen(color,width,style);
}

void ChartController::setAxisTitles(const QString &xTitle, const QString &yTitle)
{
    m_plot->setAxisTitle(QwtPlot::xBottom,xTitle);
    m_plot->setAxisTitle(QwtPlot::yLeft,yTitle);
}

void ChartController::enablePanning(bool enabled)
{
    m_panner.get()->setEnabled(enabled);
}

void ChartController::enableZooming(bool enabled)
{
    m_zoomer.get()->setEnabled(enabled);
}

void ChartController::enableRubberBand(bool enabled)
{
    m_zoomer.get()->setRubberBand(enabled ?
                                      QwtPlotZoomer::RectRubberBand :
                                      QwtPlotZoomer::NoRubberBand
                                  );
}

void ChartController::setRubberBandProperties(const QColor &color, qreal width, const Qt::PenStyle &style)
{

    QScopedPointer<QPen> pen;
    pen.reset(new QPen(color,width,style));
    //TODO obje adresi verildi doğru mu kontrolü yapılmalı
    m_zoomer.get()->setRubberBandPen(*pen.data());
}

void ChartController::enableMagnifier(bool enabled, double wheelFactor)
{
    m_magnifier.get()->setEnabled(enabled);
    m_magnifier.get()->setWheelFactor(wheelFactor);
}

void ChartController::exportToFile(const QString &filePath)
{
    QwtPlotRenderer renderer;
    renderer.exportTo(m_plot,filePath);
}

void ChartController::print()
{
    QPrinter printer(QPrinter::HighResolution);

    QString docName = m_plot->title().text();


    if ( !docName.isEmpty() )
    {
        docName.replace ( "\n", " -- " );
        printer.setDocName ( docName );
    }
    //TODO BUNU İNCELE SET CREATOR NE İŞE YARIYOR
    printer.setCreator( "Bode example" );
#if QT_VERSION >= 0x050300
    printer.setPageOrientation( QPageLayout::Landscape );
#else
    printer.setOrientation( QPrinter::Landscape );
#endif

    QPrintDialog dialog( &printer );
    if ( dialog.exec() )
    {
        QwtPlotRenderer renderer;

        if ( printer.colorMode() == QPrinter::GrayScale )
        {
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
            renderer.setLayoutFlag( QwtPlotRenderer::FrameWithScales );
        }

    }
}
void ChartController::initializePlot()
{
    m_grid.reset(new QwtPlotGrid());
    m_grid.get()->attach(m_plot);

    m_panner.reset(new QwtPlotPanner(m_plot->canvas()));
    //TODO BU MİDDLE BUTON ÇALIŞIYOR MU
    m_panner.get()->setMouseButton(Qt::MiddleButton);

    m_zoomer.reset(new QwtPlotZoomer(m_plot->canvas()));
    m_zoomer.get()->setMousePattern(QwtEventPattern::MouseSelect2,Qt::RightButton,Qt::ControlModifier);
    m_zoomer.get()->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton);

    m_magnifier.reset(new QwtPlotMagnifier(m_plot->canvas()));
    m_magnifier.get()->setMouseButton(Qt::NoButton);
    m_magnifier.get()->setAxisEnabled(QwtPlot::yLeft,false);
    m_magnifier.get()->setAxisEnabled(QwtPlot::xBottom,false);

    m_plot->setCanvasBackground(QBrush(Qt::white));
    m_plot->setAutoReplot(true);
    m_plot->setAxisAutoScale(true);
}

QColor ChartController::getNextColor()
{
    int baseIndex = m_nextCurveIndex % Constants::ChartController::BaseColors.size();
    int generation = m_nextCurveIndex / Constants::ChartController::BaseColors.size();
    m_nextCurveIndex++;

    QColor color = Constants::ChartController::BaseColors[baseIndex];

    // Eğer generation 0'dan büyükse, temel renge ufak varyasyon ekle
    if (generation > 0)
    {
        int factor = 100 + generation * 10;
        color = color.lighter(factor);
    }

    return color;
}
