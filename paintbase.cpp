#include "paintbase.h"

PaintBase::PaintBase(QWidget *parent) : QWidget(parent) {
    isRotationChanged = false;
    rotationValue = rand() % 360;
    mousePositionX = rand() % parentWidget()->width()/2;
    mousePositionY = rand() % parentWidget()->height()/2;
    this->move(mousePositionX, mousePositionY);
    rectWidth = rand() % parentWidget()->width() + 9;
    rectHeight = ( rand() % parentWidget()->height() + 4 ) % rectWidth;
    pointA = {Modifications::Nothing, rand() % ( rectHeight/3 )};
    pointB = {Modifications::Nothing, rand() % ( rectHeight/3 )};
    pointC = {Modifications::Nothing, rand() % ( rectHeight/3 )};
    pointD = {Modifications::Nothing, rand() % ( rectHeight/3 )};
    pointE = {Modifications::Nothing, ( rand() % ( rectWidth/4 ) ) % rectHeight};
    pointF = {Modifications::Nothing, ( rand() % ( rectWidth/4 ) ) % rectHeight};
    isResized = false;
    isSelected = false;
    isClosed = true;

    updatePerimeter();
    updateArea();
}

void PaintBase::setPoints(const std::vector<Modifications>& points) {
    if (points.size() < 6) {
        throw std::invalid_argument{"Необходимо задать 6 точек!"};
    }
    pointA.type = points[0];
    pointB.type = points[1];
    pointC.type = points[2];
    pointD.type = points[3];
    pointE.type = points[4];
    pointF.type = points[5];
}

void PaintBase::deselect() {
    isSelected = false;
    CHOOSED_OBJECT = nullptr;
    this->repaint();
}

void PaintBase::mousePressEvent(QMouseEvent *event) {

    if (CHOOSED_OBJECT != nullptr) {
        CHOOSED_OBJECT->deselect();
    }

    switch(event->button()) {
    case Qt::LeftButton:
        if (isPointOfFigure(event->pos())) {
            mousePositionX = event->globalX() - this->x();
            mousePositionY = event->globalY() - this->y();

            CHOOSED_OBJECT = this;
            isSelected = true;
            this->repaint();
        }
        break;
    case Qt::RightButton:
        if (isPointOfFigure(event->pos())) {
            CHOOSED_OBJECT = this;
            isSelected = true;
            this->repaint();
            infoAboutFigure();
        }
        break;
    default:
        QWidget::mousePressEvent(event);
        break;
    }

    QWidget::mousePressEvent(event);
};

void PaintBase::mouseMoveEvent(QMouseEvent *event) {
    if (isSelected) {
        if ( event->globalX() < parentWidget()->x() ||
             event->globalX() > parentWidget()->x() + parentWidget()->width() ||
             event->globalY() < parentWidget()->y() ||
             event->globalY() > parentWidget()->y() + parentWidget()->height()
           ) {
            return;
        }

        if (isPointOfFigure(event->pos())) {
            this->move(event->globalX() - mousePositionX,
                       event->globalY() - mousePositionY);
            this->repaint();
        }
    }
}

