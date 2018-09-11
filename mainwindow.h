#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcustomplot.h"

#include <QMainWindow>
#include <vector>
#include <QtGui>

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

    void drawGraph();
    void drawModel(int choiceModel);
    void getData();

private slots:
    void on_exit_clicked();
    void on_clear_clicked();
    void on_draw_clicked();
    void on_save_clicked();

    void on_LVM_BP_clicked();
    void on_NLVM_BP_clicked();
    void on_EVM_BP_clicked();
    void on_EFM_BP_clicked();
    void on_EVM_TP_clicked();
    void on_EFM_TP_clicked();

    void on_spaceParametr_valueChanged(int arg1);

    void on_valuePetrubationTVM_editingFinished();
    void on_valuePetrubationTFM_editingFinished();
    void on_valuePetrubationCVM_editingFinished();
    void on_valuePetrubationCFM_editingFinished();

private:
    Ui::MainWindow *ui;

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
};

#endif // MAINWINDOW_H
