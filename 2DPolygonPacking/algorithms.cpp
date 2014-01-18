#include "algorithms.h"
#include <iostream>

#define ZOOM_OUT_RATIO 0.95
#define ZOOM_IN_RATIO 1.15

#define INITIAL_TEMP 75
#define RANDOMIZE_ANGLE 40

namespace PolygonPacking
{

Algorithms::Algorithms(QWidget *parent, qreal width, qreal height, QList<int> selectedHeuristicAlgorithms, QList<int> selectedMetaHeuristicAlgorithms) :
    QWidget(parent),
    ui(new Ui::Algorithms)
{
    QPen redPen(Qt::red);
    ui->setupUi(this);
    ui->resetButton->setDisabled(true);

    strip = new Strip(width,height);
    heuristicAlgorithms = selectedHeuristicAlgorithms;
    metaHeuristicAlgorithms = selectedMetaHeuristicAlgorithms;

    QRectF boundRect = strip->sceneRect();
    sceneBound = strip->addRect(boundRect,redPen,Qt::NoBrush);

    view = ui->graphicsView;
    view->setScene(strip);

    stripWidth = ui->stripWidthLabel;
    stripWidth->setText(QString::number(width));

    stripHeight = ui->stripHeightLabel;
    stripHeight->setText(QString::number(height));

    stripDensity = ui->stripDensityLabel;
    stripDensity->setText("0%");

    algorithmsComboBox = ui->algorithmComboBox;
    algorithmsComboBox->addItem("Heuristic");
    algorithmsComboBox->addItem("Meta Heuristic");

    runButton = ui->runButton;
    runButton->setDisabled(true);

    colorMap.insert(0, QColor::QColor(238, 203, 173));
    colorMap.insert(1, QColor::QColor(193, 205, 193));
    colorMap.insert(2, QColor::QColor(135, 206, 250));
    colorMap.insert(3, QColor::QColor(152, 251, 152));
    colorMap.insert(4, QColor::QColor(255, 215,   0));
    colorMap.insert(5, QColor::QColor(205,  92,  92));
    colorMap.insert(6, QColor::QColor(250, 128, 114));
    colorMap.insert(7, QColor::QColor(221, 160, 221));
    colorMap.insert(8, QColor::QColor( 32, 178, 170));
    colorMap.insert(9, QColor::QColor(100, 149, 237));
    colorMap.insert(10, QColor::QColor(0, 191, 250));
    colorMap.insert(11, QColor::QColor(173, 255, 47));
    colorMap.insert(12, QColor::QColor(244, 164, 96));
    colorMap.insert(13, QColor::QColor(255, 69, 0));
    colorMap.insert(14, QColor::QColor(255, 105, 180));
    colorMap.insert(15, QColor::QColor(205, 42, 42));
    colorMap.insert(16, QColor::QColor(34, 175, 34));

    algorithmStepsString = "Algorithm Steps : ";
    ui->algorithmStepsLabel->setText(algorithmStepsString);

    switch(heuristicAlgorithms[0])
    {
    case FPolygon::ROTATE_TO_MIN_BOUND_SEARCH_WITH_ROTATE :
        ui->algorithmStepsLabel->setText(ui->algorithmStepsLabel->text() + "Embed (Search With Rotate) ->");
        break;
    case FPolygon::ROTATE_TO_MIN_BOUND_PARALLEL_EDGE :
        ui->algorithmStepsLabel->setText(ui->algorithmStepsLabel->text() + "Embed (Parallel Edge) ->");
        break;
    default :
        break;
    }
    switch(heuristicAlgorithms[1])
    {
    case STRIP_PACKING_FIRST_FIT_DECREASING :
        ui->algorithmStepsLabel->setText(ui->algorithmStepsLabel->text() + "Strip Pack (First Fit) ->");
        break;
    case STRIP_PACKING_SPLIT_FIT :
        ui->algorithmStepsLabel->setText(ui->algorithmStepsLabel->text() + "Strip Pack (Split Fit) ->");
        break;
    case STRIP_PACKING_MAXIMUM_CONTACTING_PERIMETER :
        ui->algorithmStepsLabel->setText(ui->algorithmStepsLabel->text() + "Strip Pack (Maximum Contacting Perimeter) ->");
        break;
    default :
        break;
    }
    switch(heuristicAlgorithms[2])
    {
    case SHRINKING_WITHOUT_IMPROVEMENT :
        ui->algorithmStepsLabel->setText(ui->algorithmStepsLabel->text() + "Shrink (Without Improvement)");
        break;
    case SHRINKING_WITH_IMPROVEMENT :
        ui->algorithmStepsLabel->setText(ui->algorithmStepsLabel->text() + "Shrink (With Improvement)");
        break;
    default :
        break;
    }
}

Algorithms::~Algorithms()
{
    delete ui;
}

void Algorithms::clearStrip()
{
    strip->clear();
    polygons.clear();
    bounds.clear();
    datas.clear();
    colors.clear();

    setStripInfo();
}

bool Algorithms::addPolygonToStrip(QPolygonF polygon)
{
    int value = qrand() % colorMap.size();

    QColor randomColor = colorMap.value(value);
    QBrush passtelBrush(randomColor);

    QPen redPen(Qt::red);
    QPen greyPen(Qt::gray);
    QFont miniFont("Arial",11,1);

    polygons.append((FPolygon *) strip->addPolygon(polygon, greyPen, passtelBrush));
    FPolygon * fPolygon = polygons.last();
    if(fPolygon->boundingRect().height() > strip->height())
    {
        strip->removeItem(polygons.last());
        polygons.removeLast();
        return false;
    }
    strip->removeItem(sceneBound);
    colors.append(randomColor);

    int size = polygons.size();

    if(size == 1)
    {
        fPolygon->setFPosition(0,0);
    }
    else
    {
        qreal lastX = bounds[size-2]->rect().right();
        fPolygon->setFPosition(lastX, 0);
    }

    datas.append(fPolygon->polygon());

    bounds.append(strip->addRect(fPolygon->boundingRect(), redPen, Qt::NoBrush));
    QGraphicsRectItem * bound = bounds.last();
    bound->setParentItem(fPolygon);

    texts.append(strip->addText(QString::number(fPolygon->boundArea()),miniFont));
    QGraphicsTextItem * text = texts.last();
    text->setDefaultTextColor(Qt::red);
    text->setPos(bound->rect().left(), bound->rect().bottom() + 5);
    text->setParentItem(fPolygon);

    strip->setWidth(bound->rect().right());
    setStripInfo();
    return true;
}

void Algorithms::addPolygonsToStrip(QList<QPolygonF> polygonList)
{
    bool issueExists = false;
    QList<QPolygonF> tempList;
    for(int ctr = 0; ctr < polygonList.size(); ++ctr)
    {
        tempList.append(polygonList[ctr]);
        if(!addPolygonToStrip(polygonList[ctr]))
            issueExists = true;
    }
    if(issueExists)
    {
        issue = new IssueDialog(this,"Polygon Fit Problem","One or more polygons does not fit into the strip!", "Unfitting polygons skipped.");
        issue->exec();
    }
    if(polygons.size() == 0)
    {
        runButton->setDisabled(true);
    }
    else
    {
        runButton->setEnabled(true);
    }
    backupPolygonList = tempList;

}

void Algorithms::embed(FPolygon::RotateToMinBoundAlgorithms algorithm)
{
    strip->clear();

    QPen redPen(Qt::red);
    QPen greyPen(Qt::gray);
    QFont miniFont("Arial",11,1);

    bool issueExists = false;

    qreal lastX;

    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        QBrush passtelBrush(colors.at(ctr));
        QPolygonF polygon(datas.at(ctr));

        polygons[ctr] = (FPolygon *) strip->addPolygon(polygon, greyPen, passtelBrush);
        FPolygon * fPolygon = polygons[ctr];

        qreal degree = fPolygon->rotateToMinBound(algorithm);

        if(fPolygon->boundingRect().height() > strip->height())
        {
            fPolygon->rotateFPolygon(90);
            if(fPolygon->boundingRect().height() > strip->height())
            {
                issueExists = true;
                fPolygon->rotateFPolygon(-90);
                fPolygon->rotateFPolygon(-degree);
            }
        }

        if(ctr == 0)
        {
            fPolygon->setFPosition(0, 0);
        }
        else
        {
            lastX = bounds[ctr - 1]->rect().right();
            fPolygon->setFPosition(lastX, 0);
        }

        bounds[ctr] = strip->addRect(fPolygon->boundingRect(), redPen, Qt::NoBrush);
        QGraphicsRectItem * bound = bounds[ctr];
        bound->setParentItem(fPolygon);

        texts[ctr] = strip->addText(QString::number(fPolygon->boundArea()),miniFont);
        QGraphicsTextItem * text = texts[ctr];
        text->setDefaultTextColor(Qt::red);
        text->setPos(bound->rect().left(), bound->rect().bottom() + 5);
        text->setParentItem(fPolygon);
    }
    if(issueExists)
    {
        issue = new IssueDialog(this,"Polygon Fit Problem","One or more polygons does not fit into the strip!", "Unfitting polygons skipped.");
        issue->exec();
    }

