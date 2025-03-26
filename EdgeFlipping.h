#ifndef EDGEFLIPPING_H
#define EDGEFLIPPING_H
#include <QVector>
#include <QPoint>

struct Triangle {
    QPoint a, b, c;
};

class EdgeFlipping {
public:
    static void apply(QVector<Triangle> &triangles, int maxIterations = 1);

private:
    static bool isValidTriangle(const Triangle &t);
    static bool isPointInCircumcircle(const Triangle &t, const QPoint &p);
    static bool haveCommonEdge(const Triangle &t1, const Triangle &t2,
                               QPoint &shared1, QPoint &shared2,
                               QPoint &unique1, QPoint &unique2);
    static bool isConvexQuad(const QPoint &a, const QPoint &b, const QPoint &c, const QPoint &d);
};

#endif //EDGEFLIPPING_H