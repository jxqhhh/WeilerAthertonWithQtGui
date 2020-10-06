
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class Canvas;

namespace Ui {
class MainWindow;
}

enum State{
    notPainting,
    paintingMainPolygonOuterRing,
    paintingMainPolygonOuterRingFinished, // but do not begin painting MainPolygonInnerRing
    paintingMainPolygonInnerRing,
    paintingMainPolygonInnerRingFinished, // but do not begin painting TailorPolygonOuterRing
    paintingTailorPolygonOuterRing,
    paintingTailorPolygonOuterRingFinished, // but do not begin painting TailorPolygonInnerRing
    paintingTailorPolygonInnerRing,
    paintingTailorPolygonInnerRingFinished,
    paintingFinished
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    State getState() const;
    friend class Canvas;
    bool isPaintingMainPolygonOuterRingFinished() const;
    bool isPaintingTailorPolygonOuterRingFinished() const;
private slots:
    void on_startMainPolygonOuterRingBtn_clicked();

    void on_finishMainPolygonOuterRingBtn_clicked();

    void on_startMainPolygonInnerRingBtn_clicked();

    void on_finishMainPolygonInnerRingBtn_clicked();

    void on_startTailorPolygonOuterRingBtn_clicked();

    void on_finishTailorPolygonOuterRingBtn_clicked();

    void on_startTailorPolygonInnerRingBtn_clicked();

    void on_finishTailorPolygonInnerRingBtn_clicked();

    void on_clearBtn_clicked();

    void on_startBtn_clicked();

private:
    Ui::MainWindow *ui;
    static QString purpleBtnStyleSheet;
    static QString blackBtnStyleSheet;
    State state = State::notPainting;
    void setAllBtnStyleSheet();
    static QString stateToMsg(State s);
};

#endif // MAINWINDOW_H
