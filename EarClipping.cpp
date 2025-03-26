#include "EarClipping.h"
#include <cmath>
#include <algorithm>

bool isCounterClockwise(const QVector<QPoint> &polygon) {
    int sum = 0;
    for (int i = 0; i < polygon.size(); i++) {
        QPoint p1 = polygon[i];
        QPoint p2 = polygon[(i + 1) % polygon.size()];
        sum += (p2.x() - p1.x()) * (p2.y() + p1.y());
    }
    return sum < 0;
}

bool isConvex(const QPoint &a, const QPoint &b, const QPoint &c) {
    return ((b.x() - a.x()) * (c.y() - a.y()) - (b.y() - a.y()) * (c.x() - a.x())) > 0;
}

bool isPointInsideTriangle(const QPoint &p, const QPoint &a, const QPoint &b, const QPoint &c) {
    double areaOrig = std::abs((b.x() - a.x()) * (c.y() - a.y()) - (b.y() - a.y()) * (c.x() - a.x()));
    double area1 = std::abs((a.x() - p.x()) * (b.y() - p.y()) - (a.y() - p.y()) * (b.x() - p.x()));
    double area2 = std::abs((b.x() - p.x()) * (c.y() - p.y()) - (b.y() - p.y()) * (c.x() - p.x()));
    double area3 = std::abs((c.x() - p.x()) * (a.y() - p.y()) - (c.y() - p.y()) * (a.x() - p.x()));

    return std::abs(areaOrig - (area1 + area2 + area3)) < 1e-9;
}

QVector<QVector<QPoint>> EarClipping::triangulate(const QVector<QPoint> &polygon) {
    QVector<QVector<QPoint>> triangles;
    QVector<QPoint> remainingPoints = polygon;

    if (!isCounterClockwise(remainingPoints)) {
        std::reverse(remainingPoints.begin(), remainingPoints.end());
    }

    while (remainingPoints.size() > 3) {
        bool earFound = false;
        for (int i = 0; i < remainingPoints.size(); i++) {
            int prev = (i - 1 + remainingPoints.size()) % remainingPoints.size();
            int next = (i + 1) % remainingPoints.size();

            QPoint a = remainingPoints[prev];
            QPoint b = remainingPoints[i];
            QPoint c = remainingPoints[next];

            if (isConvex(a, b, c)) {
                bool ear = true;
                for (const QPoint &p : remainingPoints) {
                    if (p != a && p != b && p != c && isPointInsideTriangle(p, a, b, c)) {
                        ear = false;
                        break;
                    }
                }

                if (ear) {
                    triangles.append({a, b, c});
                    remainingPoints.removeAt(i);
                    earFound = true;
                    break;
                }
            }
        }

        if (!earFound) {
            qWarning("No valid ear found, stopping!");
            break;
        }
    }

    if (remainingPoints.size() == 3) {
        triangles.append({remainingPoints[0], remainingPoints[1], remainingPoints[2]});
    }

    return triangles;
}