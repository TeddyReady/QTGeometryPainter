#include "geometry.h"

Type1::Type1(QWidget *parent) : PaintBase(parent) {
    // A B4 C2 D4 E6 F5
    Modifications pointA = Modifications::Nothing;
    Modifications pointB = Modifications::Wave;
    Modifications pointC = Modifications::Tria;
    Modifications pointD = Modifications::Wave;
    Modifications pointE = Modifications::Sphere;
    Modifications pointF = Modifications::Rect;
    setUpPoints({pointA, pointB, pointC, pointD, pointE, pointF});
}

Type2::Type2(QWidget *parent) : PaintBase(parent) {
    // A4 B1 C4 D4 E5 F
    Modifications pointA = Modifications::Wave;
    Modifications pointB = Modifications::Square;
    Modifications pointC = Modifications::Wave;
    Modifications pointD = Modifications::Wave;
    Modifications pointE = Modifications::Rect;
    Modifications pointF = Modifications::Nothing;
    setUpPoints({pointA, pointB, pointC, pointD, pointE, pointF});
}
