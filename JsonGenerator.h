#ifndef JSONGENERATOR_H
#define JSONGENERATOR_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QRandomGenerator>
#include <QMap>
#include <QPoint>

QJsonObject generator(int polygon_count, int min_polygon_vertex_count, int max_polygon_vertex_count, QString polygon_type);
void saveJsonToFile(const QJsonObject &jsonData, const QString &fileName);

#endif //JSONGENERATOR_H