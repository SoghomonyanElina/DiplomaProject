#ifndef CDT_H
#define CDT_H
#include <QVector>
#include <QPoint>
#include <QMap>
#include <QSet>
struct Edge {
    int v1, v2;
    Edge() : v1(-1), v2(-1) {}
    Edge(int v1, int v2) {
        this->v1 = std::min(v1, v2);
        this->v2 = std::max(v1, v2);
    }
    bool operator==(const Edge& other) const {
        return v1 == other.v1 && v2 == other.v2;
    }
    bool operator<(const Edge& other) const {
        return v1 < other.v1 || (v1 == other.v1 && v2 < other.v2);
    }
};
struct Triangle {
    QPoint a, b, c;
    Triangle() {}
    Triangle(const QPoint& a, const QPoint& b, const QPoint& c) : a(a), b(b), c(c) {}
    bool operator==(const Triangle& other) const {
        return (a == other.a && b == other.b && c == other.c) ||
               (a == other.b && b == other.c && c == other.a) ||
               (a == other.c && b == other.a && c == other.b);
    }
};
class CDT {
public:
    static QVector<Triangle> triangulate(const QVector<QPoint>& points);
private:
    struct IndexedTriangle {
        int v1, v2, v3;
        IndexedTriangle() : v1(-1), v2(-1), v3(-1) {}
        IndexedTriangle(int v1, int v2, int v3) : v1(v1), v2(v2), v3(v3) {}
        bool operator==(const IndexedTriangle& other) const {
            return (v1 == other.v1 && v2 == other.v2 && v3 == other.v3) ||
                   (v1 == other.v2 && v2 == other.v3 && v3 == other.v1) ||
                   (v1 == other.v3 && v2 == other.v1 && v3 == other.v2);
        }
    };
    static bool isInCircumcircle(const QPoint& a, const QPoint& b, const QPoint& c, const QPoint& p);
    static QMap<Edge, QSet<int>> buildEdgeToTrianglesMap(const QVector<IndexedTriangle>& triangles);
    static int getOpposingVertex(const IndexedTriangle& t, const Edge& e);
    static void restoreDelaunayProperty(QVector<IndexedTriangle>& triangles, const QVector<QPoint>& points, const QVector<Edge>& constraints);
    static bool isPointInPolygon(const QPoint& point, const QVector<QPoint>& polygon);
    static QVector<Triangle> convertToTriangles(const QVector<IndexedTriangle>& indexedTriangles, const QVector<QPoint>& points);
    static void ensureCompleteTriangulation(QVector<IndexedTriangle>& triangles, const QVector<QPoint>& points, int n);
};
#endif // CDT_H