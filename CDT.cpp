#include "CDT.h"
#include <cmath>
#include <algorithm>
#include <QMessageBox>

QVector<Triangle> CDT::triangulate(const QVector<QPoint>& points) {
    if (points.size() < 3) return {};

    QVector<QPoint> workingPoints = points;
    int n = points.size();

    int minX = points[0].x(), maxX = points[0].x();
    int minY = points[0].y(), maxY = points[0].y();
    for (const QPoint& p : points) {
        minX = std::min(minX, p.x());
        maxX = std::max(maxX, p.x());
        minY = std::min(minY, p.y());
        maxY = std::max(maxY, p.y());
    }

    //Creating Super Triangle
    int dx = (maxX - minX) * 100;
    int dy = (maxY - minY) * 100;
    QPoint p1(minX - dx, minY - dy);
    QPoint p2(maxX + dx, minY - dy);
    QPoint p3((minX + maxX) / 2, maxY + dy);

    //all points with Super Triangle points
    workingPoints.append(p1);
    workingPoints.append(p2);
    workingPoints.append(p3);

    int super1 = n, super2 = n + 1, super3 = n + 2;
    QVector<IndexedTriangle> triangles;
    triangles.append(IndexedTriangle(super1, super2, super3));

    //Bowyer-Watson algorithm
    for (int i = 0; i < n; ++i) {
        QVector<IndexedTriangle> badTriangles;
        QVector<Edge> polygon;

        //Find triangles that will be remove
        for (int j = 0; j < triangles.size(); j++) {
            const IndexedTriangle& t = triangles[j];

            if (isInCircumcircle(workingPoints[t.v1], workingPoints[t.v2], workingPoints[t.v3], points[i])) {
                badTriangles.append(t);
            }
        }

        for (const IndexedTriangle& t : badTriangles) {
            Edge e1(t.v1, t.v2);
            Edge e2(t.v2, t.v3);
            Edge e3(t.v3, t.v1);

            bool isShared1 = false, isShared2 = false, isShared3 = false;

            for (const IndexedTriangle& other : badTriangles) {
                if (t == other) continue;

                if ((other.v1 == e1.v1 && other.v2 == e1.v2) || (other.v1 == e1.v2 && other.v2 == e1.v1) ||
                    (other.v2 == e1.v1 && other.v3 == e1.v2) || (other.v2 == e1.v2 && other.v3 == e1.v1) ||
                    (other.v3 == e1.v1 && other.v1 == e1.v2) || (other.v3 == e1.v2 && other.v1 == e1.v1)) {
                    isShared1 = true;
                }

                if ((other.v1 == e2.v1 && other.v2 == e2.v2) || (other.v1 == e2.v2 && other.v2 == e2.v1) ||
                    (other.v2 == e2.v1 && other.v3 == e2.v2) || (other.v2 == e2.v2 && other.v3 == e2.v1) ||
                    (other.v3 == e2.v1 && other.v1 == e2.v2) || (other.v3 == e2.v2 && other.v1 == e2.v1)) {
                    isShared2 = true;
                }

                if ((other.v1 == e3.v1 && other.v2 == e3.v2) || (other.v1 == e3.v2 && other.v2 == e3.v1) ||
                    (other.v2 == e3.v1 && other.v3 == e3.v2) || (other.v2 == e3.v2 && other.v3 == e3.v1) ||
                    (other.v3 == e3.v1 && other.v1 == e3.v2) || (other.v3 == e3.v2 && other.v1 == e3.v1)) {
                    isShared3 = true;
                }
            }

            if (!isShared1) polygon.append(e1);
            if (!isShared2) polygon.append(e2);
            if (!isShared3) polygon.append(e3);
        }

        //Remove bad triangles
        for (const auto& t : badTriangles) {
            triangles.removeOne(t);
        }

        for (const auto& edge : polygon) {
            triangles.append(IndexedTriangle(edge.v1, edge.v2, i));
        }
    }

    //Remove Super Triangle's triangles
    QVector<IndexedTriangle> filteredBySuperTriangle;
    for (const auto& t : triangles) {
        if (t.v1 < n && t.v2 < n && t.v3 < n) {
            filteredBySuperTriangle.append(t);
        }
    }

    //Check is triangle inside polygon
    QVector<IndexedTriangle> filteredByPolygon;
    for (const auto& t : filteredBySuperTriangle) {
        QPoint center((points[t.v1].x() + points[t.v2].x() + points[t.v3].x()) / 3,
                      (points[t.v1].y() + points[t.v2].y() + points[t.v3].y()) / 3);

        if (isPointInPolygon(center, points) ||
            isPointInPolygon(points[t.v1], points) ||
            isPointInPolygon(points[t.v2], points) ||
            isPointInPolygon(points[t.v3], points)) {
            filteredByPolygon.append(t);
        }
    }

    QVector<Edge> constraints;
    for (int i = 0; i < n; ++i) {
        constraints.append(Edge(i, (i + 1) % n));
    }

    restoreDelaunayProperty(filteredByPolygon, points, constraints);

    ensureCompleteTriangulation(filteredByPolygon, points, n);

    return convertToTriangles(filteredByPolygon, points);
}

