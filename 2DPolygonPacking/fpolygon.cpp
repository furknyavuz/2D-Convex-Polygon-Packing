#include "fpolygon.h"

namespace PolygonPacking
{

FPolygon::FPolygon()
{
}

void FPolygon::rotateFPolygon(qreal degree)
{
    QMatrix matrix;
    matrix.rotate(degree);

    this->setPolygon((QPolygonF)matrix.map((QPolygonF)this->polygon()));
}

qreal FPolygon::rotateToMinBound(RotateToMinBoundAlgorithms algorithm)
{
    switch (algorithm)
    {
    case ROTATE_TO_MIN_BOUND_SEARCH_WITH_ROTATE :
        return rotateToMinBoundSearchWithRotate();
    case ROTATE_TO_MIN_BOUND_PARALLEL_EDGE :
        return rotateToMinBoundParallelEdge();
    default :
        return rotateToMinBoundSearchWithRotate();
    }
    return rotateToMinBoundSearchWithRotate();
}

qreal FPolygon::rotateToMinBoundSearchWithRotate()
{
    qreal width = this->boundingRect().width();
    qreal height = this->boundingRect().height();
    qreal area = width * height;
    qreal minArea = area;

    int minDegree = 0;

    for(int degree = 1; degree < 90; ++degree)
    {
        this->rotateFPolygon(1);
        width = this->boundingRect().width();
        height = this->boundingRect().height();

        area = width * height;
        if(area < minArea)
        {
            minArea = area;
            minDegree = degree;
        }
    }
    this->rotateFPolygon(minDegree - 89);

    return minDegree;
}

qreal FPolygon::rotateToMinBoundParallelEdge()
{
    qreal bestAngle = 0;
    qreal bestArea = 0;
    for(int ctr = 0; ctr <= this->polygon().size(); ++ctr)
    {
        QPolygonF poly = this->polygon();

        QPointF p1 = poly[ctr % poly.size()];
        qreal x1 = p1.x();
        qreal y1 = p1.y();

        QPointF p2 = poly[(ctr+1) % poly.size()];
        qreal x2 = p2.x();
        qreal y2 = p2.y();

        QLineF newLine(x1,y1,x2,y2);
        qreal angle = newLine.angle();
        rotateFPolygon(angle);
        if(ctr == 0)
        {
            bestArea = this->boundArea();
            bestAngle = angle;
        }
        else
        {
            if(this->boundArea() < bestArea)
            {
                bestArea = this->boundArea();
                bestAngle = angle;
            }
        }
        rotateFPolygon(-angle);
    }
    this->rotateFPolygon(bestAngle);

    return bestAngle;
}

qreal FPolygon::area()
{
    qreal area = 0;
    qreal x, y, xNext, yNext;

    int size = this->polygon().size();

    for (int ctr = 0; ctr < size; ctr++)
    {
        x = this->polygon().at(ctr).x();
        y = this->polygon().at(ctr).y();
        xNext = this->polygon().at((ctr+1)%size).x();
        yNext = this->polygon().at((ctr+1)%size).y();

        area += x*yNext - y*xNext;
    }
    area = area / 2;
    return qAbs(area);
}

qreal FPolygon::boundArea()
{
    qreal width = this->boundingRect().width();
    qreal height = this->boundingRect().height();

    return width * height;
}

void FPolygon::setFPosition(qreal x, qreal y)
{
    qreal boundX = this->boundingRect().left();
    qreal boundY = this->boundingRect().top();

    QPolygonF polygon;
    for(int ctr = 0; ctr < this->polygon().size(); ++ctr)
    {
        qreal lastX = this->polygon().at(ctr).x();
        qreal lastY = this->polygon().at(ctr).y();
        polygon << QPointF::QPointF(lastX - boundX + x, lastY - boundY + y);
    }
    this->setPolygon(polygon);
}

qreal FPolygon::localDensity()
{
    return (this->area() / this->boundArea()) * 100;
}

QLineF FPolygon::getLongestLine()
{
    QLineF longestLine(0,0,0,0);

    for(int ctr = 0; ctr <= this->polygon().size(); ++ctr)
    {
        QPolygonF poly = this->polygon();

        QPointF p1 = poly[ctr % poly.size()];
        qreal x1 = p1.x();
        qreal y1 = p1.y();

        QPointF p2 = poly[(ctr+1) % poly.size()];
        qreal x2 = p2.x();
        qreal y2 = p2.y();

        QLineF newLine(x1,y1,x2,y2);

        if(newLine.length() > longestLine.length())
        {
            longestLine = newLine;
        }
    }

    return longestLine;
}

void FPolygon::flip(bool flipX, bool flipY)
{
    qreal x = this->boundingRect().left();
    qreal y = this->boundingRect().top();

    int scaleX = 1;
    int scaleY = 1;

    QTransform trans;
    if(flipX == true)
        scaleX = -1;
    if(flipY == true)
        scaleY = -1;

    trans = trans.scale(scaleX, scaleY);

    QPolygonF flippedPolygon = trans.map(this->polygon());
    this->setPolygon(flippedPolygon);
    this->setFPosition(x, y);
}

} // END OF PolygonPacking NAMESPACE
