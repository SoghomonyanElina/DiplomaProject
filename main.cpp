#include <QApplication>
#include <QMainWindow>
#include <QScrollArea>
//#include "JsonGenerator.h"
#include "PolygonWidget.h"

int main(int argc, char *argv[]) {
    qInstallMessageHandler(0);
    QApplication app(argc, argv);
    QMainWindow mainWindow;
    //QJsonObject result = generator(10, 4, 10, "convex");
    //saveJsonToFile(result, "polygons.json");
    PolygonWidget *polygonWidget = new PolygonWidget();
    //widget->setMinimumSize(2500, 2500);
    //QScrollArea* scrollArea = new QScrollArea();
    //scrollArea->setWidget(widget);
    //scrollArea->resize(1000, 800);
    //scrollArea->show();
    mainWindow.setCentralWidget(polygonWidget);
    mainWindow.resize(700, 400);
    mainWindow.show();
    return app.exec();
}