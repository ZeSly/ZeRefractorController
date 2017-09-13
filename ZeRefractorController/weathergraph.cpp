#include <QPainter>
#include <QMouseEvent>
#include "weathergraph.h"

#define POSITON_ORDONNE 40

WeatherGraph::WeatherGraph(QWidget *parent)
    : QFrame(parent)
{
    setAutoFillBackground(true);
    mouseOver = false;
    select_start = 0;
    select_stop = 0;
    isZoomed = false;
    labelPrecison = 1;
}

WeatherGraph::~WeatherGraph()
{

}

void WeatherGraph::paintEvent(QPaintEvent *event)
{
    double graphx = POSITON_ORDONNE;
    double largeur_graph = width() - POSITON_ORDONNE - 10;
    double incx;
    double minGraph = labelPrecison * (ceil(minValue / labelPrecison) - 1);
    double maxGraph = labelPrecison * ceil(maxValue / labelPrecison);

    QPainter painter(this);

    //        painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::darkGreen, 0));

    painter.drawLine(graphx, height() - 10, width() - 10, height() - 10);
    painter.drawLine(graphx, 10, 40, height() - 10);

    if (!valueList.isEmpty())
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
        for (int i = 0 ; i < maxGraph - minGraph; i += labelPrecison)
        {
            int l = height() - 10 - i*h;
            painter.drawLine(graphx, l, width() - 10, l);
        }

        painter.setPen(QPen(Qt::green, 0));
        painter.drawText(5,15, valueFormat.arg(maxGraph));
        painter.drawText(5,height() - 10, valueFormat.arg(minGraph));

        QPoint p1 = QPoint(
            graphx - 1, height() - (int)(
            (valueList.first() - minGraph) / (maxGraph - minGraph) *
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
                (valueList.at(i) - minGraph) / (maxGraph - minGraph) *
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

            if (index >= 0 && index < valueList.count())
            {
                QString temp = valueFormat.arg(valueList.at(index));
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

int WeatherGraph::getCursorIndex(int pos_graph)
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

void WeatherGraph::setFormat(QString &f)
{
    valueFormat = f;
}

void WeatherGraph::setLabelPrecison(unsigned int p)
{
    labelPrecison = p;
}

void WeatherGraph::addValue(double value)
{
    QDateTime now = QDateTime::currentDateTime();

    if (dateList.isEmpty() || dateList.last().secsTo(now) >= 1)
    {
        if (valueList.size() == 0)
        {
            minValue = value;
            maxValue = value;
            firstIndex = 0;
            isZoomed = false;
        }
        else
        {
            minValue = qMin(value, minValue);
            maxValue = qMax(value, maxValue);
        }

        // remove first value if it was more than 12h ago
        if (!dateList.isEmpty() && dateList.first().secsTo(now) > 3600 * 12)
        {
            valueList.removeFirst();
            dateList.removeFirst();
        }

        // add only if value has change
        //if (!dateList.isEmpty() && valueList.last() == value)
        //{
        //    valueList.removeLast();
        //    dateList.removeLast();
        //}

        valueList << value;
        dateList << now;
        if (!isZoomed && !select_start)
        {
            lastIndex = valueList.count() - 1;
        }
        //qDebug("addTemperature %f",temperature);
        update();
    }
}

void WeatherGraph::mouseMoveEvent(QMouseEvent *event)
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

void WeatherGraph::mousePressEvent(QMouseEvent *event)
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

void WeatherGraph::mouseReleaseEvent(QMouseEvent *event)
{
    if (select_start)
    {
        int _start = qMin(select_start,select_stop);
        select_stop = qMax(select_start,select_stop);
        select_start = _start;
        int f = getCursorIndex(select_start - POSITON_ORDONNE);
        int l = getCursorIndex(select_stop - POSITON_ORDONNE);
        firstIndex = f < 0 ? 0 : f;
        lastIndex = l < 0 ? valueList.count() - 1 : l;
        isZoomed = true;
        select_start = 0;
        select_stop = 0;
        update();
    }
}

void WeatherGraph::mouseDoubleClickEvent(QMouseEvent *event)
{
    firstIndex = 0;
    lastIndex = valueList.count() - 1;
    isZoomed = false;
    select_start = 0;
    select_stop = 0;
    update();
}