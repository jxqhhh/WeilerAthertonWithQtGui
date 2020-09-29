#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

QString MainWindow::purpleBtnStyleSheet("color:purple");
QString MainWindow::blackBtnStyleSheet("color:black");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widget->setMainWindow(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setAllBtnStyleSheet(){
    ui->startMainPolygonInnerRingBtn->setStyleSheet(MainWindow::blackBtnStyleSheet);
    ui->startMainPolygonOuterRingBtn->setStyleSheet(MainWindow::blackBtnStyleSheet);
    ui->startTailorPolygonInnerRingBtn->setStyleSheet(MainWindow::blackBtnStyleSheet);
    ui->startTailorPolygonOuterRingBtn->setStyleSheet(MainWindow::blackBtnStyleSheet);
    ui->finishMainPolygonInnerRingBtn->setStyleSheet(MainWindow::blackBtnStyleSheet);
    ui->finishMainPolygonOuterRingBtn->setStyleSheet(MainWindow::blackBtnStyleSheet);
    ui->finishTailorPolygonInnerRingBtn->setStyleSheet(MainWindow::blackBtnStyleSheet);
    ui->finishTailorPolygonOuterRingBtn->setStyleSheet(MainWindow::blackBtnStyleSheet);
}

void MainWindow::on_startMainPolygonInnerRingBtn_clicked(){
    if(state!=State::paintingMainPolygonOuterRingFinished
            && state!=State::paintingMainPolygonInnerRingFinished){ // notice that we can paint multiple MainPolygonInnerRings
        // ignore
        return;
    }
    ui->widget->newMainPolygonInnerRing = true;
    setAllBtnStyleSheet();
    ui->startMainPolygonInnerRingBtn->setStyleSheet(MainWindow::purpleBtnStyleSheet);
    state = State::paintingMainPolygonInnerRing;
}

void MainWindow::on_finishMainPolygonInnerRingBtn_clicked(){
    if(state!=State::paintingMainPolygonInnerRing){
        // ignore
        return;
    }
    // check if we can finish now:
    // TODO
    setAllBtnStyleSheet();
    ui->finishMainPolygonInnerRingBtn->setStyleSheet(MainWindow::purpleBtnStyleSheet);
    state = State::paintingMainPolygonInnerRingFinished;
    ui->widget->finishAMainPolygonInnerRing();
    ui->widget->update();
}

void MainWindow::on_startMainPolygonOuterRingBtn_clicked(){
    if(state!=State::notPainting){
        // ignore
        return;
    }
    setAllBtnStyleSheet();
    ui->startMainPolygonOuterRingBtn->setStyleSheet(MainWindow::purpleBtnStyleSheet);
    state = State::paintingMainPolygonOuterRing;
}

void MainWindow::on_finishMainPolygonOuterRingBtn_clicked(){
    if(state!=State::paintingMainPolygonOuterRing){
        // ignore
        return;
    }
    // check if we can finish now:
    // TODO
    setAllBtnStyleSheet();
    ui->finishMainPolygonOuterRingBtn->setStyleSheet(MainWindow::purpleBtnStyleSheet);
    state = State::paintingMainPolygonOuterRingFinished;
    ui->widget->finishMainPolygonOuterRing();
    ui->widget->update();
}

void MainWindow::on_startTailorPolygonInnerRingBtn_clicked(){
    if(state!=State::paintingTailorPolygonOuterRingFinished
            && state!=State::paintingTailorPolygonInnerRingFinished){ // notice that we can paint multiple MainPolygonInnerRings
        // ignore
        return;
    }
    // check if we can finish now:
    // TODO
    setAllBtnStyleSheet();
    ui->startTailorPolygonInnerRingBtn->setStyleSheet(MainWindow::purpleBtnStyleSheet);
    ui->widget->newTailorPolygonInnerRing = true;
    state = State::paintingTailorPolygonInnerRing;
}

void MainWindow::on_finishTailorPolygonInnerRingBtn_clicked(){
    if(state!=State::paintingTailorPolygonInnerRing){
        // ignore
        return;
    }
    setAllBtnStyleSheet();
    ui->finishTailorPolygonInnerRingBtn->setStyleSheet(MainWindow::purpleBtnStyleSheet);
    state = State::paintingTailorPolygonInnerRingFinished;
    ui->widget->finishATailorPolygonInnerRing();
    ui->widget->update();
}

void MainWindow::on_startTailorPolygonOuterRingBtn_clicked(){
    if(state!=State::paintingMainPolygonInnerRingFinished
            && state!=State::paintingMainPolygonOuterRingFinished){ // notice that we can skip painting MainPolygonInnerRings
        // ignore
        return;
    }
    setAllBtnStyleSheet();
    ui->startTailorPolygonOuterRingBtn->setStyleSheet(MainWindow::purpleBtnStyleSheet);
    state = State::paintingTailorPolygonOuterRing;
}

void MainWindow::on_finishTailorPolygonOuterRingBtn_clicked(){
    if(state!=State::paintingTailorPolygonOuterRing){
        // ignore
        return;
    }
    setAllBtnStyleSheet();
    ui->finishTailorPolygonOuterRingBtn->setStyleSheet(MainWindow::purpleBtnStyleSheet);
    state = State::paintingTailorPolygonOuterRingFinished;
    ui->widget->finishTailorPolygonOuterRing();
    ui->widget->update();
}

void MainWindow::on_clearBtn_clicked(){
    setAllBtnStyleSheet();
    state = State::notPainting;
    ui->widget->clear();
    ui->widget->update();
}

void MainWindow::on_startBtn_clicked(){
    setAllBtnStyleSheet();
    state = State::notPainting;
    ui->widget->update();
}


State MainWindow::getState() const{
    return state;
}
