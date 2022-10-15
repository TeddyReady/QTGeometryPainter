#pragma once
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include "geometry.h"

class ToolBar : public QToolBar {
    Q_OBJECT
private:
    QAction *appendAction;
    QAction *type1Action;
    QAction *type2Action;
    QAction *fitAction;
    QAction *destroyAction;
public:
    explicit ToolBar(QMainWindow *parent = nullptr);

    void setDisabledDestroyAction (bool isDisabled) {
        destroyAction->setDisabled(isDisabled);
    }
private slots:
    void addFigureTask();
    void chooseFigureType1Task();
    void chooseFigureType2Task();
    void fitFiguresTask();
    void destroyFigureTask();
};