void PaintBase::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    // Ширина и высота ограничивающего прямоугольника
    int widgetWidth = rectWidth;
    int widgetHeight = rectHeight;

    // Изменение размеров виджета при повороте/изменении размеров фигуры
    if (!isResized) {
        updatePerimeter();
        updateArea();

        int newWidth = std::max( std::abs ( accumulateNewCoordinates({ 0,           0            }).first -
                                            accumulateNewCoordinates({ widgetWidth, widgetHeight }).first   ),
                                 std::abs ( accumulateNewCoordinates({ 0,           widgetHeight }).first -
                                            accumulateNewCoordinates({ widgetWidth, 0            }).first   )
                               );
        int newHeight = std::max( std::abs ( accumulateNewCoordinates({ 0,           0            }).second -
                                             accumulateNewCoordinates({ widgetWidth, widgetHeight }).second   ),
                                  std::abs ( accumulateNewCoordinates({ 0,           widgetHeight }).second -
                                             accumulateNewCoordinates({ widgetWidth, 0            }).second   )
                                );
        isResized = true;
        // Смещение виджета для устранения смещения центра фигуры
        this->move(this->x() + ( this->width() - newWidth - 4 ) / 2, this->y() + ( this->height() - newHeight - 4 ) / 2);

        // Запас в 4 пиксела для устранения погрешностей вычислений
        this->resize(newWidth+4, newHeight+4);
    }

    QPainter painter(this);

    // Смещение осей координат
    painter.translate((width()-1)/2, (height()-1)/2);

    // Поворот осей координат (по умолчению против часовой стрелки)
    if (isRotationChanged) {
        painter.rotate(+rotationValue);
    } else {
        painter.rotate(-rotationValue);
    }

    // Если фигура выделена, отрисовать синим
    if (isSelected) {
        painter.setPen(QPen(Qt::blue));
    }

    // Отрисовка прямых, соединяющих точки A,B,C,D,E,F между собой
    // Верх
    painter.drawLine(pointD.value - widgetWidth/2, - widgetHeight/2, - pointE.value/2, - widgetHeight/2);
    painter.drawLine(pointE.value/2, - widgetHeight/2, widgetWidth/2 - pointA.value, - widgetHeight/2);
    // Право
    painter.drawLine(widgetWidth/2, pointA.value - widgetHeight/2, widgetWidth/2, widgetHeight/2 - pointB.value);
    // Низ
    painter.drawLine(pointC.value - widgetWidth/2, widgetHeight/2, - pointF.value/2, widgetHeight/2);
    painter.drawLine(pointF.value/2, widgetHeight/2, widgetWidth/2 - pointB.value, widgetHeight/2);
    // Лево
    painter.drawLine(- widgetWidth/2, pointD.value - widgetHeight/2, - widgetWidth/2, widgetHeight/2 - pointC.value);

    // Универсальная отрисовка всех точек
    paintPoint(painter, pointA,  widgetWidth/2 - pointA.value, -widgetHeight/2 + pointA.value,   +1, -1);
    paintPoint(painter, pointB,  widgetWidth/2 - pointB.value,  widgetHeight/2 - pointB.value,   +1, +1);
    paintPoint(painter, pointC, -widgetWidth/2 + pointC.value,  widgetHeight/2 - pointC.value,   -1, +1);
    paintPoint(painter, pointD, -widgetWidth/2 + pointD.value, -widgetHeight/2 + pointD.value,   -1, -1);
    paintPoint(painter, pointE, 0,                             -widgetHeight/2 + pointE.value/2,  0, -1);
    paintPoint(painter, pointF, 0,                              widgetHeight/2 - pointF.value/2,  0, +1);
}

