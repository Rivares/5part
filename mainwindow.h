#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include "qcustomplot.h"
#include <QMainWindow>
#include <QtGui>
#include <vector>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void drawGraph(bool notEmpty = false);
    void draw_Model(QPixmap *graph, int choiceModel);
    void recountPixels();
    void getData();

    typedef double (MainWindow::*bMM)(unsigned long long i, uint j);   // Function Pointer
    bMM MM[4];     // Functions Pointer
    double bTV(unsigned long long i, uint j);
    double bTF(unsigned long long i, uint j);
    double bCV(unsigned long long i, uint j);
    double bCF(unsigned long long i, uint j);

private slots:
    void on_exit_clicked();

    void on_clear_clicked();

    void on_draw_clicked();

    void on_save_clicked();

    void on_LHM_clicked();

    void on_NHM_clicked();

    void on_Mexch_clicked();

    void on_Hexch_clicked();

    void on_CouiceZspinBox_valueChanged(int arg1);

    void on_lineEdit_editingFinished();

    void on_lineEdit_2_editingFinished();

    void on_lineEdit_3_editingFinished();

    void on_lineEdit_4_editingFinished();

private:
    Ui::MainWindow *ui;
    //QCustomPlot *customPlot;

    void toFileMM(vector <vector <double> > MMM, string nameModel);

    // -----Model's heat exchenger parameters------
    double RvT, RfT, a0, PTV_L, PTV_N, PTF;

    // -----Model's mass exchenger parameters------
    double RvM, RfM, E, PCV, PCF;

    vector <vector <double> > TV;
    vector <vector <double> > TF;
    vector <vector <double> > CV;
    vector <vector <double> > CF;

    double leftX,rightX;
    double leftY,rightY;
    int pictWidth,pictHeight;
    double onePixelX,onePixelY;
    double Ox,Oy;
};

#endif // MAINWINDOW_H
