#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QMouseEvent>
#include <iostream>
#include <QPainter>
#include "mainwindow.h"
#include "weileratherton.h"
using std::cout;

namespace Ui {
class Canvas;
}

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr);
    ~Canvas();
    friend class MainWindow;
private slots:
    void mouseReleaseEvent(QMouseEvent* e);
    void paintEvent(QPaintEvent* e);
private:
    Ui::Canvas *ui;

    QVector<QPoint> mainPolygonOuterRingPoints;
    QVector<QPair<QPoint, QPoint>> mainPolygonOuterRingEdges; // for painting only

    QVector<QVector<QPoint>> mainPolygonInnerRingsPoints;
    QVector<QVector<QPair<QPoint, QPoint>>> mainPolygonInnerRingsEdges; // for painting only

    QVector<QPoint> tailorPolygonOuterRingPoints;
    QVector<QPair<QPoint, QPoint>> tailorPolygonOuterRingEdges; // for painting only

    QVector<QVector<QPoint>> tailorPolygonInnerRingsPoints;
    QVector<QVector<QPair<QPoint, QPoint>>> tailorPolygonInnerRingsEdges; // for painting only

    QVector<QVector<QPoint>> clippingResult;

    void setMainWindow(MainWindow* m);
    MainWindow* mainwindow;
    QPainter* painterForMainPolygon;
    QPainter* painterForTailorPolygon;
    const static int outerRingPtsWdith = 5;
    const static int innerRingPtsWdith = 3;
    const static QColor mainPolygonPtsColor, tailorPolygonPtsColor, clippingResultPtsColor;
    void clear();
    bool finishAMainPolygonInnerRing();
    bool finishATailorPolygonInnerRing();
    bool finishMainPolygonOuterRing();
    bool finishTailorPolygonOuterRing();
    bool newMainPolygonInnerRing = false;
    bool newTailorPolygonInnerRing = false;
    bool computeIntersectedPolygons();
};

#endif // CANVAS_H