    strip->setWidth(bounds.last()->rect().right());
    setStripInfo();
}

void Algorithms::stripPacking(Algorithms::StripPackingAlgorithms algorithm)
{
    switch (algorithm)
    {
    case STRIP_PACKING_FIRST_FIT_DECREASING :
        firstFitDecreasing();
        break;
    case STRIP_PACKING_SPLIT_FIT :
        splitFit();
        break;
    case STRIP_PACKING_MAXIMUM_CONTACTING_PERIMETER :
        maximumContactingPerimeter();
        break;
    default :
        firstFitDecreasing();
        break;
    }
}

void Algorithms::shrinking(Algorithms::ShrinkingAlgorithms algorithm)
{
    switch (algorithm)
    {
    case SHRINKING_WITHOUT_IMPROVEMENT :
        shrinkingWithoutImprovement();
        break;
    case SHRINKING_WITH_IMPROVEMENT :
        shrinkingWithImprovement();
        break;
    default :
        shrinkingWithoutImprovement();
        break;
    }
}

void Algorithms::firstFitDecreasing()
{
    strip->removeItem(sceneBound);

    sortByWidthDecreasing();

    QList<QRectF> levels;

    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        FPolygon * fPolygon = polygons[ctr];
        fPolygon->setFlag(QGraphicsItem::ItemIsMovable,true);
        QRectF bound = bounds[ctr]->rect();
        if(ctr == 0)
        {
            qreal height = strip->getHeight();
            fPolygon->setFPosition(0,0);
            bound = fPolygon->boundingRect();
            levels.append(QRectF::QRectF(0, bound.bottom(), bound.width(), height - bound.height()+1));
        }
        else
        {
            bool isFit = false;
            for(int ctr2 = 0; ctr2 < levels.size(); ++ctr2)
            {
                if(bound.height() <= levels[ctr2].height())
                {
                    fPolygon->setFPosition(levels[ctr2].left(), levels[ctr2].top());
                    bound = fPolygon->boundingRect();
                    levels[ctr2].setTop(levels[ctr2].top() + bound.height());
                    isFit = true;
                    break;
                }
            }

            if(!isFit)
            {
                qreal height = strip->getHeight();
                fPolygon->setFPosition(levels.last().right(), 0);
                bound = fPolygon->boundingRect();
                levels.append(QRectF::QRectF(levels.last().right(), bound.bottom(), bound.width(), height - bound.height()+1));
            }
        }
        strip->removeItem(bounds[ctr]);
    }

    qreal maxBound = polygons[0]->boundingRect().right();
    int maxIndex = 0;
    for(int ctr = 1; ctr < polygons.size(); ++ctr)
    {
        qreal bound = polygons[ctr]->boundingRect().right();
        if(bound > maxBound)
        {
            maxBound = bound;
            maxIndex = ctr;
        }
    }

    strip->setWidth(polygons[maxIndex]->boundingRect().right());
    setStripInfo();
}

void Algorithms::splitFit()
{
    strip->removeItem(sceneBound);

    sortByWidthDecreasing();

    QList<FPolygon *> biggerThan23;
    QList<FPolygon *> biggerThan12;
    QList<FPolygon *> smallerThan12;

    qreal strip23 = strip->height() * 2 / 3;
    qreal strip12 = strip->height() / 2;
    qreal strip13 = strip->height() / 3;

    QList<qreal> splits;
    splits.append(0);
    splits.append(0);
    splits.append(0);
    splits.append(0);

    // Split
    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        FPolygon * fPolygon = polygons[ctr];
        fPolygon->setFlag(QGraphicsItem::ItemIsMovable,true);
        qreal height = fPolygon->boundingRect().height();

        if(height > strip23)
        {
            biggerThan23.append(fPolygon);
        }
        else if(height > strip12)
        {
            biggerThan12.append(fPolygon);
        }
        else
        {
            smallerThan12.append(fPolygon);
        }
        strip->removeItem(bounds[ctr]);
    }

    // w > strip's w * 2 / 3
    for(int ctr = 0; ctr < biggerThan23.size(); ++ctr)
    {
        if(ctr == 0)
        {
            biggerThan23[ctr]->setFPosition(0,0);
        }
        else
        {
            biggerThan23[ctr]->setFPosition(biggerThan23[ctr-1]->boundingRect().right(),0);
        }
        splits[0] = biggerThan23[ctr]->boundingRect().right();
    }

    splits[1] = splits[0];
    // w > strip's w / 2
    for(int ctr = 0; ctr < biggerThan12.size(); ++ctr)
    {
        if(ctr == 0)
        {
            biggerThan12[ctr]->setFPosition(splits[0],0);
        }
        else
        {
            biggerThan12[ctr]->setFPosition(biggerThan12[ctr-1]->boundingRect().right(),0);
        }
        splits[1] = biggerThan12[ctr]->boundingRect().right();
    }

    splits[2] = splits[1];

    QRectF rRegion(QRectF::QRectF(splits[0], strip23, splits[1] - splits[0], strip13));
    QList<QRectF> rLevels;
    QList<QRectF> levels;
    qreal leftWidth = rRegion.width();

    // w < strip's w / 2
    for(int ctr = 0; ctr < smallerThan12.size(); ++ctr)
    {
        FPolygon * polygon = smallerThan12[ctr];
        if(ctr == 0)
        {
            if(rRegion.height() >= polygon->boundingRect().height() &&
                    rRegion.width() >= polygon->boundingRect().width())
            {
                qreal height = rRegion.height();
                qreal left = rRegion.left();
                polygon->setFPosition(left, rRegion.top());
                rLevels.append(QRectF::QRectF(left, polygon->boundingRect().bottom(), polygon->boundingRect().width(), height - polygon->boundingRect().height()));
                leftWidth -= rLevels.last().width();
            }
            else
            {
                qreal height = strip->height();
                qreal left = splits[1];
                polygon->setFPosition(left, 0);
                levels.append(QRectF::QRectF(left, polygon->boundingRect().bottom(), polygon->boundingRect().width(), height - polygon->boundingRect().height()));
                splits[2] = levels.last().right();
            }
        }
        else
        {
            bool isFit = false;
            for(int ctr2 = 0; ctr2 < rLevels.size(); ++ctr2)
            {
                if(polygon->boundingRect().height() <= rLevels[ctr2].height())
                {
                    polygon->setFPosition(rLevels[ctr2].left(), rLevels[ctr2].top());
                    rLevels[ctr2].setTop(rLevels[ctr2].top() + polygon->boundingRect().height());
                    isFit = true;
                    break;
                }
            }
            if(!isFit)
            {
                if(rRegion.height() >= polygon->boundingRect().height()+1 &&
                        polygon->boundingRect().width() < leftWidth)
                {
                    qreal height = rRegion.height();
                    qreal left = rRegion.left() + rRegion.width() - leftWidth;

                    polygon->setFPosition(left, rRegion.top());
                    rLevels.append(QRectF::QRectF(left, polygon->boundingRect().bottom(), polygon->boundingRect().width(), height - polygon->boundingRect().height()));
                    leftWidth -= rLevels.last().width();
                }
                else
                {
                    bool isFit2 = false;
                    for(int ctr2 = 0; ctr2 < levels.size(); ++ctr2)
                    {
                        if(polygon->boundingRect().height() <= levels[ctr2].height()+1)
                        {
                            polygon->setFPosition(levels[ctr2].left(), levels[ctr2].top());
                            levels[ctr2].setTop(levels[ctr2].top() + polygon->boundingRect().height());
                            isFit2 = true;
                            break;
                        }
                    }
                    if(!isFit2)
                    {
                        qreal height = strip->height();
                        qreal left = splits[2];
                        polygon->setFPosition(left, 0);
                        levels.append(QRectF::QRectF(left, polygon->boundingRect().bottom(), polygon->boundingRect().width(), height - polygon->boundingRect().height()));
                        splits[2] = levels.last().right();
                    }
                }
            }
        }
    }

    splits[3] = splits[2];

    if(levels.size() > 0)
        splits[3] = levels.last().right();

    strip->setWidth(splits[3]);

    setStripInfo();
}

