#ifndef EARCLIPPING_H
#define EARCLIPPING_H

#include <QVector>
#include <QPoint>

class EarClipping {
public:
    static QVector<QVector<QPoint>> triangulate(const QVector<QPoint> &polygon);
};

#endif //EARCLIPPING_H