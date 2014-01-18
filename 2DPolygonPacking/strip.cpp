#include "strip.h"

namespace PolygonPacking
{

Strip::Strip(qreal width, qreal height)
{
    this->setSceneRect(0,0,width,height);
}

void Strip::setWidth(qreal width)
{
    qreal height = this->sceneRect().height();

    this->setSceneRect(0,0,width, height);
}

void Strip::setHeight(qreal height)
{
    qreal width = this->sceneRect().width();

    this->setSceneRect(0,0,width, height);
}

qreal Strip::getWidth()
{
    return this->sceneRect().width();
}

qreal Strip::getHeight()
{
    return this->sceneRect().height();
}

qreal Strip::area()
{
    return this->sceneRect().height() * this->sceneRect().width();
}

}
