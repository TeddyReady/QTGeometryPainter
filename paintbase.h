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

class PaintBase;
extern PaintBase* CHOOSED_OBJECT;

enum class Modifications { Nothing, Square, Tria, Cone, Wave, Rect, Sphere };

struct Point {
    Modifications type;
    int value;
};

class PaintBase : public QWidget {
    Q_OBJECT
public:
    explicit PaintBase(QWidget *parent = nullptr);

    // Задание типов точек Modifications для классов-наследников
    void setPoints(const std::vector<Modifications>& points);

    // Снятие выделения с данной фигуры
    void deselect();

protected:
    // Обработка события нажатия мыши на виджет (ЛКМ или ПКМ)
    void mousePressEvent(QMouseEvent *event);

    // Обработка события перемещения мыши внутри виджета (ЛКМ)
    // и перемещения фигуры (виджета) вслед за ней
    void mouseMoveEvent(QMouseEvent *event);

    // Обработка события отрисовки виджета (универсальная для всех
    // наследников класса и всех видов фигур любых размеров)
    void paintEvent(QPaintEvent *event);

private slots:
    // Вывод информации о фигуре, инициализация создания модального
    // диалогового окна для ее отображения и изменения, обработка
    // и валидация полученных изменений, их сохранение.
    void infoAboutFigure();

private:
    // Создание модального диалогового окна с одной кнопкой с текстом message
    void createDialog(QBoxLayout *boxLayout, const QString& message);

    // Создание модального диалогового окна с двумя кнопками с текстами message1 и message2
    void createDialog(QBoxLayout *boxLayout, const QString& message1, const QString& message2);

    // Перевод координат точки из исходной системы координат в повернутую на rotationValue
    std::pair<int, int> accumulateNewCoordinates (const std::pair<int, int>& oldCoordinates) const;

    // Универсальная для всех фигур проверка, лежит ли точка с указанными координатами внутри фигуры
    bool isPointOfFigure(QPoint point) const;

    // Универсальная для всех типов точек проверка, лежит ли точка с указанными координатам внутри
    // прямоугольника, ограничивающего данную точку.
    // Модификаторы устанавливаются для определения положения точки в ограничивающем прямоугольнике:
    // D: -1,-1 ... E: 0,-1 ... А: +1,-1
    // C: -1,+1 ... F: 0,+1 ... B: +1,+1
    bool isPointOfRectOfPoint(QPoint point, const Point& pointOfFigure, int modifierW, int modifierH) const;

    // Универсальная для всех типов точек отрисовка линии данной точки
    // Модификаторы устанавливаются для определения положения точки в ограничивающем прямоугольнике:
    // D: -1,-1 ... E: 0,-1 ... А: +1,-1
    // C: -1,+1 ... F: 0,+1 ... B: +1,+1
    void paintPoint(QPainter& painter, const Point& point, const int posW, const int posH, int modifierW, int modifierH) const;

    // Обновление периметра фигуры
    void updatePerimeter();

    // Универсальный для всех точек подсчет длины линии данной точки.
    // Модификатор устанавливается значением true для точек E и F.
    double accumulatePerimeterOfPoint(const Point& pointOfFigure, bool isPointEOrPointF = false) const;

    // Обновление площади фигуры
    void updateArea();

    // Универсальный для всех точек подсчет площади, ограничиваемой линией данной точки.
    // Модификатор устанавливается значением true для точек E и F.
    double accumulateAreaOfPoint(const Point& pointOfFigure, bool isPointEOrPointF = false) const;

private:
    bool isRotationChanged;
    int rotationValue;
    int mousePositionX;
    int mousePositionY;

    Point pointA;
    Point pointB;
    Point pointC;
    Point pointD;
    Point pointE;
    Point pointF;

    int rectWidth;
    int rectHeight;
    double perimeter;
    double area;

    bool isSelected;
    bool isResized;
    bool isClosed;
};
