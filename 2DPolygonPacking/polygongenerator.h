#ifndef POLYGONGENERATOR_H
#define POLYGONGENERATOR_H

#include <QtGlobal>

#include <QTime>
#include <QWidget>
#include <QPointF>
#include <QGraphicsItem>
#include <QGraphicsView>

#include "fpolygon.h"

#include "ui_polygongenerator.h"

namespace PolygonPacking
{

/**
 * @class PolygonGenerator
 *
 * @brief Class for generating random convex polygons
 *
 * @author Furkan Yavuz
 */
class PolygonGenerator : public QWidget
{

    Q_OBJECT
    
public:

    /**
     * Constructor sets the parent with given QWidget.
     *
     * @param parent parent widget
     */
    explicit PolygonGenerator(QWidget *parent = 0);

    /**
     * Default Destructor.
     */
    ~PolygonGenerator();

    /**
     * Generates convex polygon that have given number of points.
     *
     * @param points number of points that polygon will have
     *
     * @return randomly generated convex polygon
     */
    QPolygonF generateConvexPolygon(int points);


    /**
     * Gets generated polygons
     *
     * @return List of generated polygons
     */
    QList<QPolygonF> getDatas();

private slots:
    void on_genbutton_clicked();

    void on_addToPolygonsButton_clicked();

    void on_polygonsList_clicked(const QModelIndex &index);

private:

    /**
     * Fill Polygon Info box with given polygon's info.
     *
     * @param polygon polygon that have info
     */
    void setPolygonInfo(FPolygon *polygon);

    /**
     * Generates random value between given params.
     *
     * @param min minimum value that can be generate
     *
     * @param max maximum value that can be generate
     *
     * @return random generated value
     */
    int randInt(int min, int max);

    Ui::PolygonGenerator *ui;
    QGraphicsScene *scene;
    QList<FPolygon *> polygons;
    QList<QPolygonF> datas;
    QList<QGraphicsRectItem *> bounds;
    FPolygon * genPolygon;
    QGraphicsRectItem * genBound;

};

} // END OF PolygonPacking NAMESPACE
#endif // POLYGONGENERATOR_H