void Algorithms::maximumContactingPerimeter()
{
    strip->removeItem(sceneBound);

    sortByAreaDecreasing();

    QList<QLineF> hLines;
    hLines.append(QLineF::QLineF(0,0,100000,0));
    QList<QLineF> vLines;
    vLines.append(QLineF::QLineF(0,0,0, strip->height()));

    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        FPolygon * fPolygon = polygons[ctr];
        if(ctr == 0)
        {
            fPolygon->setFPosition(hLines[0].x1(), vLines[0].y1());
        }
        else
        {
            QList<QPointF> acceptedPoints;
            QList<int> hLineIndexes;
            QList<int> vLineIndexes;
            QList<int> scores;

            for(int ctrV = 0; ctrV < vLines.size(); ++ctrV)
            {
                bool isIntersect = false;
                for(int ctrH = 0; ctrH < hLines.size(); ++ctrH)
                {
                    QPointF * intersectPoint = new QPointF();
                    if(hLines[ctrH].intersect(vLines[ctrV],intersectPoint) == QLineF::NoIntersection)
                        continue;
                    if(intersectPoint->y() > vLines[ctrV].y1() + 25 && intersectPoint->x() > hLines[ctrH].x1() + 25)
                        continue;
                    isIntersect = false;
                    int xPos = intersectPoint->x();
                    int yPos = intersectPoint->y();
                    fPolygon->setFPosition(xPos, yPos);

                    for(int ctr2 = 0; ctr2 < ctr; ++ctr2)
                    {
                        QRectF region = fPolygon->boundingRect().intersect(polygons[ctr2]->boundingRect());
                        if(region.width() > 1 && region.height() > 1)
                        {
                            isIntersect = true;
                            break;
                        }
                    }
                    if(fPolygon->boundingRect().bottom() > strip->getHeight())
                        isIntersect = true;
                    if(!isIntersect)
                    {
                        qreal left = fPolygon->boundingRect().left();
                        qreal right = fPolygon->boundingRect().right();
                        qreal top = fPolygon->boundingRect().top();
                        qreal bottom = fPolygon->boundingRect().bottom();

                        QLineF polyTopLine(left, top, right, top);
                        QLineF polyLeftLine(left, top, left, bottom);

                        qreal x1 = xPos;
                        qreal x2 = hLines[ctrH].x2();
                        if(polyTopLine.x1() > x1)
                            x1 = polyTopLine.x1();
                        if(polyTopLine.x2() < x2)
                            x2 = polyTopLine.x2();
                        qreal y1 = yPos;
                        qreal y2 = vLines[ctrV].y2();
                        if(polyLeftLine.y1() > y1)
                            y1 = polyLeftLine.y1();
                        if(polyLeftLine.y2() < y2)
                            y2 = polyLeftLine.y2();

                        int topLength = x2 - x1;
                        int leftLength = y2 - y1;

/*                        if(ctrH != 0)
                            topLength += top;*/

                        leftLength += left;

                        if(topLength < polyTopLine.length() / 5)
                            continue;
                        if(leftLength < polyLeftLine.length() / 5)
                            continue;
                        acceptedPoints.append(QPointF::QPointF(xPos, yPos));
                        hLineIndexes.append(ctrH);
                        vLineIndexes.append(ctrV);

                        if(ctrH != 0)
                            topLength += yPos * 100;
                        leftLength -= xPos * 100;

                        scores.append(topLength + leftLength /*+ rightLength + bottomLength*/);
                    }
                }
            }
            if(acceptedPoints.size() == 0)
            {
                qreal maxBound = 0;
                for(int ctr2 = 0; ctr2 < ctr; ++ctr2)
                {
                    if(polygons[ctr2]->boundingRect().right() > maxBound)
                        maxBound = polygons[ctr2]->boundingRect().right();
                }
                fPolygon->setFPosition(maxBound,0);
            } else {
                int maxScore = 0;
                int maxIndex = 0;
                for(int ctr2 = 0; ctr2 < scores.size(); ++ctr2)
                {
                    if(scores[ctr2] > maxScore)
                    {
                        maxScore = scores[ctr2];
                        maxIndex = ctr2;
                    }
                }
                fPolygon->setFPosition(acceptedPoints[maxIndex].x(), acceptedPoints[maxIndex].y());
                qreal left = fPolygon->boundingRect().left();
                qreal right = fPolygon->boundingRect().right();
                qreal top = fPolygon->boundingRect().top();
                qreal bottom = fPolygon->boundingRect().bottom();

                QLineF hLine(left, bottom, right, bottom);
                QLineF vLine(right, top, right, bottom);
                if(hLine.length() >= hLines[hLineIndexes[maxIndex]].length())
                    hLines.removeAt(hLineIndexes[maxIndex]);
                if(vLine.length() >= vLines[vLineIndexes[maxIndex]].length())
                    vLines.removeAt(vLineIndexes[maxIndex]);
            }
        }

        qreal left = fPolygon->boundingRect().left();
        qreal right = fPolygon->boundingRect().right();
        qreal top = fPolygon->boundingRect().top();
        qreal bottom = fPolygon->boundingRect().bottom();

        QLineF hLine(left, bottom, right, bottom);
        QLineF vLine(right, top, right, bottom);

        hLines.append(hLine);
        vLines.append(vLine);
        fPolygon->setFlag(QGraphicsItem::ItemIsMovable,true);
    }

    qreal maxBound = polygons[0]->boundingRect().right();
    int maxIndex = 0;
    for(int ctr = 1; ctr < polygons.size(); ++ctr)
    {
        qreal bound = polygons[ctr]->boundingRect().right();
        if(bound > maxBound)
        {
            maxBound = bound;
            maxIndex = ctr;
        }
    }

    strip->setWidth(polygons[maxIndex]->boundingRect().right());
    setStripInfo();
}

