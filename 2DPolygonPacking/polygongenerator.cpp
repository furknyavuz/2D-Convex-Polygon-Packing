#include "polygongenerator.h"
#include "ui_polygongenerator.h"

#define GENERATE_RESET 100000
#define MAX_GENERATE_DISTANCE 150
#define MIN_GENERATE_DISTANCE 50
#define GENERATE_ANGLE 120
#define SCENE_WIDTH 450
#define SCENE_HEIGHT 395

namespace PolygonPacking
{

PolygonGenerator::PolygonGenerator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PolygonGenerator)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0,0,485,395);
    ui->graphicsView->setScene(scene);
    ui->addToPolygonsButton->setDisabled(true);
    datas.clear();

    connect(ui->addToPolygonsButton, SIGNAL(clicked()), parent, SLOT(polygonAdded()));
}

PolygonGenerator::~PolygonGenerator()
{
    delete ui;
}

QPolygonF PolygonGenerator::generateConvexPolygon(int points)
{
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    int x1, x2;
    int y1, y2;
    int trySize = 0;

    int generateDistance = randInt(MIN_GENERATE_DISTANCE,MAX_GENERATE_DISTANCE);

    QPolygonF polygon;
    QList<QLineF> lines;

    x1 = randInt(0,scene->width() / 2);
    y1 = randInt(0,scene->height() / 2);

    x2 = randInt(x1,scene->width() / 2);
    y2 = randInt(0,y1);

    QLineF line;
    line.setP1(QPointF::QPointF(x1,y1));
    line.setP2(QPointF::QPointF(x2,y2));
    lines.append(line);

    QPointF lastPoint = lines.last().p2();
    line.setP1(lastPoint);

    qreal angle = 0;
    QLineF newLine;
    qreal newAngle = 0;
    qreal firstAngle = 0;

    do
    {
        x2 = randInt(lastPoint.x() - generateDistance,lastPoint.x() + generateDistance);
        y2 = randInt(lastPoint.y() - generateDistance,lastPoint.y() + generateDistance);
        line.setP2(QPointF::QPointF(x2,y2));

        angle = line.angleTo(lines.last());

    } while(angle < 0 || angle > GENERATE_ANGLE);
    lines.append(line);

    for(int ctr = 2; ctr < points; ++ctr)
    {
        trySize = 0;

        lastPoint = lines.last().p2();
        line.setP1(lastPoint);
        do
        {
            angle = 0;
            newAngle = 0;
            firstAngle = 0;
            do
            {
                bool isIntersect = false;
                do
                {
                    ++trySize;
                    if(trySize > GENERATE_RESET)
                    {
                        QPolygonF poly;
                        return poly;
                    }
                    isIntersect = false;
                    x2 = randInt(lastPoint.x() - generateDistance,lastPoint.x() + generateDistance);
                    y2 = randInt(lastPoint.y() - generateDistance,lastPoint.y() + generateDistance);

                    line.setP2(QPointF::QPointF(x2,y2));
                    for(int ctr2 = 0; ctr2 < lines.size()-1; ++ctr2)
                    {
                        QPointF *pnt = new QPointF;
                        if(line.intersect(lines.at(ctr2),pnt) == QLineF::BoundedIntersection)
                        {
                            isIntersect = true;
                            break;
                        }
                    }
                } while(isIntersect);
                angle = line.angleTo(lines.last());

            } while(angle < 0 || angle > GENERATE_ANGLE);

            newLine.setPoints(line.p2(),lines.first().p1());
            newAngle = newLine.angleTo(lines.last());
            firstAngle = lines.first().angleTo(lines.last());

        } while(newAngle < angle || newAngle > firstAngle);
        lines.append(line);
    }

    for(int ctr = 0; ctr < points; ++ctr)
        polygon << lines.at(ctr).p1();
    return polygon;
}

int PolygonGenerator::randInt(int min, int max)
{
    return qrand() % ((max + 1) - min) + min;
}

QList<QPolygonF> PolygonGenerator::getDatas()
{
    return datas;
}

void PolygonGenerator::on_genbutton_clicked()
{
    ui->addToPolygonsButton->setEnabled(true);
    scene->clear();

    QBrush blackBrush(Qt::black);
    QPen blackPen(Qt::black);
    QBrush redBrush(Qt::red);
    QPen redPen(Qt::red);

    FPolygon * newPolygon = new FPolygon();
    QPolygonF polygon;
    do
    {
        scene->clear();
        polygon = generateConvexPolygon(ui->pointsSpinBox->value());
        newPolygon->setPolygon(polygon);
    } while(polygon.size() != ui->pointsSpinBox->value() || polygon.boundingRect().height() > SCENE_HEIGHT || polygon.boundingRect().width() > SCENE_WIDTH || newPolygon->area() < 1000);

    genPolygon = (FPolygon *) scene->addPolygon(polygon, Qt::NoPen, blackBrush);
    genPolygon->setFlag(QGraphicsItem::ItemIsMovable);

    genPolygon->setFPosition(scene->width()/2-genPolygon->boundingRect().width()/2,scene->height()/2-genPolygon->boundingRect().height()/2);

    genBound = scene->addRect(genPolygon->boundingRect(), redPen, Qt::NoBrush);
    genBound->setParentItem(genPolygon);

    for(int ctr = 0; ctr < polygon.size(); ++ctr)
    {
        QGraphicsEllipseItem *point = scene->addEllipse(genPolygon->polygon().at(ctr).x()-3,genPolygon->polygon().at(ctr).y()-3,7,7,blackPen,redBrush);
        point->setParentItem(genPolygon);
    }
    setPolygonInfo(genPolygon);
}

void PolygonGenerator::on_addToPolygonsButton_clicked()
{
    datas.append(genPolygon->polygon());
    polygons.append(genPolygon);
    bounds.append(genBound);
    ui->polygonsList->addItem("polygon" + QString::number(polygons.size()));
}

void PolygonGenerator::on_polygonsList_clicked(const QModelIndex &index)
{
    scene->clear();
    QBrush blackBrush(Qt::black);
    QPen blackPen(Qt::black);
    QBrush redBrush(Qt::red);
    QPen redPen(Qt::red);

    QPolygonF polygon(datas.at(index.row()));

    polygons[index.row()] = (FPolygon *) scene->addPolygon(polygon, Qt::NoPen, blackBrush);
    FPolygon * selectedPolygon = polygons[index.row()];

    bounds[index.row()] = scene->addRect(selectedPolygon->boundingRect(), redPen, Qt::NoBrush);
    bounds[index.row()]->setParentItem(selectedPolygon);
    for(int ctr = 0; ctr < polygon.size(); ++ctr)
    {
        QGraphicsEllipseItem *point = scene->addEllipse(selectedPolygon->polygon().at(ctr).x()-3,selectedPolygon->polygon().at(ctr).y()-3,7,7,blackPen,redBrush);
        point->setParentItem(selectedPolygon);
    }
    selectedPolygon->setFlag(QGraphicsItem::ItemIsMovable);
    setPolygonInfo(selectedPolygon);
}

void PolygonGenerator::setPolygonInfo(FPolygon * polygon)
{
    ui->areaInfo->setText(QString::number(polygon->area()) + " px²");
    ui->boundedAreaInfo->setText(QString::number(polygon->boundArea()) + " px²");
    ui->localDensityInfo->setText("% " + QString::number(polygon->localDensity()));
    ui->widthInfo->setText(QString::number(polygon->boundingRect().width()) + " px");
    ui->heightInfo->setText(QString::number(polygon->boundingRect().height()) + " px");
}

} // END OF PolygonPacking NAMESPACE
