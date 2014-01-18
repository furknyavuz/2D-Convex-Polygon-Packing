#ifndef STRIP_H
#define STRIP_H

#include <QGraphicsScene>

namespace PolygonPacking
{

/**
 * @class Strip
 *
 * @brief Class for strip of 2D polygon packing problem
 *
 * @author Furkan Yavuz
 */
class Strip : public QGraphicsScene
{

public:

    /**
     * Takes width and height of strip.
     *
     * @param width width of strip
     * @param height height of strip
     */
    Strip(qreal width, qreal height);

    /**
     * Set width of strip.
     *
     * @param width new width of strip
     */
    void setWidth(qreal width);

    /**
     * Set height of strip.
     *
     * @param height new height of strip
     */
    void setHeight(qreal height);

    /**
     * Get width of strip.
     *
     * @return Width of strip
     */
    qreal getWidth();

    /**
     * Get height of strip.
     *
     * @return Height of strip
     */
    qreal getHeight();

    /**
     * Get area of strip.
     *
     * @return Area of strip
     */
    qreal area();
};

}

#endif // STRIP_H
