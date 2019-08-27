#ifndef WEATHERGRAPH_H
#define WEATHERGRAPH_H

#include <QBrush>
#include <QPen>
#include <QFrame>
#include <QWidget>
#include <QDateTime>

namespace Ui
{
    class WeatherGraph;
}

class WeatherGraph : public QFrame
{
    Q_OBJECT

public:
    WeatherGraph(QWidget *parent);
    ~WeatherGraph();
    void setFormat(QString f);
    void setLabelPrecison(unsigned int p);
    void addValue(double Value);

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);

    QString valueFormat;
    unsigned int labelPrecison;

private:
    QList<double> valueList;
    QList<QDateTime> dateList;
    double minValue;
    double maxValue;
    int firstIndex, lastIndex;

    bool mouseOver;
    int mousex, mousey;
    int select_start;
    int select_stop;
    bool isZoomed;

    int getCursorIndex(int pos_graph);
};

#endif // WEATHERGRAPH_H
