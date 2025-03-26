#include "JsonGenerator.h"
#include <QLineF>
#include <algorithm>
#include <cmath>

QJsonArray generateConvexPolygon(int vertexCount, int cell_x, int cell_y, int cell_width, int cell_height) {
    QJsonArray coordArray;
    QVector<QPoint> points;

    for (int i = 0; i < vertexCount; ++i) {
        int x = QRandomGenerator::global()->bounded(cell_x + 10, cell_x + cell_width - 10);
        int y = QRandomGenerator::global()->bounded(cell_y + 10, cell_y + cell_height - 10);
        points.append(QPoint(x, y));
    }

    QPoint center(0, 0);
    for (const auto& point : points) {
        center += point;
    }
    center /= points.size();

    std::sort(points.begin(), points.end(), [center](const QPoint &a, const QPoint &b) {
        return atan2(a.y() - center.y(), a.x() - center.x()) < atan2(b.y() - center.y(), b.x() - center.x());
    });

    for (const auto &point : points) {
        coordArray.append(QJsonObject{{"x", point.x()}, {"y", point.y()}});
    }

    return coordArray;
}



QJsonObject generator(int polygon_count, int min_vertex, int max_vertex, QString polygon_type) {
    QJsonObject jsonData;
    QJsonObject areaObject;

    int columns = std::ceil(std::sqrt(polygon_count));
    int rows = std::ceil(static_cast<double>(polygon_count) / columns);

    int cell_size = 300;
    int area_width = columns * cell_size;
    int area_height = rows * cell_size * 10;

    areaObject["width"] = area_width;
    areaObject["height"] = area_height;
    jsonData["area"] = areaObject;

    QJsonArray polygonsArray;
    QMap<int, int> vertexCountMap;

    int index = 0;
    for (int row = 0; row < rows && index < polygon_count; ++row) {
        for (int col = 0; col < columns && index < polygon_count; ++col, ++index) {
            int vertexCount = QRandomGenerator::global()->bounded(min_vertex, max_vertex + 1);

            int cell_x = col * cell_size;
            int cell_y = row * cell_size;

            QJsonObject polygonObject;
            polygonObject["uid"] = "polygon" + QString::number(index + 1);
            polygonObject["coord"] = generateConvexPolygon(vertexCount, cell_x, cell_y, cell_size, cell_size);

            polygonsArray.append(polygonObject);
            vertexCountMap[vertexCount]++;
        }
    }

    jsonData["polygons"] = polygonsArray;

    QJsonObject analysisObject;
    for (auto it = vertexCountMap.begin(); it != vertexCountMap.end(); ++it) {
        analysisObject[QString::number(it.key()) + "_vertex_count"] = it.value();
    }
    jsonData["analysis"] = analysisObject;

    return jsonData;
}



void saveJsonToFile(const QJsonObject &jsonData, const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open file for writing.");
        return;
    }

    QJsonDocument jsonDoc(jsonData);
    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();
}