void CDT::ensureCompleteTriangulation(QVector<IndexedTriangle>& triangles, const QVector<QPoint>& points, int n) {
    QSet<int> usedPoints;
    for (const auto& t : triangles) {
        usedPoints.insert(t.v1);
        usedPoints.insert(t.v2);
        usedPoints.insert(t.v3);
    }

    QVector<int> unusedPoints;
    for (int i = 0; i < n; ++i) {
        if (!usedPoints.contains(i)) {
            unusedPoints.append(i);
        }
    }

    for (int unusedPoint : unusedPoints) {
        int nearestPoint = -1;
        int minDistanceSq = std::numeric_limits<int>::max();

        for (int i = 0; i < n; ++i) {
            if (i == unusedPoint) continue;
            if (usedPoints.contains(i)) {
                int dx = points[i].x() - points[unusedPoint].x();
                int dy = points[i].y() - points[unusedPoint].y();
                int distSq = dx * dx + dy * dy;
                if (distSq < minDistanceSq) {
                    minDistanceSq = distSq;
                    nearestPoint = i;
                }
            }
        }

        if (nearestPoint != -1) {
            int thirdPoint = -1;
            for (const auto& t : triangles) {
                if (t.v1 == nearestPoint || t.v2 == nearestPoint || t.v3 == nearestPoint) {
                    if (t.v1 != nearestPoint && t.v1 < n) thirdPoint = t.v1;
                    else if (t.v2 != nearestPoint && t.v2 < n) thirdPoint = t.v2;
                    else if (t.v3 != nearestPoint && t.v3 < n) thirdPoint = t.v3;
                    break;
                }
            }

            if (thirdPoint != -1) {
                triangles.append(IndexedTriangle(unusedPoint, nearestPoint, thirdPoint));
                usedPoints.insert(unusedPoint);
            }
        }
    }

    QVector<Edge> polygonEdges;
    for (int i = 0; i < n; ++i) {
        polygonEdges.append(Edge(i, (i + 1) % n));
    }

    for (const Edge& edge : polygonEdges) {
        bool edgeFound = false;
        for (const auto& t : triangles) {
            if ((t.v1 == edge.v1 && t.v2 == edge.v2) || (t.v2 == edge.v1 && t.v3 == edge.v2) ||
                (t.v3 == edge.v1 && t.v1 == edge.v2) || (t.v1 == edge.v2 && t.v2 == edge.v1) ||
                (t.v2 == edge.v2 && t.v3 == edge.v1) || (t.v3 == edge.v2 && t.v1 == edge.v1)) {
                edgeFound = true;
                break;
            }
        }

        if (!edgeFound) {
            for (int i = 0; i < n; ++i) {
                if (i != edge.v1 && i != edge.v2) {
                    QPoint center((points[edge.v1].x() + points[edge.v2].x() + points[i].x()) / 3,
                                  (points[edge.v1].y() + points[edge.v2].y() + points[i].y()) / 3);
                    if (isPointInPolygon(center, points)) {
                        triangles.append(IndexedTriangle(edge.v1, edge.v2, i));
                        break;
                    }
                }
            }
        }
    }
}