void PaintBase::infoAboutFigure() {

    // Локальные переменные для сохранения изменений внутри диалогового окна
    int newRectWidth          = rectWidth;
    int newRectHeight         = rectHeight;
    int newRotationValue      = rotationValue;
    int newPointA             = pointA.value;
    int newPointB             = pointB.value;
    int newPointC             = pointC.value;
    int newPointD             = pointD.value;
    int newPointE             = pointE.value;
    int newPointF             = pointF.value;
    bool newRotationChanged   = isRotationChanged;
    isClosed = false;

    // Бесконечный цикл, завершающийся только при вводе корректных значений
    // характеристик фигуры.
    // Минимальные значения ширины и высоты описанного прямоугольника заданы исходя
    // из уточнения к заданию: h >= 4; w >= 9; x,r >= 1; p,q >= 2.
    // Если введенные значения некорректны, выводится соответствующее сообщение,
    // после чего в повторно открытом диалоговом окне предлагается изменить данные.
    // При повторном открытии сохраняются все заданные ранее изменения.
    // Кнопка отмены изменений отменяет все заданные ранее изменения и заполняет
    // поля для ввода данных первоначальными (и текущими) значениями характеристик
    // фигуры, после чего предлагается снова изменить их или сохранить без изменений.
    while(1) {
        QBoxLayout* boxLayout =
                    new QBoxLayout(QBoxLayout::TopToBottom);

        // Создание SpinBox для удобного заполнения данных пользователем.
        // Минимальное значение установлено согласно описанию выше или минимальное
        // значение переменной типа int;
        // максмальное значение равно максимальному значению переменной типа int.
        // Для выбора направления поворота созданы 2 RadioButton - по и против
        // часовой стрелки.
        QSpinBox *spinWidth  = new QSpinBox();
        spinWidth->setRange(9, INT_MAX);
        spinWidth->setValue(newRectWidth);
        spinWidth->setSingleStep(5);

        QSpinBox *spinHeight = new QSpinBox();
        spinHeight->setRange(4, INT_MAX);
        spinHeight->setValue(newRectHeight);
        spinHeight->setSingleStep(5);

        QSpinBox *spinRotate = new QSpinBox();
        spinRotate->setRange(INT_MIN, INT_MAX);
        spinRotate->setValue(newRotationValue);
        spinRotate->setSingleStep(5);

        QRadioButton *rotateR = new QRadioButton("По часовой стрелке");
        QRadioButton *rotateL = new QRadioButton("Против часовой стрелки");
        if (newRotationChanged) {
            rotateR->setChecked(true);
        } else {
            rotateL->setChecked(true);
        }

        QSpinBox *spinPointA = new QSpinBox();
        spinPointA->setRange(1, INT_MAX);
        spinPointA->setValue(newPointA);
        spinPointA->setSingleStep(5);

        QSpinBox *spinPointB = new QSpinBox();
        spinPointB->setRange(1, INT_MAX);
        spinPointB->setValue(newPointB);
        spinPointB->setSingleStep(5);

        QSpinBox *spinPointC = new QSpinBox();
        spinPointC->setRange(1, INT_MAX);
        spinPointC->setValue(newPointC);
        spinPointC->setSingleStep(5);

        QSpinBox *spinPointD = new QSpinBox();
        spinPointD->setRange(1, INT_MAX);
        spinPointD->setValue(newPointD);
        spinPointD->setSingleStep(5);

        QSpinBox *spinPointE = new QSpinBox();
        spinPointE->setRange(2, INT_MAX);
        spinPointE->setValue(newPointE);
        spinPointE->setSingleStep(5);

        QSpinBox *spinPointF = new QSpinBox();
        spinPointF->setRange(2, INT_MAX);
        spinPointF->setValue(newPointF);
        spinPointF->setSingleStep(5);

        // Добавление вышеобъявленных и инициализированных виджетов
        // в QBoxLayout, предварительно добавляя описания поля ввода
        // в виде QLabel с текстом-информацией.
        boxLayout->addWidget(new QLabel("Информация о фигуре:"));
        boxLayout->addWidget(new QLabel("Периметр фигуры: " + QString::number(perimeter)));
        boxLayout->addWidget(new QLabel("Площадь фигуры: " + QString::number(area)));
        boxLayout->addWidget(new QLabel("Ширина:"));
        boxLayout->addWidget(spinWidth);
        boxLayout->addWidget(new QLabel("Высота:"));
        boxLayout->addWidget(spinHeight);
        boxLayout->addWidget(new QLabel("Угол поворота:"));
        boxLayout->addWidget(spinRotate);
        boxLayout->addWidget(rotateR);
        boxLayout->addWidget(rotateL);
        if (pointA.type != Modifications::Nothing) {
            boxLayout->addWidget(new QLabel("Параметры точки А:"));
            boxLayout->addWidget(spinPointA);
        }
        if (pointB.type != Modifications::Nothing) {
            boxLayout->addWidget(new QLabel("Параметры точки B:"));
            boxLayout->addWidget(spinPointB);
        }
        if (pointC.type != Modifications::Nothing) {
            boxLayout->addWidget(new QLabel("Параметры точки C:"));
            boxLayout->addWidget(spinPointC);
        }
        if (pointD.type != Modifications::Nothing) {
            boxLayout->addWidget(new QLabel("Параметры точки D:"));
            boxLayout->addWidget(spinPointD);
        }
        if (pointE.type != Modifications::Nothing) {
            boxLayout->addWidget(new QLabel("Параметры точки E:"));
            boxLayout->addWidget(spinPointE);
        }
        if (pointF.type != Modifications::Nothing) {
            boxLayout->addWidget(new QLabel("Параметры точки F:"));
            boxLayout->addWidget(spinPointF);
        }

        // Создание диалогового окна для просмотра и изменения характеристик
        // фигуры с двумя кнопками - "Сохранить изменения" и "Отменить изменения".
        createDialog(boxLayout, "Сохранить изменения", "Отменить изменения");

        // Ожидание закрытия диалогового окна.

        // Проверка на множественный вызов метода createDialog для предотвращения
        // множественного вывода сообщений о сохраненных/неверных данных.
        if (isClosed) {
            return;
        }

        // Проверка введенных данных на корректность.

        // Если данные некорректны, они сохраняются в локальные переменные, после
        // чего вызывается диалоговое окно, информирующее пользователя о некорректно
        // введенных данных, после закрытия которого цикл повторяется и модальное
        // диалоговое окно для просмотра и изменения характеристик фигуры вызывается
        // снова с сохраненными ранее введенными данными.
        if (spinWidth->value() < 9 || spinHeight->value() < 4 || spinHeight->value() >= spinWidth->value() ||
                        spinPointA->value()*3 >= spinHeight->value() ||
                        spinPointB->value()*3 >= spinHeight->value() ||
                        spinPointC->value()*3 >= spinHeight->value() ||
                        spinPointD->value()*3 >= spinHeight->value() ||
                        spinPointE->value()*4 >= spinWidth->value()  ||
                        spinPointF->value()*4 >= spinWidth->value()  ||
                        spinPointE->value() >= spinHeight->value()   ||
                        spinPointF->value() >= spinHeight->value()
                        )
        {

            newRectWidth     = spinWidth->value();
            newRectHeight    = spinHeight->value();
            newRotationValue = spinRotate->value();
            newPointA        = spinPointA->value();
            newPointB        = spinPointB->value();
            newPointC        = spinPointC->value();
            newPointD        = spinPointD->value();
            newPointE        = spinPointE->value();
            newPointF        = spinPointF->value();
            if (rotateR->isChecked()) {
                newRotationChanged = true;
            } else {
                newRotationChanged = false;
            }

            QBoxLayout* errorLayout =
                        new QBoxLayout(QBoxLayout::TopToBottom);

            errorLayout->addWidget(new QLabel("Введенные данные некорректны"));
            createDialog(errorLayout, "Изменить");

        // Если данные корректны, они сохраняются в соответствующие поля класса,
        // после чего вызывается отрисовка виджета с измененными характеристиками,
        // а пользователю выводится окно с сообщением об успешном сохранении данных.
        } else {
            rectWidth     = spinWidth->value();
            rectHeight    = spinHeight->value();
            rotationValue = spinRotate->value();
            pointA.value  = spinPointA->value();
            pointB.value  = spinPointB->value();
            pointC.value  = spinPointC->value();
            pointD.value  = spinPointD->value();
            pointE.value  = spinPointE->value();
            pointF.value  = spinPointF->value();
            if (rotateR->isChecked()) {
                isRotationChanged = true;
            } else {
                isRotationChanged = false;
            }

            isResized = false;
            this->repaint();

            QBoxLayout* saveLayout =
                        new QBoxLayout(QBoxLayout::TopToBottom);

            saveLayout->addWidget(new QLabel("Введенные данные сохранены"));
            createDialog(saveLayout, "Закрыть");

            isClosed = true;
            return;
        }
    }
}

