#ifndef FPOLYGON_H
#define FPOLYGON_H

#include <QtGui>
#include <QtCore>
#include <QtGlobal>

#include <QList>
#include <QPointF>
#include <QGraphicsPolygonItem>

namespace PolygonPacking
{

/**
 * @class FPolygon
 *
 * @brief Class that implements polygon functions
 * like rotate, area, bounding rect's area, local density
 *
 * @author Furkan Yavuz
 */
class FPolygon : public QGraphicsPolygonItem
{

public:

    /**
     * @enum RotateToMinBoundAlgorithms
     * @brief A type of algorithm for,
     * rotate polygon to polygon with minimum bounded rect.
     */
    enum RotateToMinBoundAlgorithms
    {
        ROTATE_TO_MIN_BOUND_SEARCH_WITH_ROTATE = 0,
        ROTATE_TO_MIN_BOUND_PARALLEL_EDGE
    };

    /**
     * Default Constructor.
     */
    FPolygon();

    /**
     * Rotate polygon clockwise with given degree param.
     *
     * @param degree Rotation degree
     */
    void rotateFPolygon(qreal degree);

    /**
     * Rotate polygon to polygon with minimum bounded rect with given algorithm.
     *
     * @return Degree of rotation
     */
    qreal rotateToMinBound(RotateToMinBoundAlgorithms algorithm = ROTATE_TO_MIN_BOUND_SEARCH_WITH_ROTATE);

    /**
     * Returns the area of polygon.
     *
     * @return Area of polygon
     */
    qreal area();

    /**
     * Returns the area of bounding rect.
     *
     * @return Area of bounding rect
     */
    qreal boundArea();

    /**
     * Sets bounding rect's top-left position with given x and y params.
     *
     * @param x Bounding rect's new left position
     * @param y Bounding rect's new top position
     */
    void setFPosition(qreal x, qreal y);

    /**
     * Returns the area of polygon / area of bounding rect ratio with 100 percent format.
     *
     * @return Area of polygon / area of bounding rect * 100 ratio
     */
    qreal localDensity();

    /**
     * Returns the longest line of polygon.
     *
     * @return Longest line of polygon
     */
    QLineF getLongestLine();

    /**
     * Flips the polygon x direction, y direction
     * or both direction.
     *
     * @param flipX true will flip in x direction
     * @param flipY true will flip in y direction
     */
    void flip(bool flipX, bool flipY);

protected:

    /**
     * Rotate polygon to polygon with minimum bounded rect with rotate 1 degree each time.
     *
     * @return Degree of rotation
     */
    qreal rotateToMinBoundSearchWithRotate();

    /**
     * Rotate polygon to polygon with minimum bounded rect with paralel edge algorithm.
     *
     * @return Degree of rotation
     */
    qreal rotateToMinBoundParallelEdge();
};

} // END OF PolygonPacking NAMESPACE
#endif // FPOLYGON_H
