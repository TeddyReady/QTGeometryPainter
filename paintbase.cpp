#include "paintbase.h"

PaintBase::PaintBase(QWidget *parent) : QWidget(parent) {
    isRotationChanged = false;
    rotValue = rand() % 360;
    mouseX = rand() % parentWidget()->width()/2;
    mouseY = rand() % parentWidget()->height()/2;
    this->move(mouseX, mouseY);
    figureWidth = rand() % parentWidget()->width() + 9;
    figureHeight = ( rand() % parentWidget()->height() + 4 ) % figureWidth;
    sectionA = {Modifications::Nothing, rand() % ( figureHeight/3 )};
    sectionB = {Modifications::Nothing, rand() % ( figureHeight/3 )};
    sectionC = {Modifications::Nothing, rand() % ( figureHeight/3 )};
    sectionD = {Modifications::Nothing, rand() % ( figureHeight/3 )};
    sectionE = {Modifications::Nothing, ( rand() % ( figureWidth/4 ) ) % figureHeight};
    sectionF = {Modifications::Nothing, ( rand() % ( figureWidth/4 ) ) % figureHeight};
    isResized = false;
    isSelected = false;
    isClosed = true;

    updatePerimeter();
    updateArea();
}

void PaintBase::setUpPoints(const std::vector<Modifications>& points) {
    sectionA.type = points[0];
    sectionB.type = points[1];
    sectionC.type = points[2];
    sectionD.type = points[3];
    sectionE.type = points[4];
    sectionF.type = points[5];
}

void PaintBase::removeSelect() {
    isSelected = false;
    CHOOSED_OBJECT = nullptr;
    this->update();
}

void PaintBase::mousePressEvent(QMouseEvent *event) {

    if (CHOOSED_OBJECT != nullptr) {
        CHOOSED_OBJECT->removeSelect();
    }

    switch(event->button()) {
    case Qt::LeftButton:
        if (isPointOfFigure(event->pos())) {
            mouseX = event->globalX() - this->x();
            mouseY = event->globalY() - this->y();

            CHOOSED_OBJECT = this;
            isSelected = true;
            this->update();
        }
        break;
    case Qt::RightButton:
        if (isPointOfFigure(event->pos())) {
            CHOOSED_OBJECT = this;
            isSelected = true;
            this->update();
            figureInformation();
        }
        break;
    default:
        QWidget::mousePressEvent(event);
        break;
    }

    QWidget::mousePressEvent(event);
}

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
            this->move(event->globalX() - mouseX,
                       event->globalY() - mouseY);
            this->update();
        }
    }
}

void PaintBase::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    // Ширина и высота ограничивающего прямоугольника
    int widgetWidth = figureWidth;
    int widgetHeight = figureHeight;

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
        painter.rotate(+rotValue);
    } else {
        painter.rotate(-rotValue);
    }

    // Если фигура выделена, отрисовать синим
    if (isSelected) {
        painter.setPen(QPen(Qt::blue));
    }

    // Отрисовка прямых, соединяющих точки A,B,C,D,E,F между собой
    // Верх
    painter.drawLine(sectionD.value - widgetWidth/2, - widgetHeight/2, - sectionE.value/2, - widgetHeight/2);
    painter.drawLine(sectionE.value/2, - widgetHeight/2, widgetWidth/2 - sectionA.value, - widgetHeight/2);
    // Право
    painter.drawLine(widgetWidth/2, sectionA.value - widgetHeight/2, widgetWidth/2, widgetHeight/2 - sectionB.value);
    // Низ
    painter.drawLine(sectionC.value - widgetWidth/2, widgetHeight/2, - sectionF.value/2, widgetHeight/2);
    painter.drawLine(sectionF.value/2, widgetHeight/2, widgetWidth/2 - sectionB.value, widgetHeight/2);
    // Лево
    painter.drawLine(- widgetWidth/2, sectionD.value - widgetHeight/2, - widgetWidth/2, widgetHeight/2 - sectionC.value);

    // Универсальная отрисовка всех точек
    paintPoint(painter, sectionA,  widgetWidth/2 - sectionA.value, -widgetHeight/2 + sectionA.value,   +1, -1);
    paintPoint(painter, sectionB,  widgetWidth/2 - sectionB.value,  widgetHeight/2 - sectionB.value,   +1, +1);
    paintPoint(painter, sectionC, -widgetWidth/2 + sectionC.value,  widgetHeight/2 - sectionC.value,   -1, +1);
    paintPoint(painter, sectionD, -widgetWidth/2 + sectionD.value, -widgetHeight/2 + sectionD.value,   -1, -1);
    paintPoint(painter, sectionE, 0,                             -widgetHeight/2 + sectionE.value/2,  0, -1);
    paintPoint(painter, sectionF, 0,                              widgetHeight/2 - sectionF.value/2,  0, +1);
}