void Algorithms::shrinkingWithoutImprovement()
{
    strip->removeItem(sceneBound);

    sortByTopLeft();
    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        FPolygon * fPolygon = polygons[ctr];

        QList<int> shrinkAmounts;
        shrinkAmounts.append(0);
        shrinkAmounts.append(0);
        shrinkAmounts.append(0);
        shrinkAmounts.append(0);

        qreal oldY = fPolygon->boundingRect().top();
        qreal oldX = fPolygon->boundingRect().left();
        shrinkAmounts[0] = moveLeftAsPossible(fPolygon, ctr, ctr);
        fPolygon->setFPosition(oldX, oldY);

        fPolygon->flip(true,false);
        shrinkAmounts[1] = moveLeftAsPossible(fPolygon, ctr, ctr);
        fPolygon->setFPosition(oldX, oldY);
        fPolygon->flip(true,false);

        fPolygon->flip(true,true);
        shrinkAmounts[2] = moveLeftAsPossible(fPolygon, ctr, ctr);
        fPolygon->setFPosition(oldX, oldY);
        fPolygon->flip(true,true);

        fPolygon->flip(false,true);
        shrinkAmounts[3] = moveLeftAsPossible(fPolygon, ctr, ctr);
        fPolygon->setFPosition(oldX, oldY);
        fPolygon->flip(false,true);

        int maxIndex = 0;
        int maxShift = shrinkAmounts[0];
        for(int ctr2 = 1; ctr2 < 4; ++ctr2)
        {
            if(shrinkAmounts[ctr2] > maxShift)
            {
                maxIndex = ctr2;
                maxShift = shrinkAmounts[ctr2];
            }
        }

        fPolygon->setFPosition(fPolygon->boundingRect().left()-maxShift, fPolygon->boundingRect().top());

        switch(maxIndex)
        {
        case 0:
            break;
        case 1:
            fPolygon->flip(true,false);
            break;
        case 2:
            fPolygon->flip(true,true);
            break;
        case 3:
            fPolygon->flip(false,true);
            break;
        default :
            break;
        }
    }

    qreal maxBound = polygons[0]->boundingRect().right();
    int maxIndex = 0;
    for(int ctr = 1; ctr < polygons.size(); ++ctr)
    {
        qreal bound = polygons[ctr]->boundingRect().right();
        if(bound > maxBound)
        {
            maxBound = bound;
            maxIndex = ctr;
        }
    }

    strip->setWidth(polygons[maxIndex]->boundingRect().right());
    setStripInfo();
}

void Algorithms::shrinkingWithImprovement()
{
    strip->removeItem(sceneBound);

    sortByTopLeft();
    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        FPolygon * fPolygon = polygons[ctr];

        qreal maxBound = 0;
        for(int ctr2 = 0; ctr2 < ctr; ++ctr2)
        {
            qreal bound = polygons[ctr2]->boundingRect().right();
            if(bound > maxBound)
                maxBound = bound;
        }

        if(fPolygon->boundingRect().left() > maxBound)
            fPolygon->setFPosition(maxBound+1,fPolygon->boundingRect().top());

        QList<int> shrinkAmounts;
        QList<int> shrinkArea;

        qreal density = 0;

        qreal oldX = fPolygon->boundingRect().left();
        qreal oldY = fPolygon->boundingRect().top();

        shrinkAmounts.append(moveLeftAsPossible(fPolygon, ctr, ctr));
        QRectF leftRect(fPolygon->boundingRect().left(),fPolygon->boundingRect().top(),shrinkAmounts.last() + fPolygon->boundingRect().width() / 2, fPolygon->boundingRect().height());
        QPolygonF leftPolygon(leftRect);
        QPolygonF intersectedPolygon = fPolygon->polygon().intersected(leftPolygon);
        FPolygon fIntersectedPolygon;
        fIntersectedPolygon.setPolygon(intersectedPolygon);
        density = fIntersectedPolygon.area();
        shrinkArea.append(density);
        fPolygon->setFPosition(oldX, oldY);

        fPolygon->flip(true,false);
        shrinkAmounts.append(moveLeftAsPossible(fPolygon, ctr, ctr));
        leftRect = QRectF(fPolygon->boundingRect().left(),fPolygon->boundingRect().top(),shrinkAmounts.last() + fPolygon->boundingRect().width() / 2, fPolygon->boundingRect().height());
        leftPolygon = QPolygonF(leftRect);
        intersectedPolygon = fPolygon->polygon().intersected(leftPolygon);
        fIntersectedPolygon.setPolygon(intersectedPolygon);
        density = fIntersectedPolygon.area();
        shrinkArea.append(density);
        fPolygon->setFPosition(oldX, oldY);
        fPolygon->flip(true,false);

        fPolygon->flip(true,true);
        shrinkAmounts.append(moveLeftAsPossible(fPolygon, ctr, ctr));
        leftRect = QRectF(fPolygon->boundingRect().left(),fPolygon->boundingRect().top(),shrinkAmounts.last() + fPolygon->boundingRect().width() / 2, fPolygon->boundingRect().height());
        leftPolygon = QPolygonF(leftRect);
        intersectedPolygon = fPolygon->polygon().intersected(leftPolygon);
        fIntersectedPolygon.setPolygon(intersectedPolygon);
        density = fIntersectedPolygon.area();
        shrinkArea.append(density);
        fPolygon->setFPosition(oldX, oldY);
        fPolygon->flip(true,true);

        fPolygon->flip(false,true);
        shrinkAmounts.append(moveLeftAsPossible(fPolygon, ctr, ctr));
        leftRect = QRectF(fPolygon->boundingRect().left(),fPolygon->boundingRect().top(),shrinkAmounts.last() + fPolygon->boundingRect().width() / 2, fPolygon->boundingRect().height());
        leftPolygon = QPolygonF(leftRect);
        intersectedPolygon = fPolygon->polygon().intersected(leftPolygon);
        fIntersectedPolygon.setPolygon(intersectedPolygon);
        density = fIntersectedPolygon.area();
        shrinkArea.append(density);
        fPolygon->setFPosition(oldX, oldY);
        fPolygon->flip(false,true);

        int maxIndex = 0;
        int maxArea = shrinkArea[0];
        int maxShift = shrinkAmounts[0];
        for(int ctr2 = 1; ctr2 < 4; ++ctr2)
        {
            if(shrinkArea[ctr2] > maxArea)
            {
                maxIndex = ctr2;
                maxArea = shrinkArea[ctr2];
                maxShift = shrinkAmounts[ctr2];
            }
            else if(shrinkArea[ctr2] == maxArea)
            {
                if(shrinkAmounts[ctr2] > maxShift)
                {
                    maxIndex = ctr2;
                    maxArea = shrinkArea[ctr2];
                    maxShift = shrinkAmounts[ctr2];
                }
            }
        }

        fPolygon->setFPosition(fPolygon->boundingRect().left()-maxShift, fPolygon->boundingRect().top());

        switch(maxIndex)
        {
        case 0:
            break;
        case 1:
            fPolygon->flip(true,false);
            break;
        case 2:
            fPolygon->flip(true,true);
            break;
        case 3:
            fPolygon->flip(false,true);
            break;
        default :
            break;
        }
    }

    slideIn();
    slideIn();
    slideIn();

    qreal maxBound = polygons[0]->boundingRect().right();
    int maxIndex = 0;
    for(int ctr = 1; ctr < polygons.size(); ++ctr)
    {
        qreal bound = polygons[ctr]->boundingRect().right();
        if(bound > maxBound)
        {
            maxBound = bound;
            maxIndex = ctr;
        }
    }

    strip->setWidth(polygons[maxIndex]->boundingRect().right());
    setStripInfo();
}

