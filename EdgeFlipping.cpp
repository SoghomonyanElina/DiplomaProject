#include "EdgeFlipping.h"
#include <cmath>
#include <QDebug>

    bool EdgeFlipping::isPointInCircumcircle(const Triangle &t, const QPoint &p) {
    //Computing triangle's circle center and radius
    double A = t.b.x() - t.a.x();
    double B = t.b.y() - t.a.y();
    double C = t.c.x() - t.a.x();
    double D = t.c.y() - t.a.y();

    double E = A * (t.a.x() + t.b.x()) + B * (t.a.y() + t.b.y());
    double F = C * (t.a.x() + t.c.x()) + D * (t.a.y() + t.c.y());

    double G = 2.0 * (A * (t.c.y() - t.b.y()) - B * (t.c.x() - t.b.x()));

    if (std::abs(G) < 1e-10) {
        return false;
    }

    double centerX = (D * E - B * F) / G;
    double centerY = (A * F - C * E) / G;

    double radiusSquared = (t.a.x() - centerX) * (t.a.x() - centerX) +
                           (t.a.y() - centerY) * (t.a.y() - centerY);

    double distanceSquared = (p.x() - centerX) * (p.x() - centerX) +
                             (p.y() - centerY) * (p.y() - centerY);

    return distanceSquared < radiusSquared;
}

bool EdgeFlipping::isValidTriangle(const Triangle &t) {
    if (t.a == t.b && t.b == t.c &&  t.a == t.c) {
        return false;
    }

    double area = (t.b.x() - t.a.x()) * (t.c.y() - t.a.y()) -
                  (t.c.x() - t.a.x()) * (t.b.y() - t.a.y());
    return std::abs(area) > 1e-9;
}

bool EdgeFlipping::haveCommonEdge(const Triangle &t1, const Triangle &t2,
                                  QPoint &shared1, QPoint &shared2,
                                  QPoint &unique1, QPoint &unique2) {
    QVector<QPoint> points1 = {t1.a, t1.b, t1.c};
    QVector<QPoint> points2 = {t2.a, t2.b, t2.c};

    QVector<QPoint> shared, unique1List, unique2List;

    for (const QPoint &p1 : points1) {
        bool found = false;
        for (const QPoint &p2 : points2) {
            if (p1 == p2) {
                shared.append(p1);
                found = true;
                break;
            }
        }
        if (!found) {
            unique1List.append(p1);
        }
    }

    for (const QPoint &p2 : points2) {
        bool found = false;
        for (const QPoint &p1 : points1) {
            if (p2 == p1) {
                found = true;
                break;
            }
        }
        if (!found) {
            unique2List.append(p2);
        }
    }

    if (shared.size() == 2 && unique1List.size() == 1 && unique2List.size() == 1) {
        shared1 = shared[0];
        shared2 = shared[1];
        unique1 = unique1List[0];
        unique2 = unique2List[0];
        return true;
    }

    return false;
}

void EdgeFlipping::apply(QVector<Triangle> &triangles, int maxIterations) {
    bool flipped;
    int iteration = 0;
    do {
        flipped = false;
        iteration++;
        qDebug() << "Edge flipping iteration:" << iteration;

        QVector<QPair<int, int>> indicesToFlip;
        QVector<QPair<Triangle, Triangle>> newTriangles;

        for (int i = 0; i < triangles.size(); i++) {
            for (int j = i + 1; j < triangles.size(); j++) {
                QPoint shared1, shared2, unique1, unique2;
                if (haveCommonEdge(triangles[i], triangles[j], shared1, shared2, unique1, unique2)) {
                    if (isPointInCircumcircle(triangles[i], unique2) ||
                        isPointInCircumcircle(triangles[j], unique1)) {

                        Triangle newT1 = {shared1, unique1, unique2};
                        Triangle newT2 = {shared2, unique1, unique2};

                        if (isValidTriangle(newT1) && isValidTriangle(newT2)) {
                            bool isValidFlip = true;

                            if (isValidFlip) {
                                indicesToFlip.append(qMakePair(i, j));
                                newTriangles.append(qMakePair(newT1, newT2));
                                flipped = true;
                            }
                        }
                    }
                }
            }
        }

        for (int idx = 0; idx < indicesToFlip.size(); idx++) {
            int i = indicesToFlip[idx].first;
            int j = indicesToFlip[idx].second;
            Triangle newT1 = newTriangles[idx].first;
            Triangle newT2 = newTriangles[idx].second;

            triangles[i] = newT1;
            triangles[j] = newT2;
        }

        qDebug() << "Flipped edges:" << indicesToFlip.size();
    } while (flipped && iteration < maxIterations);
}
