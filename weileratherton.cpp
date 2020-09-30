#include "weileratherton.h"

WeilerAtherton::WeilerAtherton(const QVector<QPoint>& _mainPolygonOuterRingPoints,
                               const QVector<QVector<QPoint>>& _mainPolygonInnerRingsPoints,
                               const QVector<QPoint>& _tailorPolygonOuterRingPoints,
                               const QVector<QVector<QPoint>>& _tailorPolygonInnerRingsPoints):
    mainPolygonOuterRingPoints(_mainPolygonOuterRingPoints),
    mainPolygonInnerRingsPoints(_mainPolygonInnerRingsPoints),
    tailorPolygonOuterRingPoints(_tailorPolygonOuterRingPoints),
    tailorPolygonInnerRingsPoints(_tailorPolygonInnerRingsPoints)
{

}

void WeilerAtherton::solve(QVector<QVector<QPoint>>& result){
    /*
     * 记主多边形外、内环分别为A、B={B0, B1, ..., Bn}，裁剪多边形外、内环分别为C、D={D0, D1, ..., Dm}，
     * 则result = (A-B) intersect (C-D) = (A intersect (C-D)) - (B intersect (C-D))
     */

    // 先计算 A intersect (C-D)
    QVector<QVector<QPoint>> clippedOuterRing;
    firstStep(mainPolygonOuterRingPoints, clippedOuterRing);

    // 再计算 Bi intersect (C-D)
    QVector<QVector<QPoint>> clippedInnerRings;
    for(auto& innerRing: mainPolygonInnerRingsPoints){
        QVector<QVector<QPoint>> clippedInnerRing;
        firstStep(innerRing, clippedInnerRing);
        for(auto& ring: clippedInnerRing){
            clippedInnerRings.push_back(ring);
        }
    }

    //最后计算 (A intersect (C-D)) - (B intersect (C-D))
    result = clippedOuterRing;
    for(auto& clippedInnerRing: clippedInnerRings){
        QVector<QVector<QPoint>> newResult;
        secondStep(result, clippedInnerRing, newResult);
        result = newResult;
    }
}

void WeilerAtherton::firstStep(const QVector<QPoint>& pts, QVector<QVector<QPoint>>& output){
    /*
     * 对pts求出被裁剪多边形裁剪后的结果，存储在output内
     */
    QVector<GeneralPoint> intersectPts;
    generateIntersectPoints(pts, intersectPts);
}

void WeilerAtherton::secondStep(const QVector<QVector<QPoint>>& polygons, const QVector<QPoint>& polygon, QVector<QVector<QPoint>>& output){
    /*
     * 求出 output = polygons - polygon
     */
    // TODO
}

void WeilerAtherton::generateIntersectPoints(const QVector<QPoint>& pts,QVector<GeneralPoint>& output){
    /*
     *  求出pts和裁剪多边形的交点，存储在output内
     */

    const int numRings = tailorPolygonInnerRingsPoints.size()+1;
    const int ptsSize = pts.size();
    for(int k = 0; k < numRings; k ++){
        QVector<QPoint>* tailorPolygonRing;
        if(k==0){
            tailorPolygonRing = &tailorPolygonOuterRingPoints;
        }else{
            tailorPolygonRing = &(tailorPolygonInnerRingsPoints[k-1]);
        }
        const int clipSize = tailorPolygonRing->size();
        for(int i = 0; i < clipSize; i ++){
            auto p1 = (*tailorPolygonRing)[i];
            auto p2 = (*tailorPolygonRing)[(i+1)%clipSize];
            for(int j = 0; j < ptsSize; j ++){
                auto p3 = pts[j];
                auto p4 = pts[(j+1)%ptsSize];
                if(segmentsIntersect(p1, p2, p3, p4)){
                    GeneralPoint gp;
                    gp.pos = getintersectPoint(p1, p2, p3, p4);
                    gp.index0 = j;
                    gp.index1 = k;
                    gp.index2 = i;
                    output.push_back(gp);
                }
            }
        }
    }
}

bool WeilerAtherton::segmentsIntersect(QPoint p1, QPoint p2, QPoint p3, QPoint p4){
    // TODO
    return false;
}

QPoint WeilerAtherton::getintersectPoint(QPoint p1, QPoint p2, QPoint p3, QPoint p4){
    // TODO
    return QPoint(0, 0);
}