void PaintBase::createDialog(QBoxLayout *boxLayout, const QString& message) {
    QDialog *modalDialog = new QDialog(this);
    // Задание характеристики модальности
    modalDialog->setModal(true);

    // Создание кнопки с полученным текстом message, добавление ее
    // в конец (низ) окна (QBoxLayout) и привязка сигнала нажатия
    // на кнопку к слоту закрытия окна.
    QPushButton *button = new QPushButton(message);
    boxLayout->addWidget(button);
    connect(button, SIGNAL(clicked()), modalDialog, SLOT(accept()));

    modalDialog->setLayout(boxLayout);
    modalDialog->exec();

    delete modalDialog;
}

void PaintBase::createDialog(QBoxLayout *boxLayout, const QString& message1, const QString& message2) {
    QDialog *modalDialog = new QDialog(this);

    // Задание характеристики модальности
    modalDialog->setModal(true);

    // Создание кнопки с полученным текстом message1, добавление ее
    // в конец (низ) окна (QBoxLayout) и привязка сигнала нажатия
    // на кнопку к слоту закрытия окна.
    QPushButton *saveButton = new QPushButton(message1);
    boxLayout->addWidget(saveButton);
    connect(saveButton, SIGNAL(clicked()), modalDialog, SLOT(accept()));

    // Создание кнопки с полученным текстом message2, добавление ее
    // в конец (низ) окна (QBoxLayout) и привязка сигнала нажатия
    // на кнопку к слоту закрытия окна и слоту infoAboutFigure для
    // повторного открытия модального диалогового окна с информацией
    // о фигуре и возможностью ее изменения.
    QPushButton *cancelButton = new QPushButton(message2);
    boxLayout->addWidget(cancelButton);
    connect(cancelButton, SIGNAL(clicked()), modalDialog, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(infoAboutFigure()));

    modalDialog->setLayout(boxLayout);
    modalDialog->exec();

    delete modalDialog;
}

