#include <QApplication>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setFixedSize(QSize(900, 550));
    w.show();

    return a.exec();
}
