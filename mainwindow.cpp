#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include <QPolygonF>
#include <QPointF>

#include "qwt_picker.h"
#include "qwt_plot_picker.h"
#include "qwt_plot_item.h"
#include "qwt_plot_shapeitem.h"
#include "qwt_picker_machine.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    plot = new QwtPlot(this);

    this->setCentralWidget(plot);

    plot->setCanvasBackground(Qt::white);

    /* create a curve */

    curve = new QwtPlotCurve();
    curve->setPen(Qt::blue, 3);

    QPolygonF poly;
    poly << QPointF( 10, 10)
         << QPointF( 90, 10)
         << QPointF( 50, 50)
         << QPointF( 90, 90)
         << QPointF( 10, 90)
         << QPointF( 10, 10);

    curve->setSamples(poly);

    curve->attach(plot);

    /* create a shapeItem */

    shape = new QwtPlotShapeItem("star");

    QPolygonF star;
    star << QPointF( 20, 20)
         << QPointF( 50, 40)
         << QPointF( 80, 20)
         << QPointF( 60, 50)
         << QPointF( 80, 80)
         << QPointF( 50, 60)
         << QPointF( 20, 80)
         << QPointF( 40, 50)
         << QPointF( 20, 20);
    shape->setPolygon(star);
    shape->setBrush(Qt::yellow);
    shape->setPen(Qt::darkYellow, 2);
    shape->attach(plot);

    plot->setAxisScale(QwtPlot::yLeft, 0, 100);
    plot->setAxisScale(QwtPlot::xBottom, 0, 100);
    plot->replot();

    picker = new QwtPlotPicker(plot->canvas());
    picker->setStateMachine(new QwtPickerClickPointMachine);
    picker->setTrackerMode(QwtPicker::AlwaysOff);
    picker->setRubberBand(QwtPicker::RectRubberBand);

    connect(picker, SIGNAL(activated(bool)), this, SLOT(on_picker_activated(bool)));
    connect(picker, SIGNAL(selected(const QPolygon &)), this, SLOT(on_picker_selected(const QPolygon &)));
    connect(picker, SIGNAL(appended(const QPoint &)), this, SLOT(on_picker_appended(const QPoint &)));
    connect(picker, SIGNAL(moved(const QPoint &)), this, SLOT(on_picker_moved(const QPoint &)));
    connect(picker, SIGNAL(removed(const QPoint &)), this, SLOT(on_picker_removed(const QPoint &)));
    connect(picker, SIGNAL(changed(const QPolygon &)), this, SLOT(on_picker_changed(const QPolygon &)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reset()
{
    curve->setPen(Qt::blue, 3);
    shape->setBrush(Qt::yellow);
    shape->setPen(Qt::darkYellow, 3);
}

void MainWindow::on_picker_activated (bool on)
{
    qWarning() << "picker activated " << on;
}

void MainWindow::on_picker_selected (const QPolygon &polygon)
{
    qWarning() << "picker selected " << polygon;
}

void MainWindow::on_picker_appended (const QPoint &pos)
{
    qWarning() << "picker appended " << pos;

    this->reset();

    double coords[ QwtPlot::axisCnt ];
    coords[ QwtPlot::xBottom ] = plot->canvasMap( QwtPlot::xBottom ).invTransform( pos.x() );
    coords[ QwtPlot::xTop ]    = plot->canvasMap( QwtPlot::xTop ).invTransform( pos.x() );
    coords[ QwtPlot::yLeft ]   = plot->canvasMap( QwtPlot::yLeft ).invTransform( pos.y() );
    coords[ QwtPlot::yRight ]  = plot->canvasMap( QwtPlot::yRight ).invTransform( pos.y() );

    QwtPlotItem *item = itemAt(pos);

    if ( item )
    {
        if ( item->rtti() == QwtPlotItem::Rtti_PlotShape )
        {
            QwtPlotShapeItem *theShape = static_cast<QwtPlotShapeItem *>(item);
            theShape->setPen(Qt::red, 3);
            QBrush brush = theShape->brush();
            QColor color = brush.color();
            color.setAlpha(64);
            brush.setColor(color);
            theShape->setBrush(brush);
        }

        if ( item->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            QwtPlotCurve *theCurve = static_cast<QwtPlotCurve *>(item);
            curve->setPen(Qt::red, 3);
        }

        plot->replot();

        qWarning() << "item identified:" << item->rtti();
    }
    else
    {
        qWarning() << "no item itentified at" << coords[QwtPlot::xBottom] << coords[QwtPlot::yLeft];
    }
}

void MainWindow::on_picker_moved (const QPoint &pos)
{
    qWarning() << "picker moved " << pos;
}

void MainWindow::on_picker_removed (const QPoint &pos)
{
    qWarning() << "picker removed " << pos;
}

void MainWindow::on_picker_changed (const QPolygon &selection)
{
    qWarning() << "picker changed " << selection;
}

QwtPlotItem* MainWindow::itemAt( const QPoint& pos ) const
{
    if ( plot == NULL )
        return NULL;

    // translate pos into the plot coordinates
    double coords[ QwtPlot::axisCnt ];
    coords[ QwtPlot::xBottom ] = plot->canvasMap( QwtPlot::xBottom ).invTransform( pos.x() );
    coords[ QwtPlot::xTop ]    = plot->canvasMap( QwtPlot::xTop ).invTransform( pos.x() );
    coords[ QwtPlot::yLeft ]   = plot->canvasMap( QwtPlot::yLeft ).invTransform( pos.y() );
    coords[ QwtPlot::yRight ]  = plot->canvasMap( QwtPlot::yRight ).invTransform( pos.y() );

    QwtPlotItemList items = plot->itemList();
    for ( int i = items.size() - 1; i >= 0; i-- )
    {
        QwtPlotItem *item = items[ i ];
        if ( item->isVisible() && item->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            double dist;

            QwtPlotCurve *curveItem = static_cast<QwtPlotCurve *>( item );
            const QPointF p( coords[ item->xAxis() ], coords[ item->yAxis() ] );

            if ( curveItem->boundingRect().contains( p ) || true )
            {
                // trace curves ...
                dist = 1000.;
                for (size_t line=0; line < curveItem->dataSize() - 1; line++)
                {
                    QPointF pnt;
                    double x, y;

                    pnt = curveItem->sample(line);
                    x = plot->canvasMap( QwtPlot::xBottom ).transform( pnt.x() );
                    y = plot->canvasMap( QwtPlot::yLeft ).transform( pnt.y() );
                    QPointF x0(x,y);

                    pnt = curveItem->sample(line+1);
                    x = plot->canvasMap( QwtPlot::xBottom ).transform( pnt.x() );
                    y = plot->canvasMap( QwtPlot::yLeft ).transform( pnt.y() );
                    QPointF x1(x,y);

                    QPointF r  = pos - x0;
                    QPointF s  = x1 - x0;
                    double s2  = QPointF::dotProduct(s,s);

                    if (s2 > 1e-6)
                    {
                        double xi  = QPointF::dotProduct(r,s) / s2;

                        if ( 0.0 <= xi && xi <= 1.0 )
                        {
                            QPointF t(-s.y()/sqrt(s2), s.x()/sqrt(s2));
                            double d1 = QPointF::dotProduct(r,t);
                            if ( d1 < 0.0 )  { d1 = -d1; }
                            if ( d1 < dist ) { dist = d1;}
                        }
                    }
                    else
                    {
                        dist = sqrt(QPointF::dotProduct(r,r));
                        QPointF r2 = pos - x1;
                        double d2  = sqrt(QPointF::dotProduct(r,r));
                        if ( d2 < dist ) { dist = d2; }
                    }
                }

                qWarning() << "curve dist =" << dist;

                if ( dist <= 5 ) return static_cast<QwtPlotItem *>(curveItem);
            }
        }
        if ( item->isVisible() && item->rtti() == QwtPlotItem::Rtti_PlotShape )
        {
            QwtPlotShapeItem *shapeItem = static_cast<QwtPlotShapeItem *>( item );
            const QPointF p( coords[ item->xAxis() ], coords[ item->yAxis() ] );

            if ( shapeItem->boundingRect().contains( p ) && shapeItem->shape().contains( p ) )
            {
                return static_cast<QwtPlotItem *>(shapeItem);
            }
        }
    }

    return NULL;
}