void Algorithms::sortByWidthDecreasing()
{
    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        qreal maxWidth = polygons[ctr]->boundingRect().width();
        int maxIndex = ctr;
        for(int ctr2 = ctr + 1; ctr2 < polygons.size(); ++ctr2)
        {
            qreal width = polygons[ctr2]->boundingRect().width();
            if(width > maxWidth)
            {
                maxWidth = width;
                maxIndex = ctr2;
            }
        }
        FPolygon * temp = polygons[ctr];
        polygons[ctr] = polygons[maxIndex];
        polygons[maxIndex] = temp;

        QGraphicsRectItem *tempBound = bounds[ctr];
        bounds[ctr] = bounds[maxIndex];
        bounds[maxIndex] = tempBound;

        strip->removeItem(texts[ctr]);
    }
    texts.clear();
}

void Algorithms::sortByAreaDecreasing()
{
    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        qreal maxArea = polygons[ctr]->boundArea();
        qreal maxWidth = polygons[ctr]->boundingRect().width();
        qreal maxHeight = polygons[ctr]->boundingRect().height();
        int maxIndex = ctr;
        for(int ctr2 = ctr + 1; ctr2 < polygons.size(); ++ctr2)
        {
            qreal area = polygons[ctr2]->boundArea();
            qreal width = polygons[ctr2]->boundingRect().width();
            qreal height = polygons[ctr2]->boundingRect().height();
            if(area > maxArea)
            {
                maxArea = area;
                maxWidth = width;
                maxHeight = height;
                maxIndex = ctr2;
            }
            if(area == maxArea)
            {
                qreal diff1 = qAbs((width - height) / area);
                qreal diff2 = qAbs((maxWidth - maxHeight) / maxArea);
                if(diff1 < diff2)
                {
                    maxArea = area;
                    maxWidth = width;
                    maxHeight = height;
                    maxIndex = ctr2;
                }
            }
        }
        FPolygon * temp = polygons[ctr];
        polygons[ctr] = polygons[maxIndex];
        polygons[maxIndex] = temp;

        strip->removeItem(bounds[ctr]);
        strip->removeItem(texts[ctr]);
    }
    bounds.clear();
    texts.clear();
}

void Algorithms::sortByTopLeft()
{
    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        int minY = polygons[ctr]->boundingRect().top();
        int minX = polygons[ctr]->boundingRect().left();
        int maxIndex = ctr;
        for(int ctr2 = ctr + 1; ctr2 < polygons.size(); ++ctr2)
        {
            int top = polygons[ctr2]->boundingRect().top();
            int left = polygons[ctr2]->boundingRect().left();
            if(left < minX && top < minY)
            {
                minX = left;
                minY = top;
                maxIndex = ctr2;
            }
            else if(left < minX)
            {
                minX = left;
                minY = top;
                maxIndex = ctr2;
            }
            else if(left == minX)
            {
                if(top < minY)
                {
                    minX = left;
                    minY = top;
                    maxIndex = ctr2;
                }
            }
        }
        FPolygon * temp = polygons[ctr];
        polygons[ctr] = polygons[maxIndex];
        polygons[maxIndex] = temp;
    }
}

bool Algorithms::isIntersect(FPolygon *first, FPolygon *second)
{
    for(int ctr = 0; ctr < first->polygon().size(); ++ctr)
    {
        QLineF * firstEdge = new QLineF(first->polygon()[ctr], first->polygon()[(ctr + 1) % first->polygon().size()]);
        for(int ctr2 = 0; ctr2 < second->polygon().size(); ++ctr2)
        {
            QLineF * secondEdge = new QLineF(second->polygon()[ctr2], second->polygon()[(ctr2 + 1) % second->polygon().size()]);
            QPointF * intersectionPoint = new QPointF();
            if(firstEdge->intersect(*secondEdge,intersectionPoint) == QLineF::BoundedIntersection)
                return true;
        }
    }
    return false;
}

int Algorithms::moveLeftAsPossible(FPolygon *fPolygon, int placedNumberofPolygons, int currentIndex)
{
    bool intersectExists = false;
    int shrinkAmount = 0;
    do
    {
        if(fPolygon->boundingRect().left() < 0)
        {
            intersectExists = true;
            fPolygon->setFPosition(0, fPolygon->boundingRect().top());
            break;
        }
        for(int ctr2 = 0; ctr2 < placedNumberofPolygons; ++ctr2)
        {
            if(currentIndex == ctr2)
                continue;
            FPolygon * oldPolygon = polygons[ctr2];
            QRectF intersectRect = oldPolygon->boundingRect().intersect(fPolygon->boundingRect());
            if(fPolygon->boundingRect().left() < oldPolygon->boundingRect().left())
                continue;
            if(intersectRect.width() <= 0)
                continue;
            if(fPolygon->boundingRect().top() < oldPolygon->boundingRect().bottom() + 0.9 &&
               fPolygon->boundingRect().top() > oldPolygon->boundingRect().bottom() - 0.9)
                continue;
            if(oldPolygon->boundingRect().top() < fPolygon->boundingRect().bottom() + 0.9 &&
               oldPolygon->boundingRect().top() > fPolygon->boundingRect().bottom() - 0.9)
                continue;
            if(isIntersect(fPolygon, oldPolygon))
            {
                intersectExists = true;
                if(shrinkAmount != 0)
                    fPolygon->setFPosition(fPolygon->boundingRect().left()+1, fPolygon->boundingRect().top());
                break;
            }
        }

        if(!intersectExists)
        {
            fPolygon->setFPosition(fPolygon->boundingRect().left()-1, fPolygon->boundingRect().top());
            shrinkAmount++;
        }

    } while(!intersectExists);
    return shrinkAmount;
}

int Algorithms::moveTopAsPossible(FPolygon *fPolygon, int currentIndex)
{
    bool intersectExists = false;
    int shrinkAmount = 0;
    do
    {
        if(fPolygon->boundingRect().top() < 0)
        {
            intersectExists = true;
            fPolygon->setFPosition(fPolygon->boundingRect().left(), 0);
            break;
        }
        for(int ctr = 0; ctr < polygons.size(); ++ctr)
        {
            if(currentIndex == ctr)
                continue;
            FPolygon * oldPolygon = polygons[ctr];
            QRectF intersectRect = oldPolygon->boundingRect().intersect(fPolygon->boundingRect());
            if(intersectRect.width() <= 0 || intersectRect.height() <= 0)
                continue;
            if(isIntersect(fPolygon, oldPolygon))
            {
                intersectExists = true;
                if(shrinkAmount != 0)
                    fPolygon->setFPosition(fPolygon->boundingRect().left(), fPolygon->boundingRect().top()+1);
                break;
            }
        }

        if(!intersectExists)
        {
            fPolygon->setFPosition(fPolygon->boundingRect().left(), fPolygon->boundingRect().top()-1);
            shrinkAmount++;
        }

    } while(!intersectExists);
    return shrinkAmount;
}

int Algorithms::moveBottomAsPossible(FPolygon *fPolygon, int currentIndex)
{
    bool intersectExists = false;
    int shrinkAmount = 0;
    do
    {
        if(fPolygon->boundingRect().bottom() > strip->height())
        {
            intersectExists = true;
            fPolygon->setFPosition(fPolygon->boundingRect().left(), strip->height() - fPolygon->boundingRect().height());
            break;
        }
        for(int ctr = 0; ctr < polygons.size(); ++ctr)
        {
            if(currentIndex == ctr)
                continue;
            FPolygon * oldPolygon = polygons[ctr];
            QRectF intersectRect = oldPolygon->boundingRect().intersect(fPolygon->boundingRect());
            if(intersectRect.width() <= 0 || intersectRect.height() <= 0)
                continue;
            if(isIntersect(fPolygon, oldPolygon))
            {
                intersectExists = true;
                if(shrinkAmount != 0)
                    fPolygon->setFPosition(fPolygon->boundingRect().left(), fPolygon->boundingRect().top()-1);
                break;
            }
        }

        if(!intersectExists)
        {
            fPolygon->setFPosition(fPolygon->boundingRect().left(), fPolygon->boundingRect().top()+1);
            shrinkAmount++;
        }

    } while(!intersectExists);
    return shrinkAmount;
}

