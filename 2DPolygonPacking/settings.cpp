#include "settings.h"

namespace PolygonPacking
{

Settings::Settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    ui->embedComboBox->addItem("Search With Rotate");
    ui->embedComboBox->addItem("Parallel Edge");
    ui->stripPackingComboBox->addItem("First Fit Decreasing");
    ui->stripPackingComboBox->addItem("Split Fit");
    ui->stripPackingComboBox->addItem("Maximum Contacting Perimeter");
    ui->shrinkingComboBox->addItem("Shrinking Without Improvement");
    ui->shrinkingComboBox->addItem("Shrinking With Improvement");

    titleHeuristic = "Heuristic Algorithm\n";

    embedSearchWithRotate = "1. Embedding\nSearch With Rotate : Rotate polygon 1 degree each time to find minimum bounding rect.\n\n";

    embedParallelEdge = "1. Embedding\nParallel Edge : For all edges of polygon, rotate polygon until edge is parallel with x axis, select the polygon with minimum bounding rect.\n\n";

    stripPackingFirstFit = "2. Strip Packing\nFirst Fit Decreasing : Places the item that have maximum width first.\n\n";

    stripPackingSplitFit = "2. Strip Packing\nSplit Fit : Split items into two;\n- 1. Items height bigger than strip's height / 2\n- 2. Items height less or equal than strip's height / 2.\n";
    stripPackingSplitFit += "- Place items in the first list using first fit decreasing.\n- Rearrange them;\n- 1.1. Items height bigger than strip's height * 2 / 3\n";
    stripPackingSplitFit += "- 1.2. Items height less of equal than strip's height * 2 / 3\n- Items in 1.1. list moves bottom other goes up\n";
    stripPackingSplitFit += "- Items in 2. list places using first fit decreasing next to the 1.2. list items\n- Items does not fit in 2. list places on top of the 1.2. list items using first fit algorithm.\n\n";

    stripPackingMaximumContactingPerimeter = "2. Strip Packing\nMaximum Contacting Perimeter :\n1. Sort items by decreasing area\n2. For each item find all positions where left and top of item is not empty\n";
    stripPackingMaximumContactingPerimeter += "3. Calculate the contact length with other items or strip\n4. Select the maximum contacting perimeter\n\n";

    shrinkingWithoutImprovement = "3. Shrinking : Move each polygon to left much as possible. Try move function for 4 reflections of polygon. Select the most moved one.\n\n";
    shrinkingWithImprovement = "3. Shrinking : Move each polygon to left much as possible. Try move function for 4 reflections of polygon. If move amount is low, select the reflection that left side of polygon is more full.\n\n";

    currentEmbed = embedSearchWithRotate;
    currentStripPacking = stripPackingFirstFit;
    currentShrinking = shrinkingWithoutImprovement;

    currentMetaheuristic = "Metaheuristic Algorithm\n1. Use heuristic algorithms with small random changes (grasp).\n\n2. Find nice shrinked sub area.\n\n";
    currentMetaheuristic += "3. Subtract that area and run the algorithm for rest polygons.\n\n4. Bring all sub areas together and shrink them.";

    ui->heuristicTextBrowser->insertPlainText(titleHeuristic);
    ui->heuristicTextBrowser->insertPlainText(currentEmbed);
    ui->heuristicTextBrowser->insertPlainText(currentStripPacking);
    ui->heuristicTextBrowser->insertPlainText(currentShrinking);

    ui->metaHuristicTextBrowser->insertPlainText(currentMetaheuristic);

    connect(ui->createButton, SIGNAL(clicked()), parent, SLOT(createStrip()));
}

Settings::~Settings()
{
    delete ui;
}

QList<int> Settings::selectedHeuristicAlgorithms()
{
    QList<int> algorithms;
    algorithms.append(ui->embedComboBox->currentIndex());
    algorithms.append(ui->stripPackingComboBox->currentIndex());
    algorithms.append(ui->shrinkingComboBox->currentIndex());
    return algorithms;
}

QList<int> Settings::selectedMetaHeuristicAlgorithms()
{
    QList<int> algorithms;
    algorithms.append(ui->embedComboBox->currentIndex());
    algorithms.append(ui->stripPackingComboBox->currentIndex());
    algorithms.append(ui->shrinkingComboBox->currentIndex());
    return algorithms;
}

int Settings::getHeight()
{
    return ui->stripHeightSpinBox->value();
}

void Settings::on_embedComboBox_currentIndexChanged(int index)
{
    ui->heuristicTextBrowser->clear();
    ui->heuristicTextBrowser->insertPlainText(titleHeuristic);
    switch(index)
    {
    case 0 :
    {
        currentEmbed = embedSearchWithRotate;
        break;
    }
    case 1 :
    {
        currentEmbed = embedParallelEdge;
        break;
    }
    }
    ui->heuristicTextBrowser->insertPlainText(currentEmbed);
    ui->heuristicTextBrowser->insertPlainText(currentStripPacking);
    ui->heuristicTextBrowser->insertPlainText(currentShrinking);
}

void Settings::on_stripPackingComboBox_currentIndexChanged(int index)
{
    ui->heuristicTextBrowser->clear();
    ui->heuristicTextBrowser->insertPlainText(titleHeuristic);
    ui->heuristicTextBrowser->insertPlainText(currentEmbed);
    switch(index)
    {
    case 0 :
    {
        currentStripPacking = stripPackingFirstFit;
        break;
    }
    case 1 :
    {
        currentStripPacking = stripPackingSplitFit;
        break;
    }
    case 2 :
    {
        currentStripPacking = stripPackingMaximumContactingPerimeter;
        break;
    }
    }
    ui->heuristicTextBrowser->insertPlainText(currentStripPacking);
    ui->heuristicTextBrowser->insertPlainText(currentShrinking);
}

void Settings::on_shrinkingComboBox_currentIndexChanged(int index)
{
    ui->heuristicTextBrowser->clear();
    ui->heuristicTextBrowser->insertPlainText(titleHeuristic);
    ui->heuristicTextBrowser->insertPlainText(currentEmbed);
    ui->heuristicTextBrowser->insertPlainText(currentStripPacking);
    switch(index)
    {
    case 0 :
    {
        currentShrinking = shrinkingWithoutImprovement;
        break;
    }
    case 1 :
    {
        currentShrinking = shrinkingWithImprovement;
        break;
    }
    }
    ui->heuristicTextBrowser->insertPlainText(currentShrinking);
}
}


