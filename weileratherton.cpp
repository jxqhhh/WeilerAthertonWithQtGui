#include "weileratherton.h"
#include <QMultiMap>

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

    /*

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

    */

    QVector<QVector<GeneralPoint>> mainPolygonList, tailorPolygonList;
    QVector<GeneralPoint> intersectPtsVector;

    generateIntersectPoints(intersectPtsVector);

    // 初始化mainPolygonList和tailorPolygonList，使得两者的第i个元素存储第i个环上的顶点、交叉点:
    generateList(intersectPtsVector, mainPolygonList, true);
    generateList(intersectPtsVector, tailorPolygonList, false);

}

void WeilerAtherton::generateIntersectPoints(QVector<GeneralPoint>& intersectPtsVector){
    /*
     * Generate intersect points:s
     */
    const int mainRingsNum = mainPolygonInnerRingsPoints.size()+1;
    const int tailorRingsNum = tailorPolygonInnerRingsPoints.size()+1;
    for(int i = 0; i < mainRingsNum; i ++){
        QVector<QPoint>* mainRing;
        if(i==0){
            mainRing = &mainPolygonOuterRingPoints;
        }else{
            mainRing = &(mainPolygonInnerRingsPoints[i-1]);
        }
        const int mainRingSize = mainRing->size();
        for(int j = 0; j < tailorRingsNum; j ++){
            QVector<QPoint>* tailorRing;
            if(j==0){
                tailorRing = &tailorPolygonOuterRingPoints;
            }else{
                tailorRing = &(tailorPolygonInnerRingsPoints[j-1]);
            }
            const int tailorRingSize = tailorRing->size();
            for(int k = 0; k < mainRingSize; k ++){
                auto& p1 = mainRing->operator[](k);
                auto& p2 = mainRing->operator[]((k+1)%mainRingSize);
                for(int l = 0; l < tailorRingSize; l ++){
                    auto& p3 = tailorRing->operator[](l);
                    auto& p4 = tailorRing->operator[]((l+1)%tailorRingSize);
                    if(segmentsIntersect(p1, p2, p3, p4)) {
                        GeneralPoint gp;
                        gp.pos = getintersectPoint(p1, p2, p3, p4);
                        gp.pointFlag = PointType::intersectionPoint;
                        gp.mainIndex.indexOfRing = i;
                        gp.mainIndex.indexOfEdge = k;
                        gp.tailorIndex.indexOfEdge = j;
                        gp.tailorIndex.indexOfRing = l;
                        intersectPtsVector.push_back(gp);
                    }
                }
            }
        }
    }
}

void WeilerAtherton::generateList(const QVector<GeneralPoint>& intersectPtsVector,
                                  QVector<QVector<GeneralPoint>>& outputResult,
                                  bool processingMainPolygon){
    /*
     *  若为主多边形，调用时processingMainPolygon为true；若为裁剪多边形，调用时processingMainPolygon为false
     */
    QVector<QPoint>* polygonOuterRing;
    QVector<QVector<QPoint>>* polygonInnerRings;
    if(processingMainPolygon){
        polygonOuterRing = &mainPolygonOuterRingPoints;
        polygonInnerRings = &mainPolygonInnerRingsPoints;
    }else{
        polygonOuterRing = &tailorPolygonOuterRingPoints;
        polygonInnerRings = &tailorPolygonInnerRingsPoints;
    }
    const int numRings = polygonInnerRings->size()+1;
    for(int i = 0; i < numRings; i ++){
        QVector<QPoint>* ring;
        QVector<GeneralPoint> outputForCurrentRing;
        if(i==0){
            ring = polygonOuterRing;
        }else{
            ring = &(polygonInnerRings->operator[](i-1));
        }
        const int numEdges = ring->size();
        for(int j = 0; j < numEdges; j ++){
            QPoint p1 = ring->operator[](j);
            GeneralPoint gp;
            gp.pointFlag = PointType::vertex;
            gp.pos = p1;
            outputForCurrentRing.push_back(gp);
            QMultiMap<int, GeneralPoint> oneSeg;
            for (auto it = intersectPtsVector.begin(); it != intersectPtsVector.end(); it++) {
                if ((processingMainPolygon && i == it->mainIndex.indexOfRing && j == it->mainIndex.indexOfEdge) ||
                    ((!processingMainPolygon) && i == it->tailorIndex.indexOfRing && j == it->tailorIndex.indexOfEdge)) {
                    oneSeg.insert(getDistance(it->pos, p1), *it);
                }
            }
            for(auto it = oneSeg.begin(); it != oneSeg.end(); it++){
                outputForCurrentRing.push_back(*it);
            }
        }
        outputResult.push_back(outputForCurrentRing);
    }
}

bool WeilerAtherton::segmentsIntersect(const QPoint& p1, const QPoint& p2, const QPoint& p3, const QPoint& p4){
    // TODO
    return false;
}

QPoint WeilerAtherton::getintersectPoint(const QPoint& p1, const QPoint& p2, const QPoint& p3, const QPoint& p4){
    // TODO
    return QPoint(0, 0);
}

int WeilerAtherton::getDistance(const QPoint &p1, const QPoint &p2){
    return (p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y());
}
