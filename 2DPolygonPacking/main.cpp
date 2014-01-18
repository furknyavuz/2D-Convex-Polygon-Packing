#include <QtGui/QApplication>
#include "mainwindow.h"

using namespace PolygonPacking;

int main(int argc, char *argv[])
{
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    QApplication application(argc, argv);
    MainWindow programGUI;
    programGUI.show();
    
    return application.exec();
}
