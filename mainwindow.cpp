#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include <QPolygonF>
#include <QPointF>

#include "qwt_picker.h"
#include "qwt_plot_picker.h"
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

    curve = new QwtPlotCurve();
    curve->setPen(QPen(QColor(255, 0, 0), 3));

    QPolygonF poly;
    poly << QPointF( 10, 10)
         << QPointF( 90, 10)
         << QPointF( 90, 90)
         << QPointF( 10, 90);

    curve->setSamples(poly);

    curve->attach(plot);

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
    double coords[ QwtPlot::axisCnt ];
    coords[ QwtPlot::xBottom ] = plot->canvasMap( QwtPlot::xBottom ).invTransform( pos.x() );
    coords[ QwtPlot::xTop ]    = plot->canvasMap( QwtPlot::xTop ).invTransform( pos.x() );
    coords[ QwtPlot::yLeft ]   = plot->canvasMap( QwtPlot::yLeft ).invTransform( pos.y() );
    coords[ QwtPlot::yRight ]  = plot->canvasMap( QwtPlot::yRight ).invTransform( pos.y() );

    qWarning() << "picker appended " << pos
               //<< QwtPlot::axisCnt
               << coords[QwtPlot::xBottom]
               //<< coords[QwtPlot::xTop]
               << coords[QwtPlot::yLeft]
               //<< coords[QwtPlot::yRight]
               ;
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
