#include <QApplication>
#include <QTranslator>
#include <QMessageBox>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);    
    a.setQuitOnLastWindowClosed(false);

    MainWindow researcherWindow, operatorWindow;
    researcherWindow.setFixedSize(QSize(1015, 580));
    operatorWindow.setFixedSize(QSize(1015, 580));

    QTranslator myTranslator;
    myTranslator.load(QLocale::system(), "SA", "_", QApplication::applicationDirPath());

    a.installTranslator(&myTranslator);

    QMessageBox whoIsHe;
    whoIsHe.setWindowTitle(QString("%1").arg(QMessageBox::tr("Crossroads")));
    whoIsHe.setFixedSize(QSize(250, 100));

    whoIsHe.setText(QString(QMessageBox::tr("Who are you, Dartanyan or a true European?")));

    QPushButton *researcherButton = whoIsHe.addButton(QMessageBox::tr("Dartanyan"), QMessageBox::ActionRole);
    QPushButton *operatorButton = whoIsHe.addButton(QMessageBox::tr("True European"), QMessageBox::ActionRole);

    whoIsHe.setDefaultButton(operatorButton);

    whoIsHe.exec();

    if(whoIsHe.clickedButton() == operatorButton)
    {
        operatorWindow.show();
        qApp->exit();
        whoIsHe.close();
    }
    else
    {
        if(whoIsHe.clickedButton() == researcherButton)
        {
            researcherWindow.show();
            qApp->exit();
            whoIsHe.close();
        }
    }

    a.setQuitOnLastWindowClosed(true);
    return a.exec();
}
