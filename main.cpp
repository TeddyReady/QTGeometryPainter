//Variant 10/20
#include "mainwindow.h"
#include <QApplication>

PaintBase* CHOOSED_OBJECT;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.resize(1080,720);
    w.setWindowTitle("Geometry Madness");
    w.setWindowIcon(QIcon(":/new/prefix1/icon/thumbnail.png"));
    w.show();
    return app.exec();
}