bool CDT::isInCircumcircle(const QPoint& a, const QPoint& b, const QPoint& c, const QPoint& p) {
    double ax = a.x() - p.x(), ay = a.y() - p.y();
    double bx = b.x() - p.x(), by = b.y() - p.y();
    double cx = c.x() - p.x(), cy = c.y() - p.y();

    double ab = ax * ax + ay * ay;
    double bc = bx * bx + by * by;
    double ca = cx * cx + cy * cy;

    double det = ax * (by * ca - bc * ay) - ay * (bx * ca - bc * ax) + ab * (bx * cy - by * cx);
    return det > 0;
}

bool CDT::isPointInPolygon(const QPoint& point, const QVector<QPoint>& polygon) {
    bool inside = false;
    int i, j;
    for (i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
        if (((polygon[i].y() > point.y()) != (polygon[j].y() > point.y())) &&
            (point.x() < (polygon[j].x() - polygon[i].x()) * (point.y() - polygon[i].y()) /
                                 double(polygon[j].y() - polygon[i].y()) + polygon[i].x())) {
            inside = !inside;
        }
    }
    return inside;
}

QMap<Edge, QSet<int>> CDT::buildEdgeToTrianglesMap(const QVector<IndexedTriangle>& triangles) {
    QMap<Edge, QSet<int>> map;
    for (int i = 0; i < triangles.size(); ++i) {
        const auto& t = triangles[i];
        map[Edge(t.v1, t.v2)].insert(i);
        map[Edge(t.v2, t.v3)].insert(i);
        map[Edge(t.v3, t.v1)].insert(i);
    }
    return map;
}

int CDT::getOpposingVertex(const IndexedTriangle& t, const Edge& e) {
    if (t.v1 != e.v1 && t.v1 != e.v2) return t.v1;
    if (t.v2 != e.v1 && t.v2 != e.v2) return t.v2;
    return t.v3;
}

void CDT::restoreDelaunayProperty(QVector<IndexedTriangle>& triangles, const QVector<QPoint>& points, const QVector<Edge>& constraints) {
    bool flipped;
    do {
        flipped = false;
        auto edgeMap = buildEdgeToTrianglesMap(triangles);

        for (auto it = edgeMap.begin(); it != edgeMap.end(); ++it) {
            const Edge& edge = it.key();
            bool isConstraint = false;
            for (const Edge& constraint : constraints) {
                if (edge.v1 == constraint.v1 && edge.v2 == constraint.v2) {
                    isConstraint = true;
                    break;
                }
            }

            if (isConstraint || it.value().size() != 2)
                continue;

            QVector<int> triIndices = it.value().values().toVector();
            auto& t1 = triangles[triIndices[0]];
            auto& t2 = triangles[triIndices[1]];

            int o1 = getOpposingVertex(t1, edge);
            int o2 = getOpposingVertex(t2, edge);

            if (isInCircumcircle(points[t1.v1], points[t1.v2], points[t1.v3], points[o2])) {
                t1 = IndexedTriangle(edge.v1, o1, o2);
                t2 = IndexedTriangle(edge.v2, o1, o2);
                flipped = true;
                break;
            }
        }
    } while (flipped);
}

QVector<Triangle> CDT::convertToTriangles(const QVector<IndexedTriangle>& indexedTriangles, const QVector<QPoint>& points) {
    QVector<Triangle> result;
    for (const auto& t : indexedTriangles) {
        result.append(Triangle(points[t.v1], points[t.v2], points[t.v3]));
    }
    return result;
}