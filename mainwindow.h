#pragma once
#include <QMainWindow>
#include <QApplication>
#include "toolbar.h"
#include "menu.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
private:
    ToolBar *tools;
};
