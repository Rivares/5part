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
    explicit MainWindow(QWidget *parent = NULL);
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
    void on_TP_ACU_clicked();

    void on_selectDRC_textChanged(QString dRCNew);

    void on_action3D_model_triggered();

    void on_actionCheck_stat_of_values_triggered();

    void on_spaceParametrBP_valueChanged(int countSpacePoints);
    void on_spaceParametrTP_valueChanged(int countSpacePoints);
    void on_spaceParametrACU_valueChanged(int countSpacePoints);
    void on_spaceParametrEVAP_valueChanged(int countSpacePoints);

private:
    Ui::MainWindow *uiMain;

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
