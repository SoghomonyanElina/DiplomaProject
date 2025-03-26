#ifndef POLYGONWIDGET_HPP
#define POLYGONWIDGET_HPP

#include <QWidget>
#include <QVector>
#include <QPoint>
//#include "EdgeFlipping.h"
#include "CDT.h"

class PolygonWidget : public QWidget {
    Q_OBJECT
public:
    explicit PolygonWidget(QWidget *parent = nullptr);
    void loadPolygonsFromJson(const QString &filename);

    enum TriangulationMode {
        EarClipping,
        EdgeFlipping,
        CDT
    };

    void setTriangulationMode(TriangulationMode mode) { currentMode = mode; update(); }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<QVector<QPoint>> polygons;
    QVector<QVector<QPoint>> trianglesEarClipping;
    QVector<Triangle> trianglesEdgeFlipping;
    QVector<Triangle> trianglesCDT;
    TriangulationMode currentMode = CDT;
};

#endif //POLYGONWIDGET_HPP