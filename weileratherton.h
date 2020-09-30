#ifndef WEILERATHERTON_H
#define WEILERATHERTON_H

#include <QVector>
#include <QPoint>

enum PointType{
    vertex,
    intersectionPoint
};

struct GeneralPoint{
    PointType pointFlag;
    int index0; // 表示点在主多边形的第几条边上
    int index1; // 表示点在裁剪多边形的第几个环上（外环为0号环，内环的编号从1开始）
    int index2; // 表示点在裁剪多边形的第index1环上的第几个边上
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
    static bool segmentsIntersect(QPoint p1, QPoint p2, QPoint p3, QPoint p4);
    static QPoint getintersectPoint(QPoint p1, QPoint p2, QPoint p3, QPoint p4);
private:
    void firstStep(const QVector<QPoint>& pts, QVector<QVector<QPoint>>& output);
    void generateIntersectPoints(const QVector<QPoint>& pts,QVector<GeneralPoint>& output);
    void secondStep(const QVector<QVector<QPoint>>& polygons, const QVector<QPoint>& clippingPolygon, QVector<QVector<QPoint>>& output);
};

#endif // WEILERATHERTON_H
