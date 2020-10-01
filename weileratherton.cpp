#include "weileratherton.h"
#include <QMultiMap>
#include <algorithm>
using std::min;
using std::max;

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

    QVector<QVector<GeneralPoint>> mainPolygonList, tailorPolygonList;
    QVector<GeneralPoint> intersectPtsVector;

    generateIntersectPoints(intersectPtsVector);

    // 初始化mainPolygonList和tailorPolygonList，使得两者的第i个元素存储第i个环上的顶点、交叉点:
    generateList(intersectPtsVector, mainPolygonList, true);
    generateList(intersectPtsVector, tailorPolygonList, false);

    setInFlag(mainPolygonList, tailorPolygonList);
    generateClipArea(mainPolygonList, tailorPolygonList, result);

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
                        gp.tailorIndex.indexOfRing = j;
                        gp.tailorIndex.indexOfEdge = l;
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

void WeilerAtherton::setInFlag(QVector<QVector<GeneralPoint>> &mainPolygonList, QVector<QVector<GeneralPoint>> &tailorPolygonList){
    // Step 1: set inFlag for mainPolygonList
    for(auto it = mainPolygonList.begin(); it != mainPolygonList.end(); it ++){
        bool inFlag;
        for(auto it2 = it->begin(); it2 != it->end(); it2 ++){
            if(it2->pointFlag == PointType::vertex){
                inFlag = isPointInsideTailorPolygon(it2->pos);
            }else{
                inFlag = !inFlag;
                it2->inFlag = inFlag;
            }
        }
    }
    for(auto mainIt = mainPolygonList.begin(); mainIt != mainPolygonList.end(); mainIt ++){
        for(auto mainIt2 = mainIt->begin(); mainIt2 != mainIt->end(); mainIt2 ++){
            if(mainIt2->pointFlag==PointType::vertex){
                continue;
            }
            auto& tailorRing = tailorPolygonList[mainIt2->tailorIndex.indexOfRing];
            for(auto tailorIt = tailorRing.begin(); tailorIt != tailorRing.end(); tailorIt ++){
                if(tailorIt->pointFlag==PointType::vertex){
                    continue;
                }
                if(tailorIt->pos.x()==mainIt2->pos.x()
                        && tailorIt->pos.y()==mainIt2->pos.y()){
                    tailorIt->inFlag = mainIt2->inFlag;
                }
            }
        }
    }
}

void WeilerAtherton::generateClipArea(QVector<QVector<GeneralPoint>> &mainPolygonList,
                                      QVector<QVector<GeneralPoint>> &tailorPolygonList,
                                      QVector<QVector<QPoint>> &result){

    QVector<QPoint> clipResult;
    while(true){
        for(auto mainIt = mainPolygonList.begin(); mainIt != mainPolygonList.end(); mainIt ++){
            auto mainIt2 = mainIt->begin();
            for(; mainIt2 != mainIt->end(); mainIt2 ++){
                if(!(mainIt2->processed) && mainIt2->pointFlag == PointType::intersectionPoint && mainIt2->inFlag){
                    mainIt2->processed = true;
                    break; // 循环直到找到主多边形上一个未处理的入点
                }
            }
            if(mainIt2 == mainIt->end()){
                continue;
            }
            while(true){
                while(true){ // 在主多边形的当前环上循环直到找到下一个出点
                    clipResult.push_back(mainIt2->pos);
                    if(!(mainIt2->processed) && mainIt2->pointFlag == PointType::intersectionPoint && !mainIt2->inFlag){
                        mainIt2->processed = true;
                        break;
                    }
                    mainIt2->processed = true;
                    //clipResult.push_back(mainIt2->pos);
                    mainIt2 ++;
                    if(mainIt2 == mainIt->end()){
                        mainIt2 = mainIt->begin();
                    }
                }
                auto& tailorRing = tailorPolygonList[mainIt2->tailorIndex.indexOfRing];
                auto tailorIt = tailorRing.begin();
                while(true){
                    if(tailorIt->pos.x()==mainIt2->pos.x() && tailorIt->pos.y()==mainIt2->pos.y()){
                        break;
                    }
                    tailorIt ++;
                }
                while(true){ // 在裁剪多边形的当前环上循环直到找到下一个入点
                    if(!(tailorIt->processed) && tailorIt->pointFlag == PointType::intersectionPoint && tailorIt->inFlag){
                        tailorIt->processed = true;
                        break;
                    }
                    tailorIt->processed = true;
                    clipResult.push_back(tailorIt->pos);
                    tailorIt++;
                    if(tailorIt==tailorRing.end()){
                        tailorIt = tailorRing.begin();
                    }
                }
                if(tailorIt->pos.x()==clipResult[0].x() && tailorIt->pos.y()==clipResult[0].y()){
                    break; // 这种情况下已经找到一个闭合的多边形
                }else{
                    mainIt = &(mainPolygonList[tailorIt->mainIndex.indexOfRing]);
                    mainIt2 = mainIt->begin();
                    while(true){
                        if(tailorIt->pos.x()==mainIt2->pos.x() && tailorIt->pos.y()==mainIt2->pos.y()){
                            break;
                        }
                        mainIt2 ++;
                    }
                }
            }
            break;
        }
        if(clipResult.size()==0){
            break;
        }else{
            result.push_back(clipResult);
            clipResult.clear();
        }
    }
}

