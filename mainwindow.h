#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QwtPlot;
class QwtPlotCurve;
class QwtPicker;
class QwtPlotItem;
class QwtPlotShapeItem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void reset();
    QwtPlotItem* itemAt( const QPoint& pos ) const;

public slots:
    void on_picker_activated (bool on);
    void on_picker_selected (const QPolygon &polygon);
    void on_picker_appended (const QPoint &pos);
    void on_picker_moved (const QPoint &pos);
    void on_picker_removed (const QPoint &pos);
    void on_picker_changed (const QPolygon &selection);

private:
    Ui::MainWindow   *ui;
    QwtPlot          *plot;
    QwtPlotCurve     *curve;
    QwtPlotShapeItem *shape;
    QwtPicker        *picker;
};

#endif // MAINWINDOW_H
