#pragma once
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QDialog>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QRadioButton>
#include <QBoxLayout>
#include <QMainWindow>
#include <cmath>
#include <stdexcept>
#include <random>

//К выбранной фигуре должен доступ всей программе:
class PaintBase;
extern PaintBase* CHOOSED_OBJECT;

//Типы модификаций фигуры:
enum class Modifications { Nothing, Square, Tria, Cone, Wave, Rect, Sphere };

//Вспомагательная структура значения Модификации:
struct Sector {
    Modifications type;
    int value;
};
//Основной базовый класс фигуры:
class PaintBase : public QWidget {
    Q_OBJECT
private:
    //Значения секторов:
    Sector sectionA;
    Sector sectionB;
    Sector sectionC;
    Sector sectionD;
    Sector sectionE;
    Sector sectionF;

    //Данные фигуры:
    int figureWidth;
    int figureHeight;
    int rotValue;
    double figurePerimeter;
    double figureArea;

    //Позиция курсора:
    int mouseX;
    int mouseY;

    //Вспомогательные переменные состояний:
    bool isSelected;
    bool isResized;
    bool isClosed;
    bool isRotationChanged;
public:
    explicit PaintBase(QWidget *parent = nullptr);

    //Установка модификация по секторам:
    void setUpPoints(const std::vector<Modifications>& points);

    //Снятие выделение:
    void removeSelect();
protected:
    //Различные event's:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private slots:
    //Диалоговое окно с инфо о фигуре:
    void figureInformation();

private:
    //Диалоговое окно с одним текстом:
    void createDialog(QBoxLayout *boxLayout, const QString& message);

    //Перегрузка предыдущего метода с двумя текстами:
    void createDialog(QBoxLayout *boxLayout, const QString& message1, const QString& message2);

    //Поворот координат на rotValue:
    std::pair<int, int> accumulateNewCoordinates (const std::pair<int, int>& oldCoordinates) const;

    //Проверка нахождение точки в фигуре:
    bool isPointOfFigure(QPoint point) const;

    //Более сложная проверка на нахождение точки в описывающем прямоугольнике:
    bool isPointOfRectOfPoint(QPoint point, const Sector& pointOfFigure, int modifierW, int modifierH) const;

    //Отрисовка фигуры:
    void paintPoint(QPainter& painter, const Sector& point, const int posW, const int posH, int modifierW, int modifierH) const;

    //Обновление периметра и площади фигуры:
    void updatePerimeter();
    void updateArea();

    //Подсчет длины линии точки
    double findPointPerimeter(const Sector& pointOfFigure, bool isPointOfFigure = false) const;

    //Подсчет площади точки ограничиваемой линией
    double findPointArea(const Sector& pointOfFigure, bool isPointOfFigure = false) const;
};