int WeilerAtherton::cross(const QPoint &p0, const QPoint &p1, const QPoint &p2){
    return ((p2.x() - p0.x()) * (p1.y() - p0.y()) - (p1.x() - p0.x()) * (p2.y() - p0.y()));
}

bool WeilerAtherton::onSegment(const QPoint &p0, const QPoint &p1, const QPoint &p2){
    int minx = min(p0.x(), p1.x()), maxx = max(p0.x(), p1.x());
    int miny = min(p0.y(), p1.y()), maxy = max(p0.y(), p1.y());
    if (p2.x() >= minx && p2.x() <= maxx && p2.y() >= miny && p2.y() <= maxy) return true;
    return false;
}

bool WeilerAtherton::segmentsIntersect(const QPoint& p1, const QPoint& p2, const QPoint& p3, const QPoint& p4){
    int d1 = cross(p3, p4, p1);
    int d2 = cross(p3, p4, p2);
    int d3 = cross(p1, p2, p3);
    int d4 = cross(p1, p2, p4);
    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
        ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
        return true;
    if (d1 == 0 && onSegment(p3, p4, p1)) return true;
    if (d2 == 0 && onSegment(p3, p4, p2)) return true;
    if (d3 == 0 && onSegment(p1, p2, p3)) return true;
    if (d4 == 0 && onSegment(p1, p2, p4)) return true;
    return false;
}

QPoint WeilerAtherton::getintersectPoint(const QPoint& p1, const QPoint& p2, const QPoint& p3, const QPoint& p4){
    QPoint p;
    int b1 = (p2.y() - p1.y()) * p1.x() + (p1.x() - p2.x()) * p1.y();
    int b2 = (p4.y() - p3.y()) * p3.x() + (p3.x() - p4.x()) * p3.y();
    int D = (p2.x() - p1.x()) * (p4.y() - p3.y()) - (p4.x() - p3.x()) * (p2.y() - p1.y());
    int D1 = b2 * (p2.x() - p1.x()) - b1 * (p4.x() - p3.x());
    int D2 = b2 * (p2.y() - p1.y()) - b1 * (p4.y() - p3.y());
    p.setX(D1/D);
    p.setY(D2/D);
    return p;
}

int WeilerAtherton::getDistance(const QPoint &p1, const QPoint &p2){
    return (p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y());
}

bool WeilerAtherton::isPointInsidePg(const QPoint &p, const QVector<QPoint> &py){
    int cnt = 0, size = py.size();
        for (int i = 0; i < size; i++) {
            auto& p1 = py[i];
            auto& p2 = py[(i + 1) % size];
            if (p1.y() == p2.y()) continue;
            if (p.y() < min(p1.y(), p2.y())) continue;
            if (p.y() >= max(p1.y(), p2.y())) continue;
            double x = (double)(p.y() - p1.y()) * (double)(p2.x() - p1.x()) / (double)(p2.y() - p1.y()) + p1.x();
            if (x > p.x()) cnt++;
        }
        return (cnt % 2 == 1);
}

bool WeilerAtherton::isPointInsideTailorPolygon(const QPoint &p){
    if(!isPointInsidePg(p, tailorPolygonOuterRingPoints)){
        return false;
    }
    for(auto& ring: tailorPolygonInnerRingsPoints){
        if(isPointInsidePg(p, ring)){
            return false;
        }
    }
    return true;
}