std::pair<int, int> PaintBase::accumulateNewCoordinates (const std::pair<int, int>& oldCoordinates) const {
    // Для перевода градусов поля rotationValue в радианы
    double degreesToRadians = 3.14159 / 180.0;
    // Перевод координат из исходной системы координат в
    // повернутую при помощи матрицы поворота в зависимости
    // от направления поворота.
    if (isRotationChanged) {
        return { ( std::cos(rotationValue * degreesToRadians) * oldCoordinates.first + std::sin(rotationValue * degreesToRadians) * oldCoordinates.second ),
                 (-std::sin(rotationValue * degreesToRadians) * oldCoordinates.first + std::cos(rotationValue * degreesToRadians) * oldCoordinates.second ) };
    } else {
        return { ( std::cos(rotationValue * degreesToRadians) * oldCoordinates.first - std::sin(rotationValue * degreesToRadians) * oldCoordinates.second ),
                 ( std::sin(rotationValue * degreesToRadians) * oldCoordinates.first + std::cos(rotationValue * degreesToRadians) * oldCoordinates.second ) };
    }
}

bool PaintBase::isPointOfFigure(QPoint point) const {
    int widgetWidth = rectWidth;
    int widgetHeight = rectHeight;
    // Смещение координат
    point.rx() -= (width()-1)/2;
    point.ry() -= (height()-1)/2;
    // Изменение координат точки из исходной системы в повернутую
    auto newPoint = accumulateNewCoordinates({point.x(), point.y()});
    point.rx() = newPoint.first;
    point.ry() = newPoint.second;

    // Точка не находится в прямоугольнике фигуры
    if (std::abs(point.x()) > widgetWidth/2 || std::abs(point.y()) > widgetHeight/2) {
        return false;
    }

    // Точка попадает в ...
    // Квадрат точки A
    if (point.x() >= (widgetWidth/2 - pointA.value) && point.y() <= (-widgetHeight/2 + pointA.value)) {
        return isPointOfRectOfPoint(point, pointA, +1, -1);
    }
    // Квадрат точки B
    if (point.x() >= (widgetWidth/2 - pointB.value) && point.y() >= (widgetHeight/2 - pointB.value)) {
        return isPointOfRectOfPoint(point, pointB, +1, +1);;
    }
    // Квадрат точки C
    if (point.x() <= (-widgetWidth/2 + pointC.value) && point.y() >= (widgetHeight/2 - pointC.value)) {
        return isPointOfRectOfPoint(point, pointC, -1, +1);;
    }
    // Квадрат точки D
    if (point.x() <= (-widgetWidth/2 + pointD.value) && point.y() <= (-widgetHeight/2 + pointD.value)) {
        return isPointOfRectOfPoint(point, pointD, -1, -1);;
    }
    // Прямоугольник точки E
    if (std::abs(point.x()) <= pointE.value/2 && point.y() <= (-widgetHeight/2 + pointE.value/2)) {
        return isPointOfRectOfPoint(point, pointE, 0, -1);;
    }
    // Прямоугольник точки F
    if (std::abs(point.x()) <= pointF.value/2 && point.y() >= (widgetHeight/2 - pointF.value/2)) {
        return isPointOfRectOfPoint(point, pointF, 0, 1);;
    }
    return true;
}

