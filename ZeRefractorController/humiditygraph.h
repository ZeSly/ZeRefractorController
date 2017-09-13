#ifndef HUMIDITYGRAPH_H
#define HUMIDITYGRAPH_H

#include <QBrush>
#include <QPen>
#include <QFrame>
#include <QWidget>
#include <QDateTime>

namespace Ui
{
    class HumidityGraph;
}

class HumidityGraph : public QFrame
{
    Q_OBJECT

public:
    HumidityGraph(QWidget *parent = 0);
    void addHumidity(double humidity);

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    int maxList;
    QList<double> humidityList;
    QList<QDateTime> dateList;
    double minHumidity;
    double maxHumidity;

    bool mouseOver;
    int mousex, mousey;
};

#endif // HUMIDITYGRAPH_H
