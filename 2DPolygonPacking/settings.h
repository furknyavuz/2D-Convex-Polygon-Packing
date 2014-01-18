#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

#include "ui_settings.h"

namespace PolygonPacking
{

/**
 * @class Settings
 *
 * @brief Class for set strip and algorithms
 *
 * @author Furkan Yavuz
 */
class Settings : public QWidget
{

    Q_OBJECT
    
public:

    /**
     * Constructor sets the parent with given QWidget.
     *
     * @param parent parent widget
     */
    explicit Settings(QWidget *parent = 0);

    /**
     * Default Destructor.
     */
    ~Settings();

    /**
     * Returns the heuristic algorithms.
     *
     * @return Heuristic algorithms
     */
    QList<int> selectedHeuristicAlgorithms();

    /**
     * Returns the meta heuristic algorithms.
     *
     * @return Meta heuristic algorithms
     */
    QList<int> selectedMetaHeuristicAlgorithms();

    /**
     * Returns the height of strip
     *
     * @return Height of strip
     */
    int getHeight();

private slots:
    void on_embedComboBox_currentIndexChanged(int index);

    void on_stripPackingComboBox_currentIndexChanged(int index);

    void on_shrinkingComboBox_currentIndexChanged(int index);

private:
    Ui::Settings *ui;

    QString titleHeuristic;
    QString embedSearchWithRotate;
    QString embedParallelEdge;
    QString stripPackingFirstFit;
    QString stripPackingSplitFit;
    QString stripPackingMaximumContactingPerimeter;
    QString shrinkingWithoutImprovement;
    QString shrinkingWithImprovement;

    QString currentEmbed;
    QString currentStripPacking;
    QString currentShrinking;
    QString currentMetaheuristic;
};

}

#endif // SETTINGS_H
