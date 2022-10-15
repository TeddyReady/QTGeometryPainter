#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    //Set Background Color
    this->setStyleSheet("background-color:white;");

    //Creating ToolBar
    tools = new ToolBar(this);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->spontaneous() && CHOOSED_OBJECT != nullptr) {
        CHOOSED_OBJECT->deselect();
    }

    if (CHOOSED_OBJECT == nullptr) {
        tools->setDisabledDestroyAction(true);
    } else {
        tools->setDisabledDestroyAction(false);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)
    tools->resize(this->width(), 80);
    tools->update();
}
