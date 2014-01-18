#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

#include <QList>
#include <QAction>
#include <QMainWindow>
#include <QGraphicsRectItem>

#include "fpolygon.h"
#include "algorithms.h"
#include "polygongenerator.h"
#include "settings.h"

#include "ui_mainwindow.h"

namespace PolygonPacking
{

/**
 * @class MainWindow
 *
 * @brief Main class for gui
 *
 * @author Furkan Yavuz
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:

    /**
     * Constructor sets the parent with given QWidget.
     *
     * @param parent parent widget
     */
    explicit MainWindow(QWidget *parent = 0);

    /**
     * Default Destructor.
     */
    ~MainWindow();
    
private:

    /**
     * Open file with given file path.
     *
     * @param filePath path of file
     *
     * @return If file can open true else false
     */
    bool openFile(QString filePath);

    Ui::MainWindow *ui;
    Algorithms * algorithms;
    Settings * settings;
    PolygonGenerator * polyGen;
    QFile *file;
    int stripHeight;
    QList<int> heuristicAlgorithms;
    QList<int> metaHeuristicAlgorithms;
    QList<QPolygonF> polygons;

private slots:
    void addPolygons();
    void generatePolygons();
    void polygonsList();
    void save();
    void neo();

public slots:
    void createStrip();
    void polygonAdded();
};

} // END OF PolygonPacking NAMESPACE
#endif // MAINWINDOW_H