bool PaintBase::isPointOfRectOfPoint(QPoint point, const Point& pointOfFigure, int modifierW, int modifierH) const {
    // Ширина и высота ограничивающего прямоугольника
    int widgetWidth = rectWidth;
    int widgetHeight = rectHeight;

    // Вычисления в зависимости от типа точки и установленных модификаторов
    switch(pointOfFigure.type) {
    case Modifications::Nothing:
        return true;
        break;
    case Modifications::Square:
        return false;
        break;
    case Modifications::Tria:
        if ( std::abs( point.x() + ( modifierH * modifierW ) * point.y() ) <
             std::abs( modifierW * widgetWidth/2 + ( modifierH * modifierW ) * ( modifierH * widgetHeight/2 - modifierH * pointOfFigure.value ) ) ) {
            return true;
        } else {
            return false;
        }
        break;
    case Modifications::Cone:
        point.rx() -= modifierW * widgetWidth/2;
        point.ry() -= modifierH * widgetHeight/2;
        if ( ( point.x() * point.x() + point.y() * point.y() ) >= ( pointOfFigure.value * pointOfFigure.value ) ) {
            return true;
        } else {
            return false;
        }
        break;
    case Modifications::Wave:
        point.rx() -= modifierW * ( widgetWidth/2 - pointOfFigure.value );
        point.ry() -= modifierH * ( widgetHeight/2 - pointOfFigure.value );
        if ( ( point.x() * point.x() + point.y() * point.y() ) <= ( pointOfFigure.value * pointOfFigure.value ) ) {
            return true;
        } else {
            return false;
        }
        break;
    case Modifications::Rect:
        return false;
        break;
    case Modifications::Sphere:
        point.ry() -= modifierH * widgetHeight/2;
        if ( ( point.x() * point.x() + point.y() * point.y() ) >= ( ( pointOfFigure.value/2 ) * ( pointOfFigure.value/2 ) ) ) {
            return true;
        } else {
            return false;
        }
        break;
    }
    return false;
}

void PaintBase::paintPoint(QPainter& painter, const Point& point, const int posW, const int posH, int modifierW, int modifierH) const {
    // Отрисовка в зависимости от типа точки и установленных модификаторов
    switch(point.type) {
    case Modifications::Nothing:
        // Если точка A/B/C/D
        if (modifierW != 0) {
            painter.drawLine(posW + modifierW * point.value, posH + modifierH * point.value, posW + modifierW * point.value, posH);
            painter.drawLine(posW + modifierW * point.value, posH + modifierH * point.value, posW,                           posH + modifierH * point.value);
        // Если точка E/F
        } else {
            painter.drawLine(posW + point.value/2, posH + modifierH * point.value/2, posW - point.value/2, posH + modifierH * point.value/2);
        }
        break;
    case Modifications::Square:
        painter.drawLine(posW, posH, posW + modifierW * point.value, posH);
        painter.drawLine(posW, posH, posW,                           posH + modifierH * point.value);
        break;
    case Modifications::Tria:
        painter.drawLine(posW + modifierW * point.value, posH, posW, posH + modifierH * point.value);
        break;
    case Modifications::Cone:
        painter.drawArc(posW, posH,
                        modifierW * point.value * 2, modifierH * point.value * 2,
                        modifierH * 90*16, modifierW * modifierH * 90*16);
        break;
    case Modifications::Wave:
        painter.drawArc(posW + modifierW * point.value, posH + modifierH * point.value,
                        - modifierW * point.value * 2, - modifierH * point.value * 2,
                        - modifierH * 90*16, modifierW * modifierH * 90*16);
        break;
    case Modifications::Rect:
        // Горизонталь
        painter.drawLine(posW, posH, posW + point.value/2, posH);
        painter.drawLine(posW, posH, posW - point.value/2, posH);
        // Вертикаль
        painter.drawLine(posW + point.value/2, posH, posW + point.value/2, posH + modifierH * point.value/2);
        painter.drawLine(posW - point.value/2, posH, posW - point.value/2, posH + modifierH * point.value/2);
        break;
    case Modifications::Sphere:
        painter.drawArc(posW - point.value/2, posH,
                        point.value, modifierH * point.value,
                        0*16, modifierH * 180*16);
        break;
    default: break;
    }
}

