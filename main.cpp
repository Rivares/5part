#include <QApplication>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    //w.setFixedSize(QSize(850, 540));
    w.show();

    return a.exec();
}