void PaintBase::figureInformation() {

    // Локальные переменные для сохранения изменений внутри диалогового окна
    int newwidth = figureWidth;
    int newheight = figureHeight;
    int newrotValue = rotValue;
    int newsectionA = sectionA.value;
    int newsectionB = sectionB.value;
    int newsectionC = sectionC.value;
    int newsectionD = sectionD.value;
    int newsectionE = sectionE.value;
    int newsectionF = sectionF.value;
    bool newRotationChanged = isRotationChanged;
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
        QBoxLayout* boxLayout = new QBoxLayout(QBoxLayout::TopToBottom);

        // Создание SpinBox для удобного заполнения данных пользователем.
        // Минимальное значение установлено согласно описанию выше или минимальное
        // значение переменной типа int;
        // максмальное значение равно максимальному значению переменной типа int.
        // Для выбора направления поворота созданы 2 RadioButton - по и против
        // часовой стрелки.
        QSpinBox *spinWidth = new QSpinBox();
        spinWidth->setRange(9, INT_MAX);
        spinWidth->setValue(newwidth);
        spinWidth->setSingleStep(5);

        QSpinBox *spinHeight = new QSpinBox();
        spinHeight->setRange(4, INT_MAX);
        spinHeight->setValue(newheight);
        spinHeight->setSingleStep(5);

        QSpinBox *spinRotate = new QSpinBox();
        spinRotate->setRange(INT_MIN, INT_MAX);
        spinRotate->setValue(newrotValue);
        spinRotate->setSingleStep(5);

        QRadioButton *rotateR = new QRadioButton("По часовой стрелке");
        QRadioButton *rotateL = new QRadioButton("Против часовой стрелки");
        if (newRotationChanged) {
            rotateR->setChecked(true);
        } else {
            rotateL->setChecked(true);
        }

        QSpinBox *spinsectionA = new QSpinBox();
        spinsectionA->setRange(1, INT_MAX);
        spinsectionA->setValue(newsectionA);
        spinsectionA->setSingleStep(5);

        QSpinBox *spinsectionB = new QSpinBox();
        spinsectionB->setRange(1, INT_MAX);
        spinsectionB->setValue(newsectionB);
        spinsectionB->setSingleStep(5);

        QSpinBox *spinsectionC = new QSpinBox();
        spinsectionC->setRange(1, INT_MAX);
        spinsectionC->setValue(newsectionC);
        spinsectionC->setSingleStep(5);

        QSpinBox *spinsectionD = new QSpinBox();
        spinsectionD->setRange(1, INT_MAX);
        spinsectionD->setValue(newsectionD);
        spinsectionD->setSingleStep(5);

        QSpinBox *spinsectionE = new QSpinBox();
        spinsectionE->setRange(2, INT_MAX);
        spinsectionE->setValue(newsectionE);
        spinsectionE->setSingleStep(5);

        QSpinBox *spinsectionF = new QSpinBox();
        spinsectionF->setRange(2, INT_MAX);
        spinsectionF->setValue(newsectionF);
        spinsectionF->setSingleStep(5);

        // Добавление вышеобъявленных и инициализированных виджетов
        // в QBoxLayout, предварительно добавляя описания поля ввода
        // в виде QLabel с текстом-информацией.
        boxLayout->addWidget(new QLabel("Информация о фигуре:"));
        boxLayout->addWidget(new QLabel("Периметр фигуры: " + QString::number(figurePerimeter)));
        boxLayout->addWidget(new QLabel("Площадь фигуры: " + QString::number(figureArea)));
        boxLayout->addWidget(new QLabel("Ширина:"));
        boxLayout->addWidget(spinWidth);
        boxLayout->addWidget(new QLabel("Высота:"));
        boxLayout->addWidget(spinHeight);
        boxLayout->addWidget(new QLabel("Угол поворота:"));
        boxLayout->addWidget(spinRotate);
        boxLayout->addWidget(rotateR);
        boxLayout->addWidget(rotateL);
        if (sectionA.type != Modifications::Nothing) {
            boxLayout->addWidget(new QLabel("Параметры точки А:"));
            boxLayout->addWidget(spinsectionA);
        }
        if (sectionB.type != Modifications::Nothing) {
            boxLayout->addWidget(new QLabel("Параметры точки B:"));
            boxLayout->addWidget(spinsectionB);
        }
        if (sectionC.type != Modifications::Nothing) {
            boxLayout->addWidget(new QLabel("Параметры точки C:"));
            boxLayout->addWidget(spinsectionC);
        }
        if (sectionD.type != Modifications::Nothing) {
            boxLayout->addWidget(new QLabel("Параметры точки D:"));
            boxLayout->addWidget(spinsectionD);
        }
        if (sectionE.type != Modifications::Nothing) {
            boxLayout->addWidget(new QLabel("Параметры точки E:"));
            boxLayout->addWidget(spinsectionE);
        }
        if (sectionF.type != Modifications::Nothing) {
            boxLayout->addWidget(new QLabel("Параметры точки F:"));
            boxLayout->addWidget(spinsectionF);
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
                        spinsectionA->value()*3 >= spinHeight->value() ||
                        spinsectionB->value()*3 >= spinHeight->value() ||
                        spinsectionC->value()*3 >= spinHeight->value() ||
                        spinsectionD->value()*3 >= spinHeight->value() ||
                        spinsectionE->value()*4 >= spinWidth->value()  ||
                        spinsectionF->value()*4 >= spinWidth->value()  ||
                        spinsectionE->value() >= spinHeight->value()   ||
                        spinsectionF->value() >= spinHeight->value()
                        )
        {

            newwidth = spinWidth->value();
            newheight = spinHeight->value();
            newrotValue = spinRotate->value();
            newsectionA = spinsectionA->value();
            newsectionB = spinsectionB->value();
            newsectionC = spinsectionC->value();
            newsectionD = spinsectionD->value();
            newsectionE = spinsectionE->value();
            newsectionF = spinsectionF->value();
            if (rotateR->isChecked()) {
                newRotationChanged = true;
            } else {
                newRotationChanged = false;
            }

            QBoxLayout* errorLayout = new QBoxLayout(QBoxLayout::TopToBottom);

            errorLayout->addWidget(new QLabel("Введенные данные некорректны"));
            createDialog(errorLayout, "Изменить");

        // Если данные корректны, они сохраняются в соответствующие поля класса,
        // после чего вызывается отрисовка виджета с измененными характеристиками,
        // а пользователю выводится окно с сообщением об успешном сохранении данных.
        } else {
            figureWidth = spinWidth->value();
            figureHeight = spinHeight->value();
            rotValue = spinRotate->value();
            sectionA.value = spinsectionA->value();
            sectionB.value = spinsectionB->value();
            sectionC.value = spinsectionC->value();
            sectionD.value = spinsectionD->value();
            sectionE.value = spinsectionE->value();
            sectionF.value = spinsectionF->value();
            if (rotateR->isChecked()) {
                isRotationChanged = true;
            } else {
                isRotationChanged = false;
            }

            isResized = false;
            this->update();

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
    // на кнопку к слоту закрытия окна и слоту figureInformation для
    // повторного открытия модального диалогового окна с информацией
    // о фигуре и возможностью ее изменения.
    QPushButton *cancelButton = new QPushButton(message2);
    boxLayout->addWidget(cancelButton);
    connect(cancelButton, SIGNAL(clicked()), modalDialog, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(figureInformation()));

    modalDialog->setLayout(boxLayout);
    modalDialog->exec();

    delete modalDialog;
}

std::pair<int, int> PaintBase::accumulateNewCoordinates (const std::pair<int, int>& oldCoordinates) const {
    // Для перевода градусов поля rotValue в радианы
    double degreesToRadians = 3.14159 / 180.0;
    // Перевод координат из исходной системы координат в
    // повернутую при помощи матрицы поворота в зависимости
    // от направления поворота.
    if (isRotationChanged) {
        return { ( std::cos(rotValue * degreesToRadians) * oldCoordinates.first + std::sin(rotValue * degreesToRadians) * oldCoordinates.second ),
                 (-std::sin(rotValue * degreesToRadians) * oldCoordinates.first + std::cos(rotValue * degreesToRadians) * oldCoordinates.second ) };
    } else {
        return { ( std::cos(rotValue * degreesToRadians) * oldCoordinates.first - std::sin(rotValue * degreesToRadians) * oldCoordinates.second ),
                 ( std::sin(rotValue * degreesToRadians) * oldCoordinates.first + std::cos(rotValue * degreesToRadians) * oldCoordinates.second ) };
    }
}

bool PaintBase::isPointOfFigure(QPoint point) const {
    int widgetWidth = figureWidth;
    int widgetHeight = figureHeight;
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
    if (point.x() >= (widgetWidth/2 - sectionA.value) && point.y() <= (-widgetHeight/2 + sectionA.value)) {
        return isPointOfRectOfPoint(point, sectionA, +1, -1);
    }
    // Квадрат точки B
    if (point.x() >= (widgetWidth/2 - sectionB.value) && point.y() >= (widgetHeight/2 - sectionB.value)) {
        return isPointOfRectOfPoint(point, sectionB, +1, +1);;
    }
    // Квадрат точки C
    if (point.x() <= (-widgetWidth/2 + sectionC.value) && point.y() >= (widgetHeight/2 - sectionC.value)) {
        return isPointOfRectOfPoint(point, sectionC, -1, +1);;
    }
    // Квадрат точки D
    if (point.x() <= (-widgetWidth/2 + sectionD.value) && point.y() <= (-widgetHeight/2 + sectionD.value)) {
        return isPointOfRectOfPoint(point, sectionD, -1, -1);;
    }
    // Прямоугольник точки E
    if (std::abs(point.x()) <= sectionE.value/2 && point.y() <= (-widgetHeight/2 + sectionE.value/2)) {
        return isPointOfRectOfPoint(point, sectionE, 0, -1);;
    }
    // Прямоугольник точки F
    if (std::abs(point.x()) <= sectionF.value/2 && point.y() >= (widgetHeight/2 - sectionF.value/2)) {
        return isPointOfRectOfPoint(point, sectionF, 0, 1);;
    }
    return true;
}

bool PaintBase::isPointOfRectOfPoint(QPoint point, const Sector& pointOfFigure, int modifierW, int modifierH) const {
    // Ширина и высота ограничивающего прямоугольника
    int widgetWidth = figureWidth;
    int widgetHeight = figureHeight;

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

void PaintBase::paintPoint(QPainter& painter, const Sector& point, const int posW, const int posH, int modifierW, int modifierH) const {
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
    figurePerimeter = 0;

    // Периметр описанного прямоугольника
    figurePerimeter += figureWidth * 2 + figureHeight * 2;

    // Убрать внешний периметр квадрата каждой точки
    figurePerimeter -= sectionA.value * 2 +
                 sectionB.value * 2 +
                 sectionC.value * 2 +
                 sectionD.value * 2 +
                 sectionE.value     +
                 sectionF.value;

    // Добавить длины линий каждой точки
    figurePerimeter += findPointPerimeter(sectionA) +
                 findPointPerimeter(sectionB) +
                 findPointPerimeter(sectionC) +
                 findPointPerimeter(sectionD) +
                 findPointPerimeter(sectionE, true) +
                 findPointPerimeter(sectionF, true);
}

double PaintBase::findPointPerimeter(const Sector& pointOfFigure, bool issectionEOrsectionF) const {
    // Вычисления в зависимости от типа точки и установленного модификатора
    switch(pointOfFigure.type) {
    case Modifications::Nothing :
        if (!issectionEOrsectionF) {
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
    figureArea = 0;

    // Площадь описанного прямоугольника
    figureArea += figureWidth * figureHeight;

    // Убрать площадь квадрата каждой точки
    figureArea -= sectionA.value * sectionA.value   +
            sectionB.value * sectionB.value   +
            sectionC.value * sectionC.value   +
            sectionD.value * sectionD.value   +
            sectionE.value * sectionE.value/2 +
            sectionF.value * sectionF.value/2;

    // Добавить площади, ограничиваемые линиями каждой точки
    figureArea += findPointArea(sectionA) +
            findPointArea(sectionB) +
            findPointArea(sectionC) +
            findPointArea(sectionD) +
            findPointArea(sectionE, true) +
            findPointArea(sectionF, true);
}

double PaintBase::findPointArea(const Sector& pointOfFigure, bool issectionEOrsectionF) const {
    // Вычисления в зависимости от типа точки и установленного модификатора
    switch(pointOfFigure.type) {
    case Modifications::Nothing :
        if (!issectionEOrsectionF) {
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