void PaintBase::updatePerimeter() {
    perimeter = 0;

    // Периметр описанного прямоугольника
    perimeter += rectWidth * 2 + rectHeight * 2;

    // Убрать внешний периметр квадрата каждой точки
    perimeter -= pointA.value * 2 +
                 pointB.value * 2 +
                 pointC.value * 2 +
                 pointD.value * 2 +
                 pointE.value     +
                 pointF.value;

    // Добавить длины линий каждой точки
    perimeter += accumulatePerimeterOfPoint(pointA) +
                 accumulatePerimeterOfPoint(pointB) +
                 accumulatePerimeterOfPoint(pointC) +
                 accumulatePerimeterOfPoint(pointD) +
                 accumulatePerimeterOfPoint(pointE, true) +
                 accumulatePerimeterOfPoint(pointF, true);
}

double PaintBase::accumulatePerimeterOfPoint(const Point& pointOfFigure, bool isPointEOrPointF) const {
    // Вычисления в зависимости от типа точки и установленного модификатора
    switch(pointOfFigure.type) {
    case Modifications::Nothing :
        if (!isPointEOrPointF) {
            return pointOfFigure.value * 2;
        } else {
            return pointOfFigure.value;
        }
        break;
    case Modifications::Square :
        return pointOfFigure.value * 2;
        break;
    case Modifications::Tria :
        return pointOfFigure.value * std::sqrt(2);
        break;
    case Modifications::Cone :
        return pointOfFigure.value * 3.14159 / 2;
        break;
    case Modifications::Wave :
        return pointOfFigure.value * 3.14159 / 2;
        break;
    case Modifications::Rect :
        return pointOfFigure.value * 2;
        break;
    case Modifications::Sphere :
        return pointOfFigure.value * 3.14159 / 2;
        break;
    }
    return 0;
}

void PaintBase::updateArea() {
    area = 0;

    // Площадь описанного прямоугольника
    area += rectWidth * rectHeight;

    // Убрать площадь квадрата каждой точки
    area -= pointA.value * pointA.value   +
            pointB.value * pointB.value   +
            pointC.value * pointC.value   +
            pointD.value * pointD.value   +
            pointE.value * pointE.value/2 +
            pointF.value * pointF.value/2;

    // Добавить площади, ограничиваемые линиями каждой точки
    area += accumulateAreaOfPoint(pointA) +
            accumulateAreaOfPoint(pointB) +
            accumulateAreaOfPoint(pointC) +
            accumulateAreaOfPoint(pointD) +
            accumulateAreaOfPoint(pointE, true) +
            accumulateAreaOfPoint(pointF, true);
}

double PaintBase::accumulateAreaOfPoint(const Point& pointOfFigure, bool isPointEOrPointF) const {
    // Вычисления в зависимости от типа точки и установленного модификатора
    switch(pointOfFigure.type) {
    case Modifications::Nothing :
        if (!isPointEOrPointF) {
            return pointOfFigure.value * pointOfFigure.value;
        } else {
            return pointOfFigure.value * pointOfFigure.value/2;
        }
        break;
    case Modifications::Square :
        return 0;
        break;
    case Modifications::Tria :
        return pointOfFigure.value * pointOfFigure.value / 2;
        break;
    case Modifications::Cone :
        // r*r - ( pi * r^2 / 4 )
        return pointOfFigure.value * pointOfFigure.value * ( 1 - 3.14159 / 4 );
        break;
    case Modifications::Wave :
        return pointOfFigure.value * pointOfFigure.value * 3.14159 / 4;
        break;
    case Modifications::Rect :
        return 0;
        break;
    case Modifications::Sphere :
        // d*d/2 - ( pi * d^2 / 4 / 2 )
        return pointOfFigure.value * pointOfFigure.value * ( 1/2 - 3.14159 / 8 );
        break;
    }
    return 0;
}