int Algorithms::slideIn()
{
    sortByTopLeft();
    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        FPolygon *fPolygon = polygons[ctr];
        qreal oldX = fPolygon->boundingRect().left();
        qreal oldY = fPolygon->boundingRect().top();
        moveTopAsPossible(fPolygon,ctr);
        int topShrink = moveLeftAsPossible(fPolygon,polygons.size(),ctr);

        fPolygon->setFPosition(oldX,oldY);
        moveBottomAsPossible(fPolygon,ctr);
        int sinkShrink = moveLeftAsPossible(fPolygon,polygons.size(),ctr);

        if(topShrink >= sinkShrink)
        {
            fPolygon->setFPosition(oldX,oldY);
            moveTopAsPossible(fPolygon,ctr);
            moveLeftAsPossible(fPolygon,polygons.size(),ctr);
        }
    }
    sortByTopLeft();
    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        FPolygon *fPolygon = polygons[ctr];
        qreal oldX = fPolygon->boundingRect().left();
        qreal oldY = fPolygon->boundingRect().top();
        fPolygon->setFPosition(oldX + 50,oldY);
        moveTopAsPossible(fPolygon,ctr);
        int topShrink = moveLeftAsPossible(fPolygon,polygons.size(),ctr);

        fPolygon->setFPosition(oldX,oldY);
        fPolygon->setFPosition(oldX + 50,oldY);
        moveBottomAsPossible(fPolygon,ctr);
        int sinkShrink = moveLeftAsPossible(fPolygon,polygons.size(),ctr);

        if(topShrink >= sinkShrink)
        {
            fPolygon->setFPosition(oldX,oldY);
            moveTopAsPossible(fPolygon,ctr);
            moveLeftAsPossible(fPolygon,polygons.size(),ctr);
        }

        if(sinkShrink < 50 && topShrink < 50)
            fPolygon->setFPosition(oldX,oldY);
    }
    sortByTopLeft();
    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        FPolygon *fPolygon = polygons[ctr];
        moveLeftAsPossible(fPolygon,ctr,ctr);
    }
    return 0;
}

