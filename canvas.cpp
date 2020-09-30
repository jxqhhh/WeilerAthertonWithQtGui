#include "canvas.h"
#include "ui_canvas.h"

Canvas::Canvas(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Canvas)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);
}

Canvas::~Canvas()
{
    delete ui;
}

const QColor Canvas::mainPolygonPtsColor = Qt::blue;
const QColor Canvas::tailorPolygonPtsColor = Qt::red;

void Canvas::mouseReleaseEvent(QMouseEvent* e){
    State s = mainwindow->getState();
    if(s==State::paintingMainPolygonInnerRing){
        if(newMainPolygonInnerRing){
            QVector<QPoint> pts;
            pts.push_back(e->pos());
            newMainPolygonInnerRing = false;
            mainPolygonInnerRingsPoints.push_back(pts);
        }else{
            auto& pts = *(mainPolygonInnerRingsPoints.end()-1);
            pts.push_back(e->pos());
        }
        update();
    }else if(s==State::paintingMainPolygonOuterRing){
        mainPolygonOuterRingPoints.push_back(e->pos());
        update();
    }else if(s==State::paintingTailorPolygonInnerRing){
        if(newTailorPolygonInnerRing){
            QVector<QPoint> pts;
            pts.push_back(e->pos());
            newTailorPolygonInnerRing = false;
            tailorPolygonInnerRingsPoints.push_back(pts);
        }else{
            auto& pts = *(tailorPolygonInnerRingsPoints.end()-1);
            pts.push_back(e->pos());
        }
        update();
    }else if(s==State::paintingTailorPolygonOuterRing){
        tailorPolygonOuterRingPoints.push_back(e->pos());
        update();
    }else if(s==State::notPainting){
    }
}

void Canvas::setMainWindow(MainWindow* m){
    mainwindow = m;
}

void Canvas::paintEvent(QPaintEvent* e){
    QPainter painter(this);
    QPen pen;

    // paint mainPolygonOuterRing:
    pen.setWidth(outerRingPtsWdith);
    pen.setColor(Canvas::mainPolygonPtsColor);
    painter.setPen(pen);
    if(mainPolygonOuterRingEdges.size()>0){
        for(auto pts: mainPolygonOuterRingEdges){
            painter.drawLine(pts.first, pts.second);
        }
    }else{
        for(auto p: mainPolygonOuterRingPoints){
            painter.drawPoint(p);
        }
    }

    // paint mainPolygonInnerRings:
    pen.setWidth(innerRingPtsWdith);
    painter.setPen(pen);
    if(mainPolygonInnerRingsPoints.size()>mainPolygonInnerRingsEdges.size()){
        for(auto p: *(mainPolygonInnerRingsPoints.end()-1)){
            painter.drawPoint(p);
        }
    }
    for(auto edge: mainPolygonInnerRingsEdges){
        for(auto pts: edge){
            painter.drawLine(pts.first, pts.second);
        }
    }

    // paint tailorPolygonOuterRing:
    pen.setWidth(outerRingPtsWdith);
    pen.setColor(Canvas::tailorPolygonPtsColor);
    painter.setPen(pen);
    if(tailorPolygonOuterRingEdges.size()>0){
        for(auto pts: tailorPolygonOuterRingEdges){
            painter.drawLine(pts.first, pts.second);
        }
    }else{
        for(auto p: tailorPolygonOuterRingPoints){
            painter.drawPoint(p);
        }
    }

    // paint tailorPolygonInnerRings:
    pen.setWidth(innerRingPtsWdith);
    painter.setPen(pen);
    if(tailorPolygonInnerRingsPoints.size()>tailorPolygonInnerRingsEdges.size()){
        for(auto p: *(tailorPolygonInnerRingsPoints.end()-1)){
            painter.drawPoint(p);
        }
    }
    for(auto edge: tailorPolygonInnerRingsEdges){
        for(auto pts: edge){
            painter.drawLine(pts.first, pts.second);
        }
    }
}

void Canvas::clear(){
    mainPolygonOuterRingPoints.clear();
    mainPolygonInnerRingsPoints.clear();
    tailorPolygonOuterRingPoints.clear();
    tailorPolygonInnerRingsPoints.clear();

    mainPolygonOuterRingEdges.clear();
    mainPolygonInnerRingsEdges.clear();
    tailorPolygonOuterRingEdges.clear();
    tailorPolygonInnerRingsEdges.clear();

    clippedResult.clear();
}

bool Canvas::finishAMainPolygonInnerRing(){
    if(mainPolygonInnerRingsPoints.size()==0){
        return false;
    }
    auto pts = *(mainPolygonInnerRingsPoints.end()-1);
    const int size = pts.size();
    if(size<=2){
        return false;
    }
    QVector<QPair<QPoint, QPoint>> edges;
    for(int i = 0; i < size-1; i ++){
        edges.push_back(QPair<QPoint, QPoint>(pts[i], pts[i+1]));
    }
    edges.push_back(QPair<QPoint, QPoint>(pts[size-1], pts[0]));
    mainPolygonInnerRingsEdges.push_back(edges);
    return true;
}

bool Canvas::finishATailorPolygonInnerRing(){
    if(tailorPolygonInnerRingsPoints.size()==0){
        return false;
    }
    auto pts = *(tailorPolygonInnerRingsPoints.end()-1);
    const int size = pts.size();
    if(size<=2){
        return false;
    }
    QVector<QPair<QPoint, QPoint>> edges;
    for(int i = 0; i < size-1; i ++){
        edges.push_back(QPair<QPoint, QPoint>(pts[i], pts[i+1]));
    }
    edges.push_back(QPair<QPoint, QPoint>(pts[size-1], pts[0]));
    tailorPolygonInnerRingsEdges.push_back(edges);
    return true;
}

bool Canvas::finishMainPolygonOuterRing(){
    if(mainPolygonOuterRingPoints.size()<=2){
        return false;
    }
    mainPolygonOuterRingEdges.clear();
    const int size = mainPolygonOuterRingPoints.size();
    for(int i = 0; i < size-1; i ++){
        mainPolygonOuterRingEdges.push_back(QPair<QPoint, QPoint>(mainPolygonOuterRingPoints[i], mainPolygonOuterRingPoints[i+1]));
    }
    mainPolygonOuterRingEdges.push_back(QPair<QPoint, QPoint>(mainPolygonOuterRingPoints[size-1], mainPolygonOuterRingPoints[0]));
    return true;
}

bool Canvas::finishTailorPolygonOuterRing(){
    if(tailorPolygonOuterRingPoints.size()<=2){
        return false;
    }
    tailorPolygonOuterRingEdges.clear();
    const int size = tailorPolygonOuterRingPoints.size();
    for(int i = 0; i < size-1; i ++){
        tailorPolygonOuterRingEdges.push_back(QPair<QPoint, QPoint>(tailorPolygonOuterRingPoints[i], tailorPolygonOuterRingPoints[i+1]));
    }
    tailorPolygonOuterRingEdges.push_back(QPair<QPoint, QPoint>(tailorPolygonOuterRingPoints[size-1], tailorPolygonOuterRingPoints[0]));
    return true;
}

bool Canvas::computeIntersectedPolygons(){
    WeilerAtherton algorithm(mainPolygonOuterRingPoints, mainPolygonInnerRingsPoints, tailorPolygonOuterRingPoints, tailorPolygonInnerRingsPoints);
    algorithm.solve(clippedResult);
}
