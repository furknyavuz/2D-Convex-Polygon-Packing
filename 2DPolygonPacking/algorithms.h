#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <QWidget>
#include <QGraphicsView>

#include "fpolygon.h"
#include "strip.h"
#include "issuedialog.h"

#include "ui_algorithms.h"

namespace PolygonPacking
{

/**
 * @class Algorithms
 *
 * @brief Class for graphitize the algorithms for
 * 2D polygon packing problem
 *
 * @author Furkan Yavuz
 */
class Algorithms : public QWidget
{
    Q_OBJECT
public:

    /**
     * @enum StripPackingAlgorithms
     * @brief A type of algorithm for,
     * 2d strip packing problem.
     */
    enum StripPackingAlgorithms
    {
        STRIP_PACKING_FIRST_FIT_DECREASING = 0,
        STRIP_PACKING_SPLIT_FIT,
        STRIP_PACKING_MAXIMUM_CONTACTING_PERIMETER
    };

    /**
     * @enum ShrinkingAlgorithms
     * @brief A type of algorithm for,
     * shrinking step of embedding and
     * shrinking algorithm.
     */
    enum ShrinkingAlgorithms
    {
        SHRINKING_WITHOUT_IMPROVEMENT = 0,
        SHRINKING_WITH_IMPROVEMENT
    };

    /**
     * Constructor sets the parent with given QWidget.
     * Takes size of strip and takes selected
     * algorithms list.
     *
     * @param parent parent widget
     * @param width width of strip
     * @param height height of strip
     * @param selectedHeuristicAlgorithms selected heuristic algorithms
     * @param selectedMetaHeuristicAlgorithms selected meta heuristic algorithms
     */
    explicit Algorithms(QWidget *parent, qreal width, qreal height, QList<int> selectedHeuristicAlgorithms, QList<int> selectedMetaHeuristicAlgorithms);

    /**
     * Default Destructor.
     */
    ~Algorithms();

    /**
     * Adds given polygons to strip using addPolygon function.
     *
     * @param polygonLis polygons to add
     */
    void addPolygonsToStrip(QList<QPolygonF> polygonList);

    /**
     * Embedding algorithm : Embeds polygons to minimum bounding rects
     * with given algorithm.
     *
     * @param algorithm type
     */
    void embed(FPolygon::RotateToMinBoundAlgorithms algorithm = FPolygon::ROTATE_TO_MIN_BOUND_SEARCH_WITH_ROTATE);

    /**
     * Strip Packing algorithm : 2D Strip packing problem's solving algorithms
     *
     * @param algorithm type
     */
    void stripPacking(Algorithms::StripPackingAlgorithms algorithm = Algorithms::STRIP_PACKING_FIRST_FIT_DECREASING);

    /**
     * Shrinking Algorithm : Shrink each polygons to left as possible.
     * For each polygon try 4 reflections of polygon. If move amount is low,
     * select the reflection that left side of polygon is more full.
     *
     * @param algorithm type
     */
    void shrinking(Algorithms::ShrinkingAlgorithms algorithm = Algorithms::SHRINKING_WITHOUT_IMPROVEMENT);

protected:

    /**
     * Shrinking With Improvement Algorithm : Shrink each polygons to left as possible.
     * For each polygon try 4 reflections of polygon, select the most
     * shrinked reflection
     */
    void shrinkingWithoutImprovement();

    /**
     * Shrinking Algorithm : Shrink each polygons to left as possible.
     * For each polygon try 4 reflections of polygon. If move amount is low,
     * select the reflection that left side of polygon is more full.
     */
    void shrinkingWithImprovement();

    /**
     * Clear strip and all polygons in it.
     */
    void clearStrip();

    /**
     * Adds given polygon to strip next to last polygon.
     *
     * @param polygon polygon to add
     *
     * @return True if polygon fits in to the strip, false if it is not
     */
    bool addPolygonToStrip(QPolygonF polygon);

    /**
     * First fit decreasing algorithm : Places the item that have maximum width first.
     */
    void firstFitDecreasing();

