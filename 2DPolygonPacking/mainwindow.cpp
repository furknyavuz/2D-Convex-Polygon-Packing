#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

#define SCENE_WIDTH 755
#define SCENE_HEIGHT 500

namespace PolygonPacking
{

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->actionGenerate_Polygons->setDisabled(true);
    ui->actionAddPolygons->setDisabled(true);
    ui->actionPolygons_List->setDisabled(true);
    ui->actionSave->setDisabled(true);

    this->setWindowTitle("2D Polygon Packing Problem");

    connect(ui->actionGenerate_Polygons, SIGNAL(triggered()), this, SLOT(generatePolygons()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(neo()));
    connect(ui->actionAddPolygons, SIGNAL(triggered()), this, SLOT(addPolygons()));
    connect(ui->actionPolygons_List, SIGNAL(triggered()), this, SLOT(polygonsList()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));

    stripHeight = SCENE_HEIGHT;

    settings = new Settings(this);
    this->setCentralWidget(settings);
    this->resize(800, 600);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::openFile(QString filePath)
{
    file = new QFile(filePath);

    if(!file->open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file->errorString());
        return false;
    }
    return true;
}

void MainWindow::addPolygons()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.fply)"));

    bool isOpened = openFile(filePath);

    if(!isOpened)
        return;

    QTextStream in(file);

    algorithms = new Algorithms(this, SCENE_WIDTH, stripHeight, heuristicAlgorithms, metaHeuristicAlgorithms);
    polygons.clear();

    while(!in.atEnd()) {
        QString line = in.readLine();
        QStringList points = line.split(";");
        QPolygonF polygon;
        for(int ctr = 0; ctr < points.size(); ++ctr)
        {
            QStringList coordinates = points[ctr].split(",");
            if(coordinates.size() != 2)
                continue;
            int x = coordinates[0].toInt();
            int y = coordinates[1].toInt();
            polygon << QPointF::QPointF(x, y);
        }
        polygons.append(polygon);
    }

    algorithms->addPolygonsToStrip(polygons);

    file->close();
    this->setCentralWidget(algorithms);

    ui->actionGenerate_Polygons->setEnabled(true);
    ui->actionAddPolygons->setEnabled(true);
    ui->actionPolygons_List->setDisabled(true);
    ui->actionSave->setEnabled(true);
}

void MainWindow::generatePolygons()
{
    polyGen = new PolygonGenerator(this);
    this->setCentralWidget(polyGen);

    ui->actionGenerate_Polygons->setEnabled(true);
    ui->actionAddPolygons->setEnabled(true);
    ui->actionPolygons_List->setEnabled(true);
    ui->actionSave->setEnabled(true);
}

void MainWindow::polygonsList()
{
    algorithms = new Algorithms(this, SCENE_WIDTH, stripHeight, heuristicAlgorithms, metaHeuristicAlgorithms);

    polygons = polyGen->getDatas();

    algorithms->addPolygonsToStrip(polygons);
    this->setCentralWidget(algorithms);

    ui->actionGenerate_Polygons->setEnabled(true);
    ui->actionAddPolygons->setEnabled(true);
    ui->actionPolygons_List->setDisabled(true);
    ui->actionSave->setEnabled(true);
}

void MainWindow::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Files (*.fply)"));
    file = new QFile(fileName);

    if(!file->open(QIODevice::WriteOnly)) {
        QMessageBox::information(0, "error", file->errorString());
    }

    QTextStream out(file);

    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        QPolygonF polygon = polygons[ctr];
        for(int ctr2 = 0; ctr2 < polygon.size(); ++ctr2)
        {
            QPointF point = polygon[ctr2];
            int pX = point.x();
            int pY = point.y();
            QString str = QString::number(pX) + "," + QString::number(pY) + ";";
            out << str;
        }
        out << "\n";
    }
    file->close();
}

void MainWindow::neo()
{
    settings = new Settings(this);
    this->setCentralWidget(settings);

    ui->actionGenerate_Polygons->setDisabled(true);
    ui->actionAddPolygons->setDisabled(true);
    ui->actionPolygons_List->setDisabled(true);
    ui->actionSave->setDisabled(true);
}

void MainWindow::createStrip()
{
    heuristicAlgorithms = settings->selectedHeuristicAlgorithms();
    metaHeuristicAlgorithms = settings->selectedMetaHeuristicAlgorithms();
    stripHeight = settings->getHeight();
    algorithms = new Algorithms(this, SCENE_WIDTH, stripHeight, heuristicAlgorithms, metaHeuristicAlgorithms);

    ui->actionGenerate_Polygons->setEnabled(true);
    ui->actionAddPolygons->setEnabled(true);
    ui->actionPolygons_List->setDisabled(true);
    ui->actionSave->setEnabled(true);

    this->setCentralWidget(algorithms);
}

void MainWindow::polygonAdded()
{
    polygons = polyGen->getDatas();
}

} // END OF PolygonPacking NAMESPACE
