#include <QtGui>
#include "humiditygraph.h"

#define POSITON_ORDONNE 40

HumidityGraph::HumidityGraph(QWidget *parent)
    : QFrame(parent)
{
    setAutoFillBackground(true);
    maxList = 50;
    mouseOver = false;
}

void HumidityGraph::paintEvent(QPaintEvent *event)
{
    double graphx = POSITON_ORDONNE;
    double largeur_graph = width() - POSITON_ORDONNE - 10;
    double incx = largeur_graph / maxList;
//    double minGraph = 0;
//    double maxGraph = 100;
    double minGraph = 10 * (ceil(minHumidity / 10) - 1);
    double maxGraph = 10 * ceil(maxHumidity / 10);

    if (!humidityList.isEmpty())
    {
        QPainter painter(this);
//        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(QPen(Qt::darkGreen, 0));

        painter.drawLine(graphx, height() - 10, width() - 10, height() - 10);
        painter.drawLine(graphx, 10, 40, height() - 10);

        painter.setPen(QPen(Qt::green, 0));
        painter.drawText(5,15, QString("%1%").arg(maxGraph));
        painter.drawText(5,height() - 10, QString("%1%").arg(minGraph));

        QPoint p1 = QPoint(0,0);
        QPoint p2 = QPoint(
                    graphx, height() - (int)(
                    (humidityList.first() - minGraph) / (maxGraph - minGraph) *
                    (height() - 20)
                        ) - 10
                           );
        foreach (const double &temp, humidityList)
        {
            graphx += incx;
            if (!p1.isNull())
            {
                p2 = QPoint(
                            (int)graphx, height() - (int)(
                            (temp - minGraph) / (maxGraph - minGraph) *
                            (height() - 20)
                                ) - 10
                                   );
                painter.drawLine(p1, p2);
            }
            p1 = p2;
        }
    }

    if (mouseOver)
    {
        int pos_graph = mousex  - POSITON_ORDONNE;
        int index = -1;

        if (pos_graph >= 0 && pos_graph <= largeur_graph)
        {
            index = pos_graph / incx;
        }

        if (index >= 0 && index < humidityList.count())
        {
            double temp = humidityList.at(index);
            QPainter painter(this);
            painter.setPen(QPen(QColor(255,255,255,192)));
            painter.drawText(mousex + 2, mousey, QString("%1%").arg(temp));
            painter.drawLine(mousex, 10, mousex, height() - 10);
        }
        else
        {
            mouseOver = false;
        }
    }
}

void HumidityGraph::addHumidity(double humidity)
{
    if (humidityList.size() == 0)
    {
        minHumidity = humidity;
        maxHumidity = humidity;
    }
    else
    {
        minHumidity = qMin(humidity, minHumidity);
        maxHumidity = qMax(humidity, maxHumidity);
    }
    if (humidityList.size() == maxList)
    {
        humidityList.removeFirst();
        dateList.removeFirst();
    }
    humidityList << humidity;
    dateList << QDateTime::currentDateTime();

    //qDebug("addHumidity %f",humidity);
    update();
}

void HumidityGraph::mouseMoveEvent(QMouseEvent *event)
{
    if (event->y() >= 10 && event->y() <= height() - 10)
    {
        mousex = event->x();
        mousey = event->y();
        mouseOver = true;
    }
    else
    {
        mouseOver = false;
    }
    update();
}
