#ifndef TEMPERATUREGRAPH_H
#define TEMPERATUREGRAPH_H

#include <QBrush>
#include <QPen>
#include <QFrame>
#include <QWidget>
#include <QDateTime>

namespace Ui
{
    class TemperatureGraph;
}

class TemperatureGraph : public QFrame
{
    Q_OBJECT

public:
    TemperatureGraph(QWidget *parent = 0);
    void addTemperature(double temperature);

protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);

private:
    int maxList;
    QList<double> temperatureList;
    QList<QDateTime> dateList;
    double minTemp;
    double maxTemp;
    int firstIndex, lastIndex;
    int timeDisplayed;

    bool mouseOver;
    int mousex, mousey;
    int select_start;
    int select_stop;
    bool isZoomed;

    int getCursorIndex(int pos_graph);
};

#endif // TEMPERATUREGRAPH_H
