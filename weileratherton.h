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

struct GeneralPoint{
    PointType pointFlag;
    Index mainIndex; // 表示点在主多边形的第几个环的第几条边上
    Index tailorIndex; // 表示点在裁剪多边形的第几个环的第几条边上
    QPoint pos;
    bool inFlag;
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
private:
    void generateIntersectPoints(QVector<GeneralPoint>& intersectPtsVector);
    void generateList(const QVector<GeneralPoint>& intersectPtsVector,
                      QVector<QVector<GeneralPoint>>& comList,
                      bool processingMainPolygon);
};

#endif // WEILERATHERTON_H
