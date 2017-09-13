#include <QtGui>
#include "temperaturegraph.h"

#define POSITON_ORDONNE 40

TemperatureGraph::TemperatureGraph(QWidget *parent)
    : QFrame(parent)
{
    setAutoFillBackground(true);
    maxList = 50;
    mouseOver = false;
    select_start = 0;
    select_stop = 0;
    timeDisplayed = 0;
    isZoomed = false;
}

void TemperatureGraph::paintEvent(QPaintEvent *event)
{
    double graphx = POSITON_ORDONNE;
    double largeur_graph = width() - POSITON_ORDONNE - 10;
    double incx;    // = largeur_graph / maxList;
    double minGraph = ceil(minTemp) - 1;
    double maxGraph = ceil(maxTemp);

    QPainter painter(this);

    //        painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::darkGreen, 0));

    painter.drawLine(graphx, height() - 10, width() - 10, height() - 10);
    painter.drawLine(graphx, 10, 40, height() - 10);

    if (!temperatureList.isEmpty())
    {
        QDateTime firstDateDisplayed = dateList.at(firstIndex);
        QDateTime lastDateDisplayed = dateList.at(lastIndex);
        int largeur_temps = firstDateDisplayed.secsTo(lastDateDisplayed);
        if (largeur_temps < 30 && !isZoomed) largeur_temps = 30;
        double pps = largeur_graph / largeur_temps;

        painter.setPen(QPen(Qt::darkGreen, 0, Qt::DotLine));
        int d = maxGraph - minGraph;
        int h = (height() - 20);
        h /= d;
        for (int i = 1 ; i < maxGraph - minGraph; i++)
        {
            int l = height() - 10 - i*h;
            painter.drawLine(graphx, l, width() - 10, l);
        }

        painter.setPen(QPen(Qt::green, 0));
        painter.drawText(5,15, QString("%1 °C").arg(maxGraph));
        painter.drawText(5,height() - 10, QString("%1 °C").arg(minGraph));

        QPoint p1 = QPoint(
            graphx - 1, height() - (int)(
            (temperatureList.first() - minGraph) / (maxGraph - minGraph) *
            (height() - 20)
            ) - 10
            );
        QPoint p2;
        QDateTime d1 = firstDateDisplayed;
        QDateTime d2;

        for (int i = firstIndex ; i <= lastIndex ; i++)
        {
            d2 = dateList.at(i);
            incx = d1.secsTo(d2) * pps;
            graphx += incx;
            d1 = d2;

            p2 = QPoint(
                (int)graphx, height() - (int)(
                (temperatureList.at(i) - minGraph) / (maxGraph - minGraph) *
                (height() - 20)
                ) - 10
                );
            painter.drawLine(p1, p2);
            p1 = p2;
        }

        painter.setPen(QPen(QColor(255,255,255,192)));

        if (mouseOver)
        {
            int pos_graph = mousex  - POSITON_ORDONNE;
            int index = getCursorIndex(pos_graph);

            if (index >= 0 && index < temperatureList.count())
            {
                QString temp = QString("%1 °C").arg(temperatureList.at(index));
                QString when = dateList.at(index).toString("hh:mm:ss");
                QFontMetrics fm = painter.fontMetrics();
                int w = fm.width(when);
                int texty;

                if (mousey - 2 > 2 * fm.height())
                {
                    texty = mousey - 2;
                }
                else
                {
                    texty = mousey + 2 * fm.height();
                }
                if (mousex + 2 + w < width())
                {
                    painter.drawText(mousex + 2, texty, temp);
                    painter.drawText(mousex + 2, texty - fm.height(), when);
                }
                else
                {
                    painter.drawText(mousex - 2 - fm.width(temp), texty, temp);
                    painter.drawText(mousex - 2 - fm.width(when), texty - fm.height(), when);
                }

                painter.drawLine(mousex, 10, mousex, height() - 10);
                if (select_start)
                {
                    select_stop = mousex;
                }
            }
            else
            {
                mouseOver = false;
            }
        }
        if (select_start)
        {
            painter.drawRect(qMin(select_start, select_stop), 10, qAbs(select_stop - select_start), height() - 20);
        }
    }
}

int TemperatureGraph::getCursorIndex(int pos_graph)
{
    int index = -1;
    double largeur_graph = width() - POSITON_ORDONNE - 10;
    QDateTime firstDateDisplayed = dateList.at(firstIndex);
    QDateTime lastDateDisplayed = dateList.at(lastIndex);
    int largeur_temps = firstDateDisplayed.secsTo(lastDateDisplayed);
    if (largeur_temps < 30 && !isZoomed) largeur_temps = 30;
    double pps = largeur_graph / largeur_temps;

    QString s_firstDateDisplayed = firstDateDisplayed.toString();
    QString s_lastDateDisplayed = lastDateDisplayed.toString();

    if (pos_graph >= 0 && pos_graph <= largeur_graph)
    {
        QDateTime pos_temps = firstDateDisplayed.addSecs(pos_graph / pps);
        QString s_pos_temps = pos_temps.toString();
        if (pos_temps < lastDateDisplayed)
        {
            for (index = firstIndex ; index < lastIndex ; index++)
            {
                if (dateList.at(index) >= pos_temps) break;
            }
        }
    }

    return index;
}

void TemperatureGraph::addTemperature(double temperature)
{
    QDateTime now = QDateTime::currentDateTime();

    if (temperatureList.size() == 0)
    {
        minTemp = temperature;
        maxTemp = temperature;
        firstIndex = 0;
        isZoomed = false;
    }
    else
    {
        minTemp = qMin(temperature, minTemp);
        maxTemp = qMax(temperature, maxTemp);
    }
    //    if (temperatureList.size() == maxList)
    //    {
    //        temperatureList.removeFirst();
    //        dateList.removeFirst();
    //    }
    //    if (temperatureList.last() != temperature)
    {
        temperatureList << temperature;
        dateList << now;
        if (!isZoomed && !select_start)
        {
            lastIndex = temperatureList.count() - 1;
        }
    }
    //qDebug("addTemperature %f",temperature);
    update();
}

void TemperatureGraph::mouseMoveEvent(QMouseEvent *event)
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

void TemperatureGraph::mousePressEvent(QMouseEvent *event)
{
    if (mouseOver)
    {
        select_start = event->x();
    }
    //    else
    //    {
    //        select_start = 0;
    //    }
}

void TemperatureGraph::mouseReleaseEvent(QMouseEvent *event)
{
    if (select_start)
    {
        int _start = qMin(select_start,select_stop);
        select_stop = qMax(select_start,select_stop);
        select_start = _start;
        int f = getCursorIndex(select_start - POSITON_ORDONNE);
        int l = getCursorIndex(select_stop - POSITON_ORDONNE);
        firstIndex = f < 0 ? 0 : f;
        lastIndex = l < 0 ? temperatureList.count() - 1 : l;
        isZoomed = true;
        select_start = 0;
        select_stop = 0;
        update();
    }
}

void TemperatureGraph::mouseDoubleClickEvent(QMouseEvent *event)
{
    firstIndex = 0;
    lastIndex = temperatureList.count() - 1;
    isZoomed = false;
    select_start = 0;
    select_stop = 0;
    update();
}