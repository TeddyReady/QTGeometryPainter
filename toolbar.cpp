#include "toolbar.h"

ToolBar::ToolBar(QMainWindow *parent) : QToolBar(parent) {
    //Инициализируем Actions:
    appendAction = new QAction(QIcon(":/new/prefix1/icon/plus.png"),"Add Figure");
    type1Action = new QAction(QIcon(":/new/prefix1/icon/type1.png"),"Select Modify 1 of Figure");
    type2Action = new QAction(QIcon(":/new/prefix1/icon/type2.png"),"Select Modify 2 of Figure");
    destroyAction = new QAction(QIcon(":/new/prefix1/icon/trash.png"),"Destroy Object");

    //Устанавлваем типы Actions:
    appendAction->setDisabled(true);
    type1Action->setCheckable(true);
    type2Action->setCheckable(true);
    destroyAction->setDisabled(true);

    //Связывание соответсвующих сигналов Action со слотами:
    connect(appendAction, SIGNAL(triggered(bool)), this, SLOT(addFigureTask()));
    connect(type1Action, SIGNAL(triggered(bool)), this, SLOT(chooseFigureType1Task()));
    connect(type2Action, SIGNAL(triggered(bool)), this, SLOT(chooseFigureType2Task()));
    connect(destroyAction, SIGNAL(triggered(bool)), this, SLOT(destroyFigureTask()));

    //Запрет перемещения:
    this->setMovable(false);

    //Добавление Action'ов на панель инструментов:
    this->addAction(appendAction);
    this->addSeparator();
    this->addAction(type1Action);
    this->addAction(type2Action);
    this->addSeparator();
    this->addAction(destroyAction);

    //Демонстрируем тулбар:
    this->show();
}
//Слот добавления фигуры
void ToolBar::addFigureTask() {
    appendAction->setDisabled(true);
    if (type1Action->isChecked()) {
        type1Action->setChecked(false);
        Type1* newFigure = new Type1(parentWidget());
        newFigure->lower();
        newFigure->show();
        return;
    }
    if (type2Action->isChecked()) {
        type2Action->setChecked(false);
        Type2* newFigure = new Type2(parentWidget());
        newFigure->lower();
        newFigure->show();
        return;
    }
}
//Слот выбора фигуры типа 1
void ToolBar::chooseFigureType1Task() {
    type1Action->setChecked(true);
    type2Action->setChecked(false);
    appendAction->setEnabled(true);

}
//Слот выбора фигуры типа 2
void ToolBar::chooseFigureType2Task() {
    type2Action->setChecked(true);
    type1Action->setChecked(false);
    appendAction->setEnabled(true);
}
//Слот для действия "Уместить"
void ToolBar::fitFiguresTask(){}
//Слот удаления фигуры
void ToolBar::destroyFigureTask() {
    delete CHOOSED_OBJECT;
    CHOOSED_OBJECT = nullptr;
    destroyAction->setDisabled(true);
}