void Algorithms::metaHeuristic()
{
    ui->resultsLabel->setText("Finded sub solutions :");
    int subSolutionSize = 0;
    int maxClusterSize = backupPolygonList.size() / 3;
    int minimumCellSize = backupPolygonList.size() / 5;
    if(backupPolygonList.size() < 10)
    {
        maxClusterSize = backupPolygonList.size();
        minimumCellSize = backupPolygonList.size();
    }
    do
    {
        int temp = INITIAL_TEMP;
        QList<QPolygonF> originalList;
        for(int ctr = 0; ctr < polygons.size(); ++ctr)
            originalList.append(polygons[ctr]->polygon());

        qreal maxDensity = 0;
        qreal maxLeft = 0;
        qreal maxRight = 0;
        int maxIndex1 = -1;
        int maxIndex2 = -1;

        if(polygons.size() < minimumCellSize)
        {
            shake();
            firstFitDecreasing();
            shrinkingWithoutImprovement();
            sortByTopLeft();
            qreal filledArea = polygons[0]->area();

            maxLeft = polygons[0]->boundingRect().left();
            maxRight = polygons[0]->boundingRect().right();
            maxIndex1 = 0;
            maxIndex2 = polygons.size() - 1;
            for(int ctr = 1; ctr < polygons.size(); ++ctr)
            {
                filledArea += polygons[ctr]->area();
                if(polygons[ctr]->boundingRect().right() > maxRight)
                    maxRight = polygons[ctr]->boundingRect().right();
            }
            qreal totalArea = (maxRight - maxLeft) * strip->height();
            maxDensity = filledArea / totalArea * 100;
            ++subSolutionSize;
            ui->resultsLabel->setText(ui->resultsLabel->text() + "\n" + QString::number(subSolutionSize) + ". Sub solution : " + QString::number(maxDensity,'g',4));
        }
        else
        {
            do
            {
                maxDensity = 0;
                maxLeft = 0;
                maxRight = 0;
                maxIndex1 = -1;
                maxIndex2 = -1;

                clearStrip();
                for(int ctr = 0; ctr < originalList.size(); ++ctr)
                    addPolygonToStrip(originalList[ctr]);

                shake();
                firstFitDecreasing();
                shrinkingWithoutImprovement();
                sortByTopLeft();

                if(polygons.size() < 5)
                {
                    for(int ctr = 0; ctr < polygons.size(); ++ctr)
                    {
                        qreal x1 = polygons[ctr]->boundingRect().left();
                        qreal maxX2 = polygons[ctr]->boundingRect().right();
                        qreal filledArea = 0;
                        qreal totalArea = 0;
                        qreal density = 0;
                        for(int ctr2 = ctr; ctr2 < polygons.size(); ++ctr2)
                        {
                            filledArea += polygons[ctr2]->area();
                            if(polygons[ctr2]->boundingRect().right() > maxX2)
                                maxX2 = polygons[ctr2]->boundingRect().right();
                            totalArea = (maxX2 - x1) * strip->height();
                            density = filledArea / totalArea * 100;
                            if(density > maxDensity && (ctr2 - ctr) < maxClusterSize)
                            {
                                maxDensity = density;
                                maxIndex1 = ctr;
                                maxIndex2 = ctr2;
                                maxLeft = x1;
                                maxRight = maxX2;
                            }
                        }
                    }
                }
                else
                {
                    for(int ctr = 0; ctr < polygons.size() - 4; ++ctr)
                    {
                        qreal x1 = polygons[ctr]->boundingRect().left();
                        qreal maxX2 = polygons[ctr]->boundingRect().right();
                        qreal filledArea = polygons[ctr]->area();
                        qreal totalArea = 0;
                        qreal density = 0;
                        for(int ctr2 = ctr + 1; ctr2 < ctr + 4; ++ctr2)
                        {
                            filledArea += polygons[ctr2]->area();
                            if(polygons[ctr2]->boundingRect().right() > maxX2)
                                maxX2 = polygons[ctr2]->boundingRect().right();
                            totalArea = (maxX2 - x1) * strip->height();
                        }
                        for(int ctr2 = ctr + 4; ctr2 < polygons.size(); ++ctr2)
                        {
                            filledArea += polygons[ctr2]->area();
                            if(polygons[ctr2]->boundingRect().right() > maxX2)
                                maxX2 = polygons[ctr2]->boundingRect().right();
                            totalArea = (maxX2 - x1) * strip->height();
                            density = filledArea / totalArea * 100;
                            if(density > maxDensity && (ctr2 - ctr) < maxClusterSize)
                            {
                                maxDensity = density;
                                maxIndex1 = ctr;
                                maxIndex2 = ctr2;
                                maxLeft = x1;
                                maxRight = maxX2;
                            }
                        }
                    }
                }
                temp -= 5;
            }while(maxDensity < temp);
            ++subSolutionSize;
            ui->resultsLabel->setText(ui->resultsLabel->text() + "\n" + QString::number(subSolutionSize) + ". Sub solution : " + QString::number(maxDensity,'g',4));
        }

        QList<QPolygonF> clusteredPolygons;
        for(int ctr = maxIndex1; ctr <= maxIndex2; ++ctr)
        {
            polygons[ctr]->setFPosition(polygons[ctr]->boundingRect().left() - maxLeft, polygons[ctr]->boundingRect().top());
            clusteredPolygons.append(polygons[ctr]->polygon());
        }
        partialPolygons.append(clusteredPolygons);
        for(int ctr = maxIndex1; ctr <= maxIndex2; ++ctr)
            polygons.removeAt(maxIndex1);

        partialWidths.append(maxRight - maxLeft);

        QList<QPolygonF> leftPolygons;
        for(int ctr = 0; ctr < polygons.size(); ++ctr)
            leftPolygons.append(polygons[ctr]->polygon());
        clearStrip();
        for(int ctr = 0; ctr < leftPolygons.size(); ++ctr)
            addPolygonToStrip(leftPolygons[ctr]);
    }while(polygons.size() > 0);

    strip->clear();
    QPen greyPen(Qt::gray);

    QList<QList <int> > sorts;
    QList<qreal> densities;

    for(int ctr = 0; ctr < partialPolygons.size(); ++ctr)
    {
        QList<int> indexes;
        for(int ctr2 = 0; ctr2 < partialPolygons.size(); ++ctr2)
        {
            bool exists = false;
            int value = 0;
            do
            {
                exists = false;
                value = qrand() % partialPolygons.size();

                for(int ctr3 = 0; ctr3 < indexes.size(); ++ctr3)
                {
                    if(indexes[ctr3] == value)
                    {
                        exists = true;
                        break;
                    }
                }
            }while(exists);
            indexes.append(value);
        }
        sorts.append(indexes);
    }

    for(int ctr = 0; ctr < sorts.size(); ++ctr)
    {
        clearStrip();
        QList<int> indexes = sorts[ctr];
        for(int ctr2 = 0; ctr2 < indexes.size(); ++ctr2)
        {
            QList<QPolygonF> tempList = partialPolygons[indexes[ctr2]];
            for(int ctr3 = 0; ctr3 < tempList.size(); ++ctr3)
            {
                int value = qrand() % colorMap.size();
                QColor randomColor = colorMap.value(value);
                QBrush passtelBrush(randomColor);

                polygons.append((FPolygon *) strip->addPolygon(tempList[ctr3],greyPen,passtelBrush));
                FPolygon * fPolygon = polygons.last();
                fPolygon->setFlag(QGraphicsItem::ItemIsMovable,true);
                int distance = 0;
                for(int ctr4 = 0; ctr4 < ctr2; ++ctr4)
                    distance += partialWidths[indexes[ctr4]]+100;

                fPolygon->setFPosition(fPolygon->boundingRect().left() + distance, fPolygon->boundingRect().top());
            }
        }
        for(int ctr = 0; ctr < polygons.size(); ++ctr)
            moveLeftAsPossible(polygons[ctr],polygons.size(),ctr);

        qreal maxBound = polygons[0]->boundingRect().right();
        int maxIndex = 0;
        for(int ctr = 1; ctr < polygons.size(); ++ctr)
        {
            qreal bound = polygons[ctr]->boundingRect().right();
            if(bound > maxBound)
            {
                maxBound = bound;
                maxIndex = ctr;
            }
        }

        strip->setWidth(polygons[maxIndex]->boundingRect().right());
        qreal filledArea = 0;
        for(int ctr = 0; ctr < polygons.size(); ++ctr)
            filledArea += polygons[ctr]->area();

        qreal density = filledArea / strip->area() * 100;
        densities.append(density);
    }

    int maxIndex = 0;
    qreal maxDensity = densities[0];
    for(int ctr = 1; ctr < densities.size(); ++ctr)
    {
        if(densities[ctr] > maxDensity)
        {
            maxDensity = densities[ctr];
            maxIndex = ctr;
        }
    }

    clearStrip();
    QList<int> indexes = sorts[maxIndex];
    for(int ctr2 = 0; ctr2 < indexes.size(); ++ctr2)
    {
        QList<QPolygonF> tempList = partialPolygons[indexes[ctr2]];
        for(int ctr3 = 0; ctr3 < tempList.size(); ++ctr3)
        {
            int value = qrand() % colorMap.size();
            QColor randomColor = colorMap.value(value);
            QBrush passtelBrush(randomColor);

            polygons.append((FPolygon *) strip->addPolygon(tempList[ctr3],greyPen,passtelBrush));
            FPolygon * fPolygon = polygons.last();
            fPolygon->setFlag(QGraphicsItem::ItemIsMovable,true);
            int distance = 0;
            for(int ctr4 = 0; ctr4 < ctr2; ++ctr4)
                distance += partialWidths[indexes[ctr4]]+100;

            fPolygon->setFPosition(fPolygon->boundingRect().left() + distance, fPolygon->boundingRect().top());
        }
    }
    qreal maxBound = polygons[0]->boundingRect().right();
    maxIndex = 0;
    for(int ctr = 1; ctr < polygons.size(); ++ctr)
    {
        qreal bound = polygons[ctr]->boundingRect().right();
        if(bound > maxBound)
        {
            maxBound = bound;
            maxIndex = ctr;
        }
    }

    strip->setWidth(polygons[maxIndex]->boundingRect().right());

    qreal filledArea = 0;
    for(int ctr = 0; ctr < polygons.size(); ++ctr)
        filledArea += polygons[ctr]->area();
    qreal density = filledArea / strip->area() * 100;

    for(int ctr = 0; ctr < polygons.size(); ++ctr)
        moveLeftAsPossible(polygons[ctr],polygons.size(),ctr);

    maxBound = polygons[0]->boundingRect().right();
    maxIndex = 0;
    for(int ctr = 1; ctr < polygons.size(); ++ctr)
    {
        qreal bound = polygons[ctr]->boundingRect().right();
        if(bound > maxBound)
        {
            maxBound = bound;
            maxIndex = ctr;
        }
    }

    strip->setWidth(polygons[maxIndex]->boundingRect().right());

    filledArea = 0;
    for(int ctr = 0; ctr < polygons.size(); ++ctr)
        filledArea += polygons[ctr]->area();
    qreal densityAfterShrinking = filledArea / strip->area() * 100;
    ui->resultsLabel->setText(ui->resultsLabel->text() + "\nLast Shrink : " + QString::number(densityAfterShrinking - density,'g',4));

    strip->removeItem(sceneBound);
    setStripInfo();
    return;
}

void Algorithms::shake()
{
    strip->clear();

    QPen redPen(Qt::red);
    QPen greyPen(Qt::gray);
    QFont miniFont("Arial",11,1);

    bool issueExists = false;

    qreal lastX;

    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        QBrush passtelBrush(colors.at(ctr));
        QPolygonF polygon(datas.at(ctr));

        polygons[ctr] = (FPolygon *) strip->addPolygon(polygon, greyPen, passtelBrush);
        FPolygon * fPolygon = polygons[ctr];

        fPolygon->rotateToMinBound(FPolygon::ROTATE_TO_MIN_BOUND_PARALLEL_EDGE);
        qreal degree = (qrand() % RANDOMIZE_ANGLE) - RANDOMIZE_ANGLE / 2;
        fPolygon->rotateFPolygon(degree);

        if(fPolygon->boundingRect().height() > strip->height())
        {
            fPolygon->rotateFPolygon(90);
            if(fPolygon->boundingRect().height() > strip->height())
            {
                issueExists = true;
                fPolygon->rotateFPolygon(-90);
                fPolygon->rotateFPolygon(-degree);
                strip->removeItem(fPolygon);
                polygons.removeAt(ctr);
                continue;
            }
        }

        if(ctr == 0)
        {
            fPolygon->setFPosition(0, 0);
        }
        else
        {
            lastX = bounds[ctr - 1]->rect().right();
            fPolygon->setFPosition(lastX, 0);
        }

        bounds[ctr] = strip->addRect(fPolygon->boundingRect(), redPen, Qt::NoBrush);
        QGraphicsRectItem * bound = bounds[ctr];
        bound->setParentItem(fPolygon);

        texts[ctr] = strip->addText(QString::number(fPolygon->boundArea()),miniFont);
        QGraphicsTextItem * text = texts[ctr];
        text->setDefaultTextColor(Qt::red);
        text->setPos(bound->rect().left(), bound->rect().bottom() + 5);
        text->setParentItem(fPolygon);
    }

    if(issueExists)
    {
        issue = new IssueDialog(this,"Polygon Fit Problem","One or more polygons does not fit into the strip!", "Unfitting polygons skipped.");
        issue->exec();
    }

    strip->setWidth(bounds.last()->rect().right());
    setStripInfo();
}