    /**
     * Split fit algorithm : Split items into two :
     * 1. Items height bigger than strip's height / 2
     * 2. Items height less or equal than strip's height / 2
     * Then place items in the first list using first fit decreasing.
     * Rearrange them :
     * 1.1. Items height bigger than strip's height * 2 / 3
     * 1.2. Items height less of equal than strip's height * 2 / 3
     * Items in 1.1. list moves bottom other goes up
     * Items in 2. list places using first fit decreasing next to the 1.2. list items
     * Items does not fit in 2. list places on top of the 1.2. list items using first fit decreasing
     */
    void splitFit();

    /**
     * Maximum Contacting Perimeter algorithm :
     * 1. Sort items by decreasing area
     * 2. For each item find all positions where left and top of item is not empty
     * 3. Calculate the contact length with other items or strip
     * 4. Select the maximum contacting perimeter
     */
    void maximumContactingPerimeter();

    /**
     * Sort polygons by width in decreasing order;
     */
    void sortByWidthDecreasing();

    /**
     * Sort polygons by area in decreasing order;
     */
    void sortByAreaDecreasing();

    /**
     * Sort polygons by top-left one first order;
     */
    void sortByTopLeft();

    /**
     * Checks given 2 polygons intersect or not.
     *
     * @param first first polygon to check intersection
     * @param second second polygon to check intersection
     *
     * @return True if polygons intesects, false if it is not
     */
    bool isIntersect(FPolygon * first, FPolygon * second);

    /**
     * Moves polygon to left as possible. If polygon hits to
     * polygon already placed or reach to left most of strip stops.
     *
     * @param fPolygon polygon to move left
     * @param placedNumberofPolygons number of already placed polygons
     *
     * @return Amount of move
     */
    int moveLeftAsPossible(FPolygon * fPolygon, int placedNumberofPolygons, int currentIndex);

    int moveTopAsPossible(FPolygon * fPolygon, int currentIndex);

    int moveBottomAsPossible(FPolygon * fPolygon, int currentIndex);

    /**
     * Moves polygon to left as possible. If polygon hits to
     * polygon already placed tries to move up or down sides and
     * moves to left as possible again
     *
     * @return Amount of move
     */
    int slideIn();

    /**
     * Algorithm tries to find nice shrinked
     * sub areas using heuristic algoritms with
     * small random changes. When finded suctracts
     * that area and run the algorithm for rest
     * polygons. After that all sub areas brings
     * together and shrinks.
     */
    void metaHeuristic();

    /**
     * Embeds polygons to minimum bounding rects
     * but lets random small rotations.
     */
    void shake();

    /**
     * Mouse wheel event for zoom in and out.
     *
     * @param event QWheelEvent for mouse wheel
     */
    void wheelEvent(QWheelEvent *event);

private slots:

    /**
     * Run Button clicked handler.
     */
    void on_runButton_clicked();

    /**
     * Rest Button clicked handler.
     */
    void on_resetButton_clicked();

    void on_algorithmComboBox_currentIndexChanged(int index);

private:

    /**
     * Zoom in or out from scene. If param
     * is positive zoom in else zoom out param times.
     *
     * @param steps number of zoom steps.
     */
    void zoom(int steps);

    /**
     * Set strip info. Drawing boundries of scene and
     * calculates strip's density
     */
    void setStripInfo();

    Ui::Algorithms *ui;
    Strip *strip;
    QList<FPolygon *> polygons;
    QList<QPolygonF> datas;
    QList<QColor> colors;
    QList<QGraphicsRectItem *> bounds;
    QList<QGraphicsTextItem *> texts;
    QGraphicsRectItem * sceneBound;
    QList<int> heuristicAlgorithms;
    QList<int> metaHeuristicAlgorithms;
    QMap<int,QColor> colorMap;
    IssueDialog * issue;
    QGraphicsView *view;
    QLabel * stripWidth;
    QLabel * stripHeight;
    QLabel * stripDensity;
    QComboBox * algorithmsComboBox;
    QPushButton * runButton;
    QString algorithmStepsString;
    QList<QPolygonF> backupPolygonList;
    QList<QList<QPolygonF> > partialPolygons;
    QList<int> partialWidths;
};

} // END OF PolygonPacking NAMESPACE
#endif // ALGORITHMS_H
