#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QwtPlot;
class QwtPlotCurve;
class QwtPicker;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_picker_activated (bool on);
    void on_picker_selected (const QPolygon &polygon);
    void on_picker_appended (const QPoint &pos);
    void on_picker_moved (const QPoint &pos);
    void on_picker_removed (const QPoint &pos);
    void on_picker_changed (const QPolygon &selection);

private:
    Ui::MainWindow *ui;
    QwtPlot *plot;
    QwtPlotCurve *curve;
    QwtPicker *picker;
};

#endif // MAINWINDOW_H