void Algorithms::setStripInfo()
{
    QPen redPen(Qt::red);

    QRectF boundRect;
    boundRect = strip->sceneRect();
    sceneBound = strip->addRect(boundRect, redPen, Qt::NoBrush);
    ui->stripWidthLabel->setText(QString::number((int)boundRect.width()));
    qreal filledArea = 0;
    for(int ctr = 0; ctr < polygons.size(); ++ctr)
        filledArea += polygons[ctr]->area();
    qreal density = filledArea / strip->area() * 100;
    ui->stripDensityLabel->setText("%" + QString::number(density,'g',4));
    ui->polygonNumberLabel->setText(QString::number(polygons.size()));
}

void Algorithms::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    zoom(numSteps);
    event->accept();
}

void Algorithms::zoom(int steps)
{
    if(steps < 0)
    {
        ui->graphicsView->scale(ZOOM_OUT_RATIO,ZOOM_OUT_RATIO);
    }
    else
    {
        ui->graphicsView->scale(ZOOM_IN_RATIO,ZOOM_IN_RATIO);
    }
}

void Algorithms::on_runButton_clicked()
{
    ui->resetButton->setEnabled(true);
    // Heuristic
    if(ui->algorithmComboBox->currentIndex() == 0)
    {
        int embedAlgorithm = heuristicAlgorithms[0];
        int stripPackingAlgorithm = heuristicAlgorithms[1];
        int shrinkingAlgorithm = heuristicAlgorithms[2];

        double area = 0;
        for(int ctr = 0; ctr < polygons.size(); ++ctr)
            area += polygons[ctr]->boundArea();
        switch (embedAlgorithm)
        {
        case 0 :
            embed(FPolygon::ROTATE_TO_MIN_BOUND_SEARCH_WITH_ROTATE);
            break;
        case 1 :
            embed(FPolygon::ROTATE_TO_MIN_BOUND_PARALLEL_EDGE);
            break;
        default :
            embed(FPolygon::ROTATE_TO_MIN_BOUND_SEARCH_WITH_ROTATE);
            break;
        }
        double newArea = 0;
        for(int ctr = 0; ctr < polygons.size(); ++ctr)
            newArea += polygons[ctr]->boundArea();
        ui->resultsLabel->setText("Embed : % " + QString::number(100 - newArea / area * 100,'g',4));
        area = newArea;

        switch (stripPackingAlgorithm)
        {
        case 0 :
            stripPacking(Algorithms::STRIP_PACKING_FIRST_FIT_DECREASING);
            break;
        case 1 :
            stripPacking(Algorithms::STRIP_PACKING_SPLIT_FIT);
            break;
        case 2 :
            stripPacking(Algorithms::STRIP_PACKING_MAXIMUM_CONTACTING_PERIMETER);
            break;
        default :
            stripPacking(Algorithms::STRIP_PACKING_FIRST_FIT_DECREASING);
            break;
        }
        qreal filledArea = 0;
        for(int ctr = 0; ctr < polygons.size(); ++ctr)
            filledArea += polygons[ctr]->area();
        qreal density = filledArea / strip->area() * 100;
        ui->resultsLabel->setText(ui->resultsLabel->text() + "\nStrip Pack : % " + QString::number(density,'g',4));

        switch (shrinkingAlgorithm)
        {
        case SHRINKING_WITHOUT_IMPROVEMENT :
            shrinking(Algorithms::SHRINKING_WITHOUT_IMPROVEMENT);
            break;
        case SHRINKING_WITH_IMPROVEMENT :
            shrinking(Algorithms::SHRINKING_WITH_IMPROVEMENT);
            break;
        default :
            shrinking(Algorithms::SHRINKING_WITHOUT_IMPROVEMENT);
            break;
        }
        qreal newDensity = filledArea / strip->area() * 100;
        ui->resultsLabel->setText(ui->resultsLabel->text() + "\nShrink : % " + QString::number(newDensity - density,'g',4));
    }
    else
    {// Meta Heuristic
        partialPolygons.clear();
        partialWidths.clear();
        metaHeuristic();
    }
    ui->runButton->setDisabled(true);
}

void Algorithms::on_algorithmComboBox_currentIndexChanged(int index)
{
    ui->algorithmStepsLabel->setText(algorithmStepsString);
    if(index == 0)
    {
        switch(heuristicAlgorithms[0])
        {
        case FPolygon::ROTATE_TO_MIN_BOUND_SEARCH_WITH_ROTATE :
            ui->algorithmStepsLabel->setText(ui->algorithmStepsLabel->text() + "Embed (Search With Rotate) ->");
            break;
        case FPolygon::ROTATE_TO_MIN_BOUND_PARALLEL_EDGE :
            ui->algorithmStepsLabel->setText(ui->algorithmStepsLabel->text() + "Embed (Parallel Edge) ->");
            break;
        default :
            break;
        }
        switch(heuristicAlgorithms[1])
        {
        case STRIP_PACKING_FIRST_FIT_DECREASING :
            ui->algorithmStepsLabel->setText(ui->algorithmStepsLabel->text() + "Strip Pack (First Fit) ->");
            break;
        case STRIP_PACKING_SPLIT_FIT :
            ui->algorithmStepsLabel->setText(ui->algorithmStepsLabel->text() + "Strip Pack (Split Fit) ->");
            break;
        case STRIP_PACKING_MAXIMUM_CONTACTING_PERIMETER :
            ui->algorithmStepsLabel->setText(ui->algorithmStepsLabel->text() + "Strip Pack (Maximum Contacting Perimeter) ->");
            break;
        default :
            break;
        }
        switch(heuristicAlgorithms[2])
        {
        case SHRINKING_WITHOUT_IMPROVEMENT :
            ui->algorithmStepsLabel->setText(ui->algorithmStepsLabel->text() + "Shrink (Without Improvement)");
            break;
        case SHRINKING_WITH_IMPROVEMENT :
            ui->algorithmStepsLabel->setText(ui->algorithmStepsLabel->text() + "Shrink (With Improvement)");
            break;
        default :
            break;
        }    }
    else
    {
        ui->algorithmStepsLabel->setText(ui->algorithmStepsLabel->text() + "Grasp (Use heuristic function + random changes) -> Find sub solution -> Run the algorithm for rest -> Bring all sub solutions -> Shrink them");
    }
}

void Algorithms::on_resetButton_clicked()
{
    clearStrip();
    addPolygonsToStrip(backupPolygonList);
    ui->runButton->setEnabled(true);
    ui->resetButton->setDisabled(true);
    ui->resultsLabel->setText("");
}

} // END OF PolygonPacking NAMESPACE
