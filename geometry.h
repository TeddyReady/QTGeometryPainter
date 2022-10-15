#pragma once
#include "paintbase.h"

//Var 10 {A B4 C2 D4 E6 F5}
class Type1 : public PaintBase {
public:
    explicit Type1(QWidget *parent = nullptr);
};

//Var 20 {A4 B1 C4 D4 E5 F}
class Type2 : public PaintBase {
public:
    explicit Type2(QWidget *parent = nullptr);
};
