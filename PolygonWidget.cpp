#include "PolygonWidget.h"
//#include "EarClipping.h"
#include <QPainter>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

PolygonWidget::PolygonWidget(QWidget *parent) : QWidget(parent) {
    loadPolygonsFromJson("polygons.json");
}

void PolygonWidget::loadPolygonsFromJson(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open the file: %s", qPrintable(filename));
        return;
    }

    QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if (document.isNull()) {
        qWarning("Invalid JSON format.");
        return;
    }

    QJsonObject jsonObject = document.object();
    QJsonArray polygonsArray = jsonObject["polygons"].toArray();

    for (const QJsonValue &value : polygonsArray) {
        QJsonObject polygonObject = value.toObject();
        QJsonArray coordArray = polygonObject["coord"].toArray();

        QVector<QPoint> polygonPoints;
        for (const QJsonValue &coordValue : coordArray) {
            QJsonObject coordObject = coordValue.toObject();
            int x = coordObject["x"].toInt();
            int y = coordObject["y"].toInt();
            polygonPoints.append(QPoint(x, y));
        }

        polygons.append(polygonPoints);

        //EarClipping triangulation
        /*QVector<QVector<QPoint>> triangulated = EarClipping::triangulate(polygonPoints);
        for (const QVector<QPoint> &triangle : triangulated) {
            trianglesEarClipping.append(triangle);
        }*/

        //EdgeFlipping triangulation
        /*QVector<Triangle> triangleList;
        for (const QVector<QPoint> &triangle : triangulated) {
            if (triangle.size() == 3) {
                triangleList.append({triangle[0], triangle[1], triangle[2]});
            }
        }*/

        /*int originalCount = triangleList.size();
        qDebug() << "Before edge flipping:" << originalCount << "triangles";
        EdgeFlipping::apply(triangleList);
        qDebug() << "After edge flipping:" << triangleList.size() << "triangles";

        for (const Triangle &t : triangleList) {
            trianglesEdgeFlipping.append(t);
        }*/

        //CDT triangulation
        QVector<Triangle> cdtTriangles = CDT::triangulate(polygonPoints);
        qDebug() << "CDT triangulation:" << cdtTriangles.size() << "triangles";

        for (const Triangle &t : cdtTriangles) {
            trianglesCDT.append(t);
        }
    }
}

void PolygonWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //Draw polygon outlines
    painter.setPen(Qt::green);
    painter.setBrush(Qt::NoBrush);
    for (const QVector<QPoint> &polygon : polygons) {
        painter.drawPolygon(QPolygon(polygon));
    }

    //Draw text with triangle count
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 12));
    painter.drawText(10, 20, QString("CDT triangles: %1").arg(trianglesCDT.size()));

    //Draw CDT triangles with high visibility
    painter.setPen(QPen(Qt::blue, 2));
    painter.setBrush(QColor(255, 0, 0, 150));

    for (const Triangle &triangle : trianglesCDT) {
        QPolygon poly;
        poly << triangle.a << triangle.b << triangle.c;
        painter.drawPolygon(QPolygon({triangle.a, triangle.b, triangle.c}));
    }

    /*painter.setBrush(Qt::blue);
    for (const QVector<QPoint> &triangle : trianglesEarClipping) {
        painter.drawPolygon(QPolygon(triangle));
    }*/

    /*painter.setPen(QPen(Qt::red, 1));
    painter.setBrush(QColor(255, 0, 0, 100));
    for (const Triangle &triangle : trianglesEdgeFlipping) {
        painter.drawPolygon(QPolygon({triangle.a, triangle.b, triangle.c}));
    }*/
}