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
    void on_ACU_clicked();
    void on_EVAP_clicked();

    void on_spaceParametr_valueChanged(int countSpacePoints);

    void on_selectDRC_textChanged(QString dRCNew);

    void on_valuePetrubationTVM_textChanged(QString P_TV_New);
    void on_valuePetrubationTFM_textChanged(QString P_TF_New);
    void on_valuePetrubationCVM_textChanged(QString P_CV_New);
    void on_valuePetrubationCFM_textChanged(QString P_CF_New);  

    void on_action3D_model_triggered();

private:
    Ui::MainWindow *ui;

    vector <vector <double> > TV;
    vector <vector <double> > TF;
    vector <vector <double> > CV;
    vector <vector <double> > CF;
    vector <vector <double> > TB;
    vector <vector <double> > TFG;

    double leftX,rightX;
    double leftY,rightY;
};

#endif // MAINWINDOW_H
