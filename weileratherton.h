#ifndef WEILERATHERTON_H
#define WEILERATHERTON_H

#include <QVector>
#include <QPoint>

enum PointType{
    vertex,
    intersectionPoint
};

struct Index{
    int indexOfRing; // 外环索引为0，内环的索引从1开始
    int indexOfEdge;
};

enum InFlag{
    uninitialized,
    in, // 入点
    out // 出点
};

struct GeneralPoint{
    PointType pointFlag;
    Index mainIndex; // 表示交叉点在主多边形的第几个环的第几条边上，有pointFlag==PointType::intersectionPoint时才会设置该变量
    Index tailorIndex; // 表示交叉点在裁剪多边形的第几个环的第几条边上，有pointFlag==PointType::intersectionPoint时才会设置该变量
    QPoint pos;
    bool processed = false;
    InFlag inFlag=InFlag::uninitialized; // 只有pointFlag==PointType::intersectionPoint时才会设置该变量
};

class WeilerAtherton
{
    QVector<QPoint> mainPolygonOuterRingPoints;
    QVector<QVector<QPoint>> mainPolygonInnerRingsPoints;
    QVector<QPoint> tailorPolygonOuterRingPoints;
    QVector<QVector<QPoint>> tailorPolygonInnerRingsPoints;
public:
    WeilerAtherton(const QVector<QPoint>& _mainPolygonOuterRingPoints,
                   const QVector<QVector<QPoint>>& _mainPolygonInnerRingsPoints,
                   const QVector<QPoint>& _tailorPolygonOuterRingPoints,
                   const QVector<QVector<QPoint>>& _tailorPolygonInnerRingsPoints);
    void solve(QVector<QVector<QPoint>>& result);
    static bool segmentsIntersect(const QPoint& p1, const QPoint& p2, const QPoint& p3, const QPoint& p4);
    static QPoint getintersectPoint(const QPoint& p1, const QPoint& p2, const QPoint& p3, const QPoint& p4);
    static int getDistance(const QPoint& p1, const QPoint& p2);
    static int cross(const QPoint& p0, const QPoint& p1, const QPoint& p2);
    static bool onSegment(const QPoint& p0, const QPoint& p1, const QPoint& p2);
    static bool isPointInsidePg(const QPoint& p, const QVector<QPoint>& py);
private:
    void generateIntersectPoints(QVector<GeneralPoint>& intersectPtsVector);
    void generateList(const QVector<GeneralPoint>& intersectPtsVector,
                      QVector<QVector<GeneralPoint>>& comList,
                      bool processingMainPolygon);
    void setInFlag(QVector<QVector<GeneralPoint>>& mainPolygonList, QVector<QVector<GeneralPoint>>& tailorPolygonList);
    void generateClipArea(QVector<QVector<GeneralPoint>>& mainPolygonList,
                          QVector<QVector<GeneralPoint>>& tailorPolygonList,
                          QVector<QVector<QPoint>>& result);
    bool isPointInsideTailorPolygon(const QPoint& p);
};

#endif // WEILERATHERTON_H
