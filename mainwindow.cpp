#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include "checkwindow.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <Qt3DExtras>
#include <QString>
#include <QDebug>
#include <vector>
#include <cmath>
#include <QUrl>
#include <array>
#include <vector>
#include <thread>
#include <time.h>
#include <fstream>
#include <iostream>
#include <functional>   //ref()

using std::cout;
using std::endl;

static bool okey = false;

static double dRC = 0.0;
static double dh = 0.0000;
static double dt = 0.0000;
static unsigned int selectZ = 0;
static unsigned long long selectN = 0;


vector <double> initLayerTV;
vector <double> initLayerTF;
vector <double> initLayerCV;
vector <double> initLayerCF;
vector <double> initLayerTB;
vector <double> initLayerTFG;

//---------------Borders--------------//
static double initLayerTV_0 = 0.0, initLayerTV_1 = 0.0;
static double initLayerTF_0 = 0.0, initLayerTF_1 = 0.0;
static double initLayerCV_0 = 0.0, initLayerCV_1 = 0.0;
static double initLayerCF_0 = 0.0, initLayerCF_1 = 0.0;
static double initLayerTB_0 = 0.0, initLayerTB_1 = 0.0;
static double initLayerTFG_0 = 0.0, initLayerTFG_1 = 0.0;

//----------Petrtubation----------//
static double P_TV = 0.0;
static double P_TF = 0.0;
static double P_CV = 0.0;
static double P_CF = 0.0;
//static double P_TB = 0.0;
//static double P_TFG = 0.0;


bool TMTPLMM(vector <vector <double> > &TV, vector <vector <double> > &TF);
bool TMTPNMM(vector <vector <double> > &TV, vector <vector <double> > &TF);
bool ETMBPMM(vector <vector <double> > &TV, vector <vector <double> > &TF,
             vector <vector <double> > &CV, vector <vector <double> > &CF);
bool ETMTPMM(vector <vector <double> > &TV, vector <vector <double> > &TF,
             vector <vector <double> > &CV, vector <vector <double> > &CF);
bool ACUMM(vector <vector <double> > &TV, vector <vector <double> > &TB);
bool EVAP(vector <vector <double> > &TF, vector <vector <double> > &TB,
          vector <vector <double> > &TFG);

void initialLayerTV(vector <vector <double> > &TV);         // Would be relize as template
void initialLayerTF(vector <vector <double> > &TF);
void initialLayerCV(vector <vector <double> > &CV);
void initialLayerCF(vector <vector <double> > &CF);
void initialLayerTB(vector <vector <double> > &TB);
void initialLayerTFG(vector <vector <double> > &TFG);

void toFileMM(vector <vector <double> > MMM, string nameModel);



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), uiMain(new Ui::MainWindow)
{
        uiMain->setupUi(this);

        uiMain->inputLeftX->setReadOnly(true);

        selectZ = static_cast <size_t>(uiMain->tableBordersAndInitialConditions_1->columnCount());
        selectN = uiMain->inputRightX->text().toULongLong(&okey, 10);
        dRC = uiMain->selectDRC->text().toDouble(&okey);

        dh = dRC/(selectZ-2);

        uiMain->inputLeftY->clear();
        uiMain->inputRightY->clear();

        uiMain->inputLeftY->insert("0");
        uiMain->inputRightY->insert("777");

        leftY = uiMain->inputLeftY->text().toDouble();
        rightY = uiMain->inputRightY->text().toDouble();
}


MainWindow::~MainWindow()
{
    delete uiMain;
}

void MainWindow::getData()
{
    leftX = uiMain->inputLeftX->text().toDouble();
    rightX = uiMain->inputRightX->text().toDouble();
    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();
    selectN = uiMain->inputRightX->text().toULongLong(&okey, 10);
    selectZ = static_cast <size_t>(uiMain->tableBordersAndInitialConditions_1->columnCount());

    if(uiMain->LVM_BP->isChecked())
    {
        initLayerTV_0 = (uiMain->tableBordersAndInitialConditions_1->item(0, 0)->text()).toDouble();
        initLayerTV_1 = (uiMain->tableBordersAndInitialConditions_1->item(0, uiMain->tableBordersAndInitialConditions_1->columnCount()-1)->text()).toDouble();

        initLayerTF_0 = (uiMain->tableBordersAndInitialConditions_2->item(0, 0)->text()).toDouble();
        initLayerTF_1 = (uiMain->tableBordersAndInitialConditions_2->item(0, uiMain->tableBordersAndInitialConditions_2->columnCount()-1)->text()).toDouble();

        initLayerTV.assign((selectZ-2), 0.0);
        initLayerTF.assign((selectZ-2), 0.0);

        for(uint j = 1; j < (selectZ-1); ++j)
        {
            initLayerTV[j-1] = (uiMain->tableBordersAndInitialConditions_1->item(0, j)->text()).toDouble();
            initLayerTF[j-1] = (uiMain->tableBordersAndInitialConditions_2->item(0, j)->text()).toDouble();
        }
    }

    if(uiMain->NLVM_BP->isChecked())
    {
        initLayerTV_0 = (uiMain->tableBordersAndInitialConditions_1->item(0, 0)->text()).toDouble();
        initLayerTV_1 = (uiMain->tableBordersAndInitialConditions_1->item(0, uiMain->tableBordersAndInitialConditions_1->columnCount()-1)->text()).toDouble();

        initLayerTF_0 = (uiMain->tableBordersAndInitialConditions_2->item(0, 0)->text()).toDouble();
        initLayerTF_1 = (uiMain->tableBordersAndInitialConditions_2->item(0, uiMain->tableBordersAndInitialConditions_2->columnCount()-1)->text()).toDouble();

        initLayerTV.assign((selectZ-2), 0.0);
        initLayerTF.assign((selectZ-2), 0.0);

        for(uint j = 1; j <= (selectZ-2); ++j)
        {
            initLayerTV[j-1] = (uiMain->tableBordersAndInitialConditions_1->item(0, j)->text()).toDouble();
            initLayerTF[j-1] = (uiMain->tableBordersAndInitialConditions_2->item(0, j)->text()).toDouble();
        }
    }

    if( (uiMain->EVM_BP->isChecked()) | (uiMain->EFM_BP->isChecked()) )
    {
        initLayerTV_0 = (uiMain->tableBordersAndInitialConditions_1->item(0, 0)->text()).toDouble();
        initLayerTV_1 = (uiMain->tableBordersAndInitialConditions_1->item(0, uiMain->tableBordersAndInitialConditions_1->columnCount()-1)->text()).toDouble();

        initLayerTF_0 = (uiMain->tableBordersAndInitialConditions_2->item(0, 0)->text()).toDouble();
        initLayerTF_1 = (uiMain->tableBordersAndInitialConditions_2->item(0, uiMain->tableBordersAndInitialConditions_2->columnCount()-1)->text()).toDouble();

        initLayerCV_0 = (uiMain->tableBordersAndInitialConditions_3->item(0, 0)->text()).toDouble();
        initLayerCV_1 = (uiMain->tableBordersAndInitialConditions_3->item(0, uiMain->tableBordersAndInitialConditions_3->columnCount()-1)->text()).toDouble();

        initLayerCF_0 = (uiMain->tableBordersAndInitialConditions_4->item(0, 0)->text()).toDouble();
        initLayerCF_1 = (uiMain->tableBordersAndInitialConditions_4->item(0, uiMain->tableBordersAndInitialConditions_4->columnCount()-1)->text()).toDouble();

        initLayerTV.assign((selectZ-2), 0.0);
        initLayerTF.assign((selectZ-2), 0.0);
        initLayerCV.assign((selectZ-2), 0.0);
        initLayerCF.assign((selectZ-2), 0.0);

        for(uint j = 1; j <= (selectZ-2); ++j)
        {
            initLayerTV[j-1] = (uiMain->tableBordersAndInitialConditions_1->item(0, j)->text()).toDouble();
            initLayerTF[j-1] = (uiMain->tableBordersAndInitialConditions_2->item(0, j)->text()).toDouble();
            initLayerCV[j-1] = (uiMain->tableBordersAndInitialConditions_3->item(0, j)->text()).toDouble();
            initLayerCF[j-1] = (uiMain->tableBordersAndInitialConditions_4->item(0, j)->text()).toDouble();
        }
    }

    if( (uiMain->EVM_TP->isChecked()) | (uiMain->EFM_TP->isChecked()) )
    {
        initLayerTV_0 = (uiMain->tableBordersAndInitialConditions_1->item(0, 0)->text()).toDouble();
        initLayerTV_1 = (uiMain->tableBordersAndInitialConditions_1->item(0, uiMain->tableBordersAndInitialConditions_1->columnCount()-1)->text()).toDouble();

        initLayerTF_0 = (uiMain->tableBordersAndInitialConditions_2->item(0, 0)->text()).toDouble();
        initLayerTF_1 = (uiMain->tableBordersAndInitialConditions_2->item(0, uiMain->tableBordersAndInitialConditions_2->columnCount()-1)->text()).toDouble();

        initLayerCV_0 = (uiMain->tableBordersAndInitialConditions_3->item(0, 0)->text()).toDouble();
        initLayerCV_1 = (uiMain->tableBordersAndInitialConditions_3->item(0, uiMain->tableBordersAndInitialConditions_3->columnCount()-1)->text()).toDouble();

        initLayerCF_0 = (uiMain->tableBordersAndInitialConditions_4->item(0, 0)->text()).toDouble();
        initLayerCF_1 = (uiMain->tableBordersAndInitialConditions_4->item(0, uiMain->tableBordersAndInitialConditions_4->columnCount()-1)->text()).toDouble();

        initLayerTV.assign((selectZ-2), 0.0);
        initLayerTF.assign((selectZ-2), 0.0);
        initLayerCV.assign((selectZ-2), 0.0);
        initLayerCF.assign((selectZ-2), 0.0);

        for(uint j = 1; j <= (selectZ-2); ++j)
        {
            initLayerTV[j-1] = (uiMain->tableBordersAndInitialConditions_1->item(0, j)->text()).toDouble();
            initLayerTF[j-1] = (uiMain->tableBordersAndInitialConditions_2->item(0, j)->text()).toDouble();
            initLayerCV[j-1] = (uiMain->tableBordersAndInitialConditions_3->item(0, j)->text()).toDouble();
            initLayerCF[j-1] = (uiMain->tableBordersAndInitialConditions_4->item(0, j)->text()).toDouble();
        }
    }

    if(uiMain->ACU->isChecked())
    {
        initLayerTV_0 = (uiMain->tableBordersAndInitialConditions_1->item(0, 0)->text()).toDouble();
        initLayerTV_1 = (uiMain->tableBordersAndInitialConditions_1->item(0, uiMain->tableBordersAndInitialConditions_1->columnCount()-1)->text()).toDouble();

        initLayerTB_0 = (uiMain->tableBordersAndInitialConditions_2->item(0, 0)->text()).toDouble();
        initLayerTB_1 = (uiMain->tableBordersAndInitialConditions_2->item(0, uiMain->tableBordersAndInitialConditions_2->columnCount()-1)->text()).toDouble();

        initLayerTV.assign((selectZ-2), 0.0);
        initLayerTB.assign((selectZ-2), 0.0);

        for(uint j = 1; j <= (selectZ-2); ++j)
        {
            initLayerTV[j-1] = (uiMain->tableBordersAndInitialConditions_1->item(0, j)->text()).toDouble();
            initLayerTB[j-1] = (uiMain->tableBordersAndInitialConditions_2->item(0, j)->text()).toDouble();
        }
    }

    if(uiMain->EVAP->isChecked())
    {
        initLayerTF_0 = (uiMain->tableBordersAndInitialConditions_1->item(0, 0)->text()).toDouble();
        initLayerTF_1 = (uiMain->tableBordersAndInitialConditions_1->item(0, uiMain->tableBordersAndInitialConditions_1->columnCount()-1)->text()).toDouble();

        initLayerTB_0 = (uiMain->tableBordersAndInitialConditions_2->item(0, 0)->text()).toDouble();
        initLayerTB_1 = (uiMain->tableBordersAndInitialConditions_2->item(0, uiMain->tableBordersAndInitialConditions_2->columnCount()-1)->text()).toDouble();

        initLayerTFG_0 = (uiMain->tableBordersAndInitialConditions_3->item(0, 0)->text()).toDouble();
        initLayerTFG_1 = (uiMain->tableBordersAndInitialConditions_3->item(0, uiMain->tableBordersAndInitialConditions_3->columnCount()-1)->text()).toDouble();

        initLayerTF.assign((selectZ-2), 0.0);
        initLayerTB.assign((selectZ-2), 0.0);
        initLayerTFG.assign((selectZ-2), 0.0);

        for(uint j = 1; j <= (selectZ-2); ++j)
        {
            initLayerTF[j-1]  = (uiMain->tableBordersAndInitialConditions_1->item(0, j)->text()).toDouble();
            initLayerTB[j-1]  = (uiMain->tableBordersAndInitialConditions_2->item(0, j)->text()).toDouble();
            initLayerTFG[j-1] = (uiMain->tableBordersAndInitialConditions_3->item(0, j)->text()).toDouble();
        }
    }

    dt = (uiMain->selectStepT->text().toDouble() <= 0.0)? 0.01 : abs(uiMain->selectStepT->text().toDouble());
    dh = dRC/(selectZ-2);
}

void MainWindow::drawGraph()
{
    clock_t timeMW_1 = clock();
    int msec = 0;

    uiMain->firstState->clear();
    uiMain->secondState->clear();
    uiMain->thirdState->clear();

    // configure right and top axis to show ticks but no labels:
    uiMain->customPlot->xAxis2->setVisible(true);
    uiMain->customPlot->xAxis2->setTickLabels(false);
    uiMain->customPlot->yAxis2->setVisible(true);
    uiMain->customPlot->yAxis2->setTickLabels(false);

    // give the axes some labels:
    uiMain->customPlot->xAxis->setLabel("t, sec");

    uiMain->customPlot->yAxis->setLabel("Магия, dB");

    if(uiMain->LVM_BP->isChecked() || uiMain->NLVM_BP->isChecked() || uiMain->EVM_BP->isChecked() || uiMain->EVM_TP->isChecked())
        uiMain->customPlot->yAxis->setLabel("Tемпература, 'C");

    if(uiMain->EFM_BP->isChecked() || uiMain->EFM_TP->isChecked())
        uiMain->customPlot->yAxis->setLabel("Концентрация, %");

    uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
    uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    /*----------------------------------------------------------------------*/

    // Out to display steady-state value temperature
    QList <QString> listStatesFirst, listStatesSecond, listStatesThird;

    uiMain->statusBar->showMessage(QString("(!) Calculating the mathematical model... (!)"));

    if (uiMain->LVM_BP->isChecked())
    {
        if(!TMTPLMM(TV, TF))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            uiMain->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));

            drawModel(0);

            for(uint j = 1; j < selectZ-1; ++j)
            {
                listStatesFirst.append(QString::number(TV[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
                listStatesSecond.append(QString::number(TF[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
            }
        }
    }

    if (uiMain->NLVM_BP->isChecked())
    {       
        if(!TMTPNMM(TV, TF))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            uiMain->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));

            drawModel(0);

            for(uint j = 1; j < selectZ-1; ++j)
            {
                listStatesFirst.append(QString::number(TV[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
                listStatesSecond.append(QString::number(TF[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
            }
        }
    }

    if((uiMain->EVM_BP->isChecked()) || (uiMain->EFM_BP->isChecked()))
    {       
        if(!ETMBPMM(TV, TF, CV, CF))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            if((uiMain->EVM_BP->isChecked()))
            {
                drawModel(0);

                for(uint j = 1; j < selectZ-1; ++j)
                {
                    listStatesFirst.append(QString::number(TV[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
                    listStatesSecond.append(QString::number(TF[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
                }
            }
            else
            {
                drawModel(1);

                // Out to display steady-state concentration of absorbent
                for(uint j = 1; j < selectZ-1; ++j)
                {
                    listStatesFirst.append(QString::number(CV[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
                    listStatesSecond.append(QString::number(CF[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
                }
            }

            uiMain->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));
        }
    }

    if((uiMain->EVM_TP->isChecked()) || (uiMain->EFM_TP->isChecked()))
    {       
        if(!ETMTPMM(TV, TF, CV, CF))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            if((uiMain->EVM_TP->isChecked()))
            {
                drawModel(0);

                for(uint j = 1; j < selectZ-1; ++j)
                {
                    listStatesFirst.append(QString::number(TV[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
                    listStatesSecond.append(QString::number(TF[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
                }
            }
            else
            {
                drawModel(1);

                // Out to display steady-state concentration of absorbent
                for(uint j = 1; j < selectZ-1; ++j)
                {
                    listStatesFirst.append(QString::number(CV[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
                    listStatesSecond.append(QString::number(CF[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
                }
            }

            uiMain->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));
        }
    }

    if((uiMain->ACU->isChecked()))
    {       
        if(!ACUMM(TV, TB))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            uiMain->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));

            drawModel(2);

            // Out to display steady-state value temperature (ACU)
            for(uint j = 1; j < selectZ-1; ++j)
            {
                listStatesFirst.append(QString::number(TV[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
                listStatesSecond.append(QString::number(TB[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
            }
        }
    }

    if((uiMain->EVAP->isChecked()))
    {
        if(!EVAP(TF, TB, TFG))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            uiMain->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));

            drawModel(3);

            // Out to display steady-state value temperature (EVAP)
            for(uint j = 1; j < selectZ-1; ++j)
            {
                listStatesFirst.append(QString::number(TF[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
                listStatesSecond.append(QString::number(TB[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
                listStatesThird.append(QString::number(TFG[ static_cast <size_t> ((selectN / dt) - 1) ][j]));
            }
        }
    }

    uiMain->statusBar->showMessage(QString("Ready!"));

    /*----------------------------------------------------------------------*/

    QStringList firstStates(listStatesFirst);
    QStringList secondStates(listStatesSecond);
    QStringList thirdStates(listStatesThird);

    uiMain->firstState->addItems(firstStates);
    uiMain->secondState->addItems(secondStates);
    uiMain->thirdState->addItems(thirdStates);

    clock_t timeDiff = clock() - timeMW_1;
    if (timeDiff != 0)
    {
        msec = (double)(timeDiff * 1000) / (double)CLOCKS_PER_SEC;
        cout << endl <<"Drawing time of program taken " << msec/1000 << " seconds, and " << msec%1000 <<" milliseconds!" << endl;
    }
    return;
}

void MainWindow::on_exit_clicked()
{
    this->close();
}

void MainWindow::on_clear_clicked()
{
    uiMain->customPlot->clearGraphs();
    uiMain->customPlot->replot();
}

void MainWindow::on_draw_clicked()
{
    getData();
    drawGraph();
}

void MainWindow::drawModel(int choiceModel)
{
    /*-------------Prepare data to output to screen------------------*/

    QVector <double> t(static_cast <size_t>(selectN / dt));

    QVector <QVector <double>> M_mat0, M_mat1, M_mat2;

    // Convert from std::vector <std::vector <double>> to QVector <QVector <double>>
    for(uint i = 0; i < selectZ; ++i)
    {
        QVector <double> tmpVectorM0, tmpVectorM1, tmpVectorM2;

        for(uint j = 0; j < (static_cast <size_t>((selectN / dt))); ++j)
        {
            if(choiceModel == 0)    // Heat exchange part
            {
                tmpVectorM0.push_back(TV[j][i]);
                tmpVectorM1.push_back(TF[j][i]);
            }

            if(choiceModel == 1)    // Mass transfer part
            {
                tmpVectorM0.push_back(CV[j][i]);
                tmpVectorM1.push_back(CF[j][i]);
            }

            if(choiceModel == 2)    // Condensation part
            {
                tmpVectorM0.push_back(TV[j][i]);
                tmpVectorM1.push_back(TB[j][i]);
            }

            if(choiceModel == 3)    // Evaporation part
            {
                tmpVectorM0.push_back(TF[j][i]);
                tmpVectorM1.push_back(TB[j][i]);
                tmpVectorM2.push_back(TFG[j][i]);
            }

            t[j] = j;
        }

        if( (choiceModel == 0) || (choiceModel == 1) || (choiceModel == 2) )
        {
            M_mat0.push_back(tmpVectorM0);
            M_mat1.push_back(tmpVectorM1);
        }

        if(choiceModel == 3)
        {
            M_mat0.push_back(tmpVectorM0);
            M_mat1.push_back(tmpVectorM1);
            M_mat2.push_back(tmpVectorM2);
        }
    }


    /*-------------Choice drawing processes------------------*/

    uint countModels = 0;
    QVector <QVector <double>> drawingProcces_0, drawingProcces_1, drawingProcces_2;

    if( (choiceModel == 0) || (choiceModel == 1) || (choiceModel == 2) )
    {
        drawingProcces_0 = M_mat0;
        drawingProcces_1 = M_mat1;
        countModels = 2;
    }

    if(choiceModel == 3)
    {
        drawingProcces_0 = M_mat0;
        drawingProcces_1 = M_mat1;
        drawingProcces_2 = M_mat2;
        countModels = 3;
    }


    /*-------------Rendering graphics------------------*/

    uiMain->customPlot->clearGraphs();
    uiMain->customPlot->legend->setVisible(true);

    srand(time(0));
    int randColorR, randColorG, randColorB;

    QPen pen;
    qreal widthPen = 2.2;
    int translucent = 35;

    for(uint i = 0, j = 0, k = 0, counter = 1; i < (selectZ-2) * countModels; ++i, ++counter)
    {
        uiMain->customPlot->addGraph();

        randColorR = rand() % 255,
        randColorG = rand() % 255,
        randColorB = rand() % 255;

        pen.setColor(QColor(randColorR, randColorG, randColorB));

        if(counter <= (selectZ-2))
        {
            /*-------------Customize of pen to drawing first model------------------*/

            pen.setStyle(Qt::SolidLine);
            uiMain->customPlot->graph(i)->setName(QString(tr("First phase_%1")).arg(i));
        }
        else
            if(counter <= (selectZ-2) * (countModels-1))
            {
                /*-------------Customize of pen to drawing second model------------------*/

                pen.setStyle(Qt::DashLine);
                uiMain->customPlot->graph(i)->setName(QString(tr("Third phase_%1")).arg(k));
                ++k;
            }
            else
            {
                /*-------------Customize of pen to drawing third model------------------*/

                pen.setStyle(Qt::DotLine);
                uiMain->customPlot->graph(i)->setName(QString(tr("Second phase_%1")).arg(j));
                ++j;
            }

        pen.setWidthF(widthPen);
        uiMain->customPlot->graph(i)->setPen(pen);
        uiMain->customPlot->graph(i)->setBrush(QBrush(QColor (randColorR,
                                                          randColorG,
                                                          randColorB, translucent) ));
    }

    clock_t timeMW_Cust = clock();
    int msec = 0;

    for(uint i = 0, j = 1, k = 1, counter = 1; i < (selectZ-2) * countModels; ++i, ++counter)
    {
        if(counter <= (selectZ-2))
        {
            uiMain->customPlot->graph(i)->setData(t, drawingProcces_0[i+1]);
        }
        else
            if(counter <= (selectZ-2) * (countModels-1))
            {
                uiMain->customPlot->graph(i)->setData(t, drawingProcces_2[k]);
                ++k;
            }
            else
            {
                uiMain->customPlot->graph(i)->setData(t, drawingProcces_1[j]);
                ++j;
            }
    }

    uiMain->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    uiMain->customPlot->replot();

    clock_t timeDiff = clock() - timeMW_Cust;
    msec = (double)(timeDiff * 1000) / (double)CLOCKS_PER_SEC;
    cout << endl <<"(QCustomPlot) Drawing time of program taken " << msec/1000 << " seconds, and " << msec%1000 <<" milliseconds!" << endl;
}

void MainWindow::on_save_clicked()
{
    if(uiMain->LVM_BP->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, ref(TV), "TV_BP");
        std::thread threadToFileTFMM(toFileMM, ref(TF), "TF_BP");

        threadToFileTVMM.join();
        threadToFileTFMM.join();
    }

    if(uiMain->NLVM_BP->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, ref(TV), "NTV_BP");
        std::thread threadToFileTFMM(toFileMM, ref(TF), "NTF_BP");

        threadToFileTVMM.join();
        threadToFileTFMM.join();
    }

    if(uiMain->EVM_BP->isChecked() || uiMain->EFM_BP->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, ref(TV), "TV_BP");
        std::thread threadToFileTFMM(toFileMM, ref(TF), "TF_BP");
        std::thread threadToFileCVMM(toFileMM, ref(CV), "CV_BP");
        std::thread threadToFileCFMM(toFileMM, ref(CF), "CF_BP");

        threadToFileTVMM.join();
        threadToFileTFMM.join();
        threadToFileCVMM.join();
        threadToFileCFMM.join();
    }

    if(uiMain->EVM_TP->isChecked() || uiMain->EFM_TP->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, ref(TV), "TV_TP");
        std::thread threadToFileTFMM(toFileMM, ref(TF), "TF_TP");
        std::thread threadToFileCVMM(toFileMM, ref(CV), "CV_TP");
        std::thread threadToFileCFMM(toFileMM, ref(CF), "CF_TP");

        threadToFileTVMM.join();
        threadToFileTFMM.join();
        threadToFileCVMM.join();
        threadToFileCFMM.join();
    }

    if(uiMain->ACU->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, ref(TV), "TV_ACU");
        std::thread threadToFileTBMM(toFileMM, ref(TB), "TB_ACU");

        threadToFileTVMM.join();
        threadToFileTBMM.join();
    }

    if(uiMain->EVAP->isChecked())
    {
        std::thread threadToFileTFMM(toFileMM, ref(TF), "TF_EVAP");
        std::thread threadToFileTBMM(toFileMM, ref(TB), "TB_EVAP");
        std::thread threadToFileTFGMM(toFileMM, ref(TFG), "TFG_EVAP");

        threadToFileTFMM.join();
        threadToFileTBMM.join();
        threadToFileTFGMM.join();
    }

    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString name;

    if(date.day() < 10)
        name += "0";

    name += QString::number(date.day()) + ".";

    if(date.month()<10)
        name += "0";

    name += QString::number(date.month()) + ".";
    name += QString::number(date.year()) + "_";

    if(time.hour() < 10)
        name += "0";

    name += QString::number(time.hour()) + "-";

    if(time.minute() < 10)
        name += "0";

    name += QString::number(time.minute()) + "-";

    if(time.second() < 10)
        name += "0";

    name += QString::number(time.second());

    QFile file(name + ".png");
    qDebug() << name;
    file.open(QIODevice::WriteOnly);

    QMessageBox msgBox;
    msgBox.setStandardButtons(QMessageBox::Ok);

    if(uiMain->customPlot->savePng(name + ".png", 0, 0, 1.0, -1))
    {
        msgBox.setText("Saved to program folder with name: " + name + ".png");
        msgBox.setWindowTitle("Saved!");
    }
    else
    {
        msgBox.setText("Error saving.");
        msgBox.setWindowTitle("Error!");
    }

    msgBox.exec();
}

void MainWindow::on_LVM_BP_clicked()
{
    uiMain->valuePetrubationCVM->hide();
    uiMain->valuePetrubationCFM->hide();

    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->inputLeftY->insert("100");
    uiMain->inputRightY->insert("170");
    uiMain->inputRightX->insert("10000");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(1.400));
    uiMain->spaceParametr->setValue(3);


    uiMain->tableBordersAndInitialConditions_1->setItem(0, 0, new QTableWidgetItem(tr("160.000")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, 0, new QTableWidgetItem(tr("160.000")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, 1, new QTableWidgetItem(tr("156.999750")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 2, new QTableWidgetItem(tr("153.999500")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 3, new QTableWidgetItem(tr("150.999250")));


    uiMain->tableBordersAndInitialConditions_2->setItem(0, 0, new QTableWidgetItem(tr("120.377000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, 0, new QTableWidgetItem(tr("120.377000")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, 1, new QTableWidgetItem(tr("123.382500")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 2, new QTableWidgetItem(tr("126.388000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 3, new QTableWidgetItem(tr("129.393500")));


    uiMain->tableBordersAndInitialConditions_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));


    uiMain->tableBordersAndInitialConditions_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_4->setDisabled(true);
}

void MainWindow::on_NLVM_BP_clicked()
{
    uiMain->valuePetrubationCVM->hide();
    uiMain->valuePetrubationCFM->hide();

    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->inputLeftY->insert("100");
    uiMain->inputRightY->insert("170");
    uiMain->inputRightX->insert("10000");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(1.400));
    uiMain->spaceParametr->setValue(3);

    uiMain->tableBordersAndInitialConditions_1->setItem(0, 0, new QTableWidgetItem(tr("160.000")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, 0, new QTableWidgetItem(tr("160.000")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, 1, new QTableWidgetItem(tr("156.999750")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 2, new QTableWidgetItem(tr("153.999500")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 3, new QTableWidgetItem(tr("150.999250")));


    uiMain->tableBordersAndInitialConditions_2->setItem(0, 0, new QTableWidgetItem(tr("120.377000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, 0, new QTableWidgetItem(tr("120.377000")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, 1, new QTableWidgetItem(tr("123.382500")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 2, new QTableWidgetItem(tr("126.388000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 3, new QTableWidgetItem(tr("129.393500")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));


    uiMain->tableBordersAndInitialConditions_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_4->setDisabled(true);
}

void MainWindow::on_EVM_BP_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->valuePetrubationCVM->show();
    uiMain->valuePetrubationCFM->show();

    uiMain->inputLeftY->insert("100");
    uiMain->inputRightY->insert("170");
    uiMain->inputRightX->insert("10000");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(1.400));
    uiMain->spaceParametr->setValue(3);

    //---------------Temperature part------------//
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 0, new QTableWidgetItem(tr("160.000")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, 0, new QTableWidgetItem(tr("160.000")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, 1, new QTableWidgetItem(tr("156.999750")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 2, new QTableWidgetItem(tr("153.999500")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 3, new QTableWidgetItem(tr("150.999250")));


    uiMain->tableBordersAndInitialConditions_2->setItem(0, 0, new QTableWidgetItem(tr("120.377000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, 0, new QTableWidgetItem(tr("120.377000")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, 1, new QTableWidgetItem(tr("123.382500")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 2, new QTableWidgetItem(tr("126.388000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 3, new QTableWidgetItem(tr("129.393500")));

    //---------------Concentration part------------//
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 0, new QTableWidgetItem(tr("67.9440")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, 0, new QTableWidgetItem(tr("67.9440")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("72.0440")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("72.0440")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, 1, new QTableWidgetItem(tr("68.9690")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 2, new QTableWidgetItem(tr("69.9940")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 3, new QTableWidgetItem(tr("71.0190")));


    uiMain->tableBordersAndInitialConditions_4->setItem(0, 0, new QTableWidgetItem(tr("6.550")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, 0, new QTableWidgetItem(tr("6.550")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("2.7880")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("2.7880")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, 1, new QTableWidgetItem(tr("5.60950")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 2, new QTableWidgetItem(tr("4.66900")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 3, new QTableWidgetItem(tr("3.72850")));

    uiMain->tableBordersAndInitialConditions_3->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_4->setDisabled(false);
}

void MainWindow::on_EFM_BP_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->valuePetrubationCVM->show();
    uiMain->valuePetrubationCFM->show();

    uiMain->inputLeftY->insert("0");
    uiMain->inputRightY->insert("90");
    uiMain->inputRightX->insert("10000");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(1.400));
    uiMain->spaceParametr->setValue(3);


    //---------------Temperature part------------//
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 0, new QTableWidgetItem(tr("160.000")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, 0, new QTableWidgetItem(tr("160.000")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, 1, new QTableWidgetItem(tr("156.999750")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 2, new QTableWidgetItem(tr("153.999500")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 3, new QTableWidgetItem(tr("150.999250")));


    uiMain->tableBordersAndInitialConditions_2->setItem(0, 0, new QTableWidgetItem(tr("120.377000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, 0, new QTableWidgetItem(tr("120.377000")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, 1, new QTableWidgetItem(tr("123.382500")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 2, new QTableWidgetItem(tr("126.388000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 3, new QTableWidgetItem(tr("129.393500")));

    //---------------Concentration part------------//
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 0, new QTableWidgetItem(tr("67.9440")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, 0, new QTableWidgetItem(tr("67.9440")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("72.0440")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("72.0440")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, 1, new QTableWidgetItem(tr("68.9650")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 2, new QTableWidgetItem(tr("69.9900")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 3, new QTableWidgetItem(tr("71.0150")));


    uiMain->tableBordersAndInitialConditions_4->setItem(0, 0, new QTableWidgetItem(tr("6.550")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, 0, new QTableWidgetItem(tr("6.550")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("2.7880")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("2.7880")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, 1, new QTableWidgetItem(tr("5.570")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 2, new QTableWidgetItem(tr("4.640")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 3, new QTableWidgetItem(tr("3.710")));

    uiMain->tableBordersAndInitialConditions_3->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_4->setDisabled(false);
}

void MainWindow::on_EVM_TP_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->valuePetrubationCVM->show();
    uiMain->valuePetrubationCFM->show();

    uiMain->inputLeftY->insert("20");
    uiMain->inputRightY->insert("150");
    uiMain->inputRightX->insert("50000");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(3.510));
    uiMain->spaceParametr->setValue(3);


    //---------------Temperature part------------//
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 0, new QTableWidgetItem(tr("142.500")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, 0, new QTableWidgetItem(tr("142.500")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("45.300")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("45.300")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, 1, new QTableWidgetItem(tr("118.200000")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 2, new QTableWidgetItem(tr("93.9000000")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 3, new QTableWidgetItem(tr("69.6000000")));


    uiMain->tableBordersAndInitialConditions_2->setItem(0, 0, new QTableWidgetItem(tr("30.0000000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, 0, new QTableWidgetItem(tr("30.0000000")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("139.000000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("139.000000")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, 1, new QTableWidgetItem(tr("57.2500000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 2, new QTableWidgetItem(tr("84.5000000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 3, new QTableWidgetItem(tr("111.750000")));

    //---------------Concentration part------------//
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 0, new QTableWidgetItem(tr("0.5300000")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, 0, new QTableWidgetItem(tr("0.5300000")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("1.138000")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("1.138000")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, 1, new QTableWidgetItem(tr("0.6820000")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 2, new QTableWidgetItem(tr("0.8340000")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 3, new QTableWidgetItem(tr("0.9860000")));


    uiMain->tableBordersAndInitialConditions_4->setItem(0, 0, new QTableWidgetItem(tr("1.0000000")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, 0, new QTableWidgetItem(tr("1.0000000")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("0.5530000")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("0.5530000")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, 1, new QTableWidgetItem(tr("0.8882500")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 2, new QTableWidgetItem(tr("0.7765000")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 3, new QTableWidgetItem(tr("0.6647500")));

    uiMain->tableBordersAndInitialConditions_3->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_4->setDisabled(false);
}

void MainWindow::on_EFM_TP_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->valuePetrubationCVM->show();
    uiMain->valuePetrubationCFM->show();

    uiMain->inputLeftY->insert("0");
    uiMain->inputRightY->insert("1.5");
    uiMain->inputRightX->insert("50000");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(3.510));
    uiMain->spaceParametr->setValue(3);


    //---------------Temperature part------------//
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 0, new QTableWidgetItem(tr("142.500")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, 0, new QTableWidgetItem(tr("142.500")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("45.300")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("45.300")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, 1, new QTableWidgetItem(tr("118.200000")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 2, new QTableWidgetItem(tr("93.9000000")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 3, new QTableWidgetItem(tr("69.6000000")));


    uiMain->tableBordersAndInitialConditions_2->setItem(0, 0, new QTableWidgetItem(tr("30.0000000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, 0, new QTableWidgetItem(tr("30.0000000")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("139.000000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("139.000000")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, 1, new QTableWidgetItem(tr("57.2500000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 2, new QTableWidgetItem(tr("84.5000000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 3, new QTableWidgetItem(tr("111.750000")));

    //---------------Concentration part------------//
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 0, new QTableWidgetItem(tr("0.5300000")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, 0, new QTableWidgetItem(tr("0.5300000")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("1.138000")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("1.138000")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, 1, new QTableWidgetItem(tr("0.6820000")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 2, new QTableWidgetItem(tr("0.8340000")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 3, new QTableWidgetItem(tr("0.9860000")));


    uiMain->tableBordersAndInitialConditions_4->setItem(0, 0, new QTableWidgetItem(tr("1.0000000")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, 0, new QTableWidgetItem(tr("1.0000000")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("0.5530000")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("0.5530000")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, 1, new QTableWidgetItem(tr("0.8882500")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 2, new QTableWidgetItem(tr("0.7765000")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 3, new QTableWidgetItem(tr("0.6647500")));

    uiMain->tableBordersAndInitialConditions_3->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_4->setDisabled(false);
}

void MainWindow::on_ACU_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->valuePetrubationCVM->show();
    uiMain->valuePetrubationCFM->show();

    uiMain->inputLeftY->insert("0");
    uiMain->inputRightY->insert("80");
    uiMain->inputRightX->insert("30");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(4.0));
    uiMain->spaceParametr->setValue(3);

    //---------------Temperature part------------//
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 0, new QTableWidgetItem(tr("72.500")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, 0, new QTableWidgetItem(tr("72.500")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("42.49400")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("42.49400")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, 1, new QTableWidgetItem(tr("64.9985000")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 2, new QTableWidgetItem(tr("57.4970000")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 3, new QTableWidgetItem(tr("49.9955000")));


    uiMain->tableBordersAndInitialConditions_2->setItem(0, 0, new QTableWidgetItem(tr("30.7500000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, 0, new QTableWidgetItem(tr("30.7500000")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("23.2900000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("23.2900000")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, 1, new QTableWidgetItem(tr("28.8850000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 2, new QTableWidgetItem(tr("27.0200000")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 3, new QTableWidgetItem(tr("25.1550000")));


    uiMain->tableBordersAndInitialConditions_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));


    uiMain->tableBordersAndInitialConditions_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_4->setDisabled(true);

}

void MainWindow::on_EVAP_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->valuePetrubationCVM->show();
    uiMain->valuePetrubationCFM->show();

    uiMain->inputLeftY->insert("140");
    uiMain->inputRightY->insert("300");
    uiMain->inputRightX->insert("3000");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(5.5));
    uiMain->spaceParametr->setValue(4);


    if((uiMain->tableBordersAndInitialConditions_1->columnCount()-1) == 4)
    {
        uiMain->tableBordersAndInitialConditions_1->insertColumn(4);
        uiMain->tableBordersAndInitialConditions_2->insertColumn(4);
        uiMain->tableBordersAndInitialConditions_3->insertColumn(4);
        uiMain->tableBordersAndInitialConditions_4->insertColumn(4);
    }

    uiMain->tableBordersAndInitialConditions_1->setItem(0, 0, new QTableWidgetItem(tr("160.00")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, 0, new QTableWidgetItem(tr("160.00")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("139.00")));
    uiMain->tableBordersAndInitialConditions_1->setItem(1, (uiMain->tableBordersAndInitialConditions_1->columnCount()-1), new QTableWidgetItem(tr("139.00")));

    uiMain->tableBordersAndInitialConditions_1->setItem(0, 1, new QTableWidgetItem(tr("160.161650")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 2, new QTableWidgetItem(tr("154.220450")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 3, new QTableWidgetItem(tr("148.736080")));
    uiMain->tableBordersAndInitialConditions_1->setItem(0, 4, new QTableWidgetItem(tr("143.673400")));


    uiMain->tableBordersAndInitialConditions_2->setItem(0, 0, new QTableWidgetItem(tr("160.200")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, 0, new QTableWidgetItem(tr("160.200")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("147.07220")));
    uiMain->tableBordersAndInitialConditions_2->setItem(1, (uiMain->tableBordersAndInitialConditions_2->columnCount()-1), new QTableWidgetItem(tr("147.07220")));

    uiMain->tableBordersAndInitialConditions_2->setItem(0, 1, new QTableWidgetItem(tr("164.48250")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 2, new QTableWidgetItem(tr("158.20900")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 3, new QTableWidgetItem(tr("152.41800")));
    uiMain->tableBordersAndInitialConditions_2->setItem(0, 4, new QTableWidgetItem(tr("147.07220")));


    uiMain->tableBordersAndInitialConditions_3->setItem(0, 0, new QTableWidgetItem(tr("300.0")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, 0, new QTableWidgetItem(tr("300.0")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("240.54030")));
    uiMain->tableBordersAndInitialConditions_3->setItem(1, (uiMain->tableBordersAndInitialConditions_3->columnCount()-1), new QTableWidgetItem(tr("240.54030")));

    uiMain->tableBordersAndInitialConditions_3->setItem(0, 1, new QTableWidgetItem(tr("283.30650")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 2, new QTableWidgetItem(tr("267.89660")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 3, new QTableWidgetItem(tr("253.67150")));
    uiMain->tableBordersAndInitialConditions_3->setItem(0, 4, new QTableWidgetItem(tr("240.54030")));


    uiMain->tableBordersAndInitialConditions_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(1, (uiMain->tableBordersAndInitialConditions_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_4->setItem(0, 4, new QTableWidgetItem(tr("0.0")));


    uiMain->tableBordersAndInitialConditions_3->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_4->setDisabled(true);
}

//--------------------------LINER MODEL-----------------------------------
bool TMTPLMM(vector <vector <double> > &TV, vector <vector <double> > &TF)
{
    double  RvT = 0.0191806, RfT = 0.0000777,
            PTV = (0.05654433 * dt) / dh,
            PTF = (0.0002291314 * dt) / dh;

    if( (PTV > 0.5) || (PTF > 0.5) )
    {
        qDebug() << "The scheme diverges!!!";
        return false;
    }

    vector <double> bmp;
    bmp.assign(selectZ, 0.0);

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TF.erase(TF.begin(), TF.end());
    }

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.push_back(bmp);
        TF.push_back(bmp);
    }

    std::thread threadInitialLayerTV(initialLayerTV, ref(TV));
    std::thread threadInitialLayerTF(initialLayerTF, ref(TF));

    threadInitialLayerTV.join();
    threadInitialLayerTF.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << TV[0][j] << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << TF[0][j] << std::fixed << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)// time
    {
       for(uint j = 1; j < (selectZ-1); ++j)        // place
       {
           TV[i][j] = (dt * RvT * TF[i-1][(selectZ-1)-j])
                    + (PTV * TV[i-1][j-1])
                    + TV[i-1][j] * (-(dt * RvT) - PTV)
                    + TV[i-1][j];

           TF[i][j] = (dt * RfT * TV[i-1][(selectZ-1)-j])
                    + (PTF * TF[i-1][j-1])
                    + TF[i-1][j] * (-PTF - (dt * RfT))
                    + TF[i-1][j];
       }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TV[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TF[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    return true;
}

//--------------------------NON-LINER MODEL-----------------------------------
bool TMTPNMM(vector <vector <double> > &TV, vector <vector <double> > &TF)
{
    //double a0 = 0.0001152735759 or 0.00016966, What is TRUE?????????

    // -----Model's heat exchenger parameters------
    double  RvT = 0.0191806, RfT = 0.0000777, a0 = 1.6966E-4,
            PTV_L = (a0 * 273.15 * dt) / dh,
            PTV_N = 0.0,
            PTF = (0.0002291314 * dt) / dh;

    if( (PTV_L > 0.5) || (PTF > 0.5) )
    {
        qDebug() << "The scheme diverges!!!";
        return false;
    }

    vector <double> bmp;
    bmp.assign(selectZ, 0.0);

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TF.erase(TF.begin(), TF.end());
    }

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.push_back(bmp);
        TF.push_back(bmp);
    }

    std::thread threadInitialLayerTV(initialLayerTV, ref(TV));
    std::thread threadInitialLayerTF(initialLayerTF, ref(TF));

    threadInitialLayerTV.join();
    threadInitialLayerTF.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << TV[0][j] << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << TF[0][j] << std::fixed << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)
    {
       for(uint j = 1; j < (selectZ-1); ++j)
       {
           PTV_N = (a0 * TV[i-1][j] * dt) / dh;

           TV[i][j] = (dt * RvT * TF[i-1][(selectZ-1)-j])
                    - (PTV_N * TV[i-1][j+1])
                    - TV[i-1][j] * (dt*RvT + PTV_L - PTV_N)
                    + (PTV_L * TV[i-1][j-1])
                    + TV[i-1][j];

           TF[i][j] = (dt * RfT * TV[i-1][(selectZ-1)-j])
                    + (PTF * TF[i-1][j-1])
                    - TF[i-1][j] * (dt*RfT + PTF)
                    + TF[i-1][j];
        }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TV[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TF[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    return true;
}

//---------------------------INTERCONNECTED MODEL(BOTTOM PART)------------------------------
bool ETMBPMM(vector <vector <double> > &TV, vector <vector <double> > &TF,
             vector <vector <double> > &CV, vector <vector <double> > &CF)
{
    // -----Model's heat exchenger parameters------
    double  RvT = 0.0191806, RfT = 0.0000777, a0 = 1.6966E-4,
            PTV_L = (a0 * 273.15 * dt) / dh,
            PTV_N = 0.0,
            PTF = (0.0002291314 * dt) / dh;

    // -----Model's mass exchenger parameters------
    double E = 1.0E-9, RvM = 0.004302, RfM = 1.222E-5;
                       //RfM = 0.000010734, RvM = 0.0216487318;

    if( (PTV_L > 0.5) || (PTF > 0.5) )
    {
        qDebug() << "The scheme diverges!!!";
        return false;
    }

    vector <double> bmp;
    bmp.assign(selectZ, 0.0);

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TF.erase(TF.begin(), TF.end());
        CV.erase(CV.begin(), CV.end());
        CF.erase(CF.begin(), CF.end());
    }

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.push_back(bmp);
        TF.push_back(bmp);
        CV.push_back(bmp);
        CF.push_back(bmp);
    }

    std::thread threadInitialLayerTV(initialLayerTV, ref(TV));
    std::thread threadInitialLayerTF(initialLayerTF, ref(TF));
    std::thread threadInitialLayerCV(initialLayerCV, ref(CV));
    std::thread threadInitialLayerCF(initialLayerCF, ref(CF));

    threadInitialLayerTV.join();
    threadInitialLayerTF.join();
    threadInitialLayerCV.join();
    threadInitialLayerCF.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << TV[0][j] << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << TF[0][j] << " | ";
    }   cout << endl;

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << CV[0][j] << " | ";
    }   cout << endl;

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << CF[0][j] << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)// time
    {
        for(size_t j = 1; j < (selectZ-1); ++j)      // place
        {
            // -----Calculate layer heat exchenger model------
            PTV_N = (a0 * TV[i-1][j] * dt) / dh;

            TV[i][j] = (dt * RvT * TF[i-1][(selectZ-1)-j])
                    - (PTV_N * TV[i-1][j+1])
                    - TV[i-1][j] * (dt*RvT + PTV_L - PTV_N)
                    + (PTV_L * TV[i-1][j-1])
                    + TV[i-1][j];

            TF[i][j] = (dt * RfT * TV[i-1][(selectZ-1)-j])
                    + (PTF * TF[i-1][j-1])
                    - TF[i-1][j] * (dt*RfT + PTF)
                    + TF[i-1][j];

            // -----Calculate layer mass exchenger model------

            /* New schema: CV(i,i-1); CV(i,i-1). Error was increase..., but dynamic of process good*/         
            /* 1 A-
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM + PTV_L + PTV_N + 1)
                        - CV[i-1][j-1] * (PTV_L + PTV_N);*/

            /* 1 B+
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM - PTV_L - PTV_N + 1)
                        + CV[i-1][j-1] * (PTV_L + PTV_N);*/

            /* 2 A-
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM + PTV_L - PTV_N + 1)
                        - CV[i-1][j-1] * PTV_L
                        + CV[i-1][j+1] * PTV_N;*/
            /* 2 B-
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM - PTV_L + PTV_N + 1)
                        + CV[i-1][j-1] * PTV_L
                        - CV[i-1][j+1] * PTV_N;*/

            /* 3 A-
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM - PTV_L + PTV_N + 1)
                        - CV[i-1][j-1] * PTV_N
                        + CV[i-1][j+1] * PTV_L;*/

            /* 3 B-
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM + PTV_L - PTV_N + 1)
                        + CV[i-1][j-1] * PTV_N
                        - CV[i-1][j+1] * PTV_L;*/

            /* 4 A+*/
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM - PTV_L - PTV_N + 1)
                        + CV[i-1][j+1] * (PTV_L + PTV_N);

            /* 4 B-
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM + PTV_L + PTV_N + 1)
                        - CV[i-1][j+1] * (PTV_L + PTV_N);*/

            /* 1 A-
            CF[i][j] =  - (dt * RfM * CV[i-1][(selectZ-1)-j])
                        + CF[i-1][j] * (dt*RfM*E + PTF + 1)
                        - (PTF * CF[i-1][j-1]);*/

            /* 1 B+*/
            CF[i][j] =  - (dt * RfM * CV[i-1][j])
                        + CF[i-1][j] * (dt*RfM*E - PTF + 1)
                        + (PTF * CF[i-1][j-1]);

            /* 2 A-
            CF[i][j] =  - (dt * RfM * CV[i-1][(selectZ-1)-j])
                        + CF[i-1][j] * (dt*RfM*E - PTF + 1)
                        + (PTF * CF[i-1][j+1]);*/

            /* 2 B-
            CF[i][j] =  - (dt * RfM * CV[i-1][(selectZ-1)-j])
                        + CF[i-1][j] * (dt*RfM*E + PTF + 1)
                        - (PTF * CF[i-1][j+1]);*/
        }

    }

    cout << endl << "Steady-state values:" << endl;
    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TV[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TF[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << CV[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << CF[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    return true;
}

//---------------------------INTERCONNECTED MODEL(TOP PART)------------------------------
bool ETMTPMM(vector <vector <double> > &TV, vector <vector <double> > &TF,
             vector <vector <double> > &CV, vector <vector <double> > &CF)
{
    // -----Model's heat exchenger parameters------
    double  RvT = 2.500, RfT = 0.000085500, a0 = 0.0034868520, // (a0 = 0.0034868520) != (a0_Simulink = 0.002702752)
            PTV_L = (a0 * 273.15 * dt) / dh,
            PTV_N = 0.0,
            PTF = (0.0000400 * dt) / dh;

    // -----Model's mass exchenger parameters------
    double E = 1.0E-9, RvM = 0.1450, RfM = 6.0E-6;

    if( (PTV_L > 0.5) || (PTF > 0.5) )
    {
        qDebug() << "The scheme diverges!!!";
        return false;
    }

    vector <double> bmp;
    bmp.assign(selectZ, 0.0);

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TF.erase(TF.begin(), TF.end());
        CV.erase(CV.begin(), CV.end());
        CF.erase(CF.begin(), CF.end());
    }

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.push_back(bmp);
        TF.push_back(bmp);
        CV.push_back(bmp);
        CF.push_back(bmp);
    }

    std::thread threadInitialLayerTV(initialLayerTV, ref(TV));
    std::thread threadInitialLayerTF(initialLayerTF, ref(TF));
    std::thread threadInitialLayerCV(initialLayerCV, ref(CV));
    std::thread threadInitialLayerCF(initialLayerCF, ref(CF));

    threadInitialLayerTV.join();
    threadInitialLayerTF.join();
    threadInitialLayerCV.join();
    threadInitialLayerCF.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << TV[0][j] << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << TF[0][j] << " | ";
    }   cout << endl;

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << CV[0][j] << " | ";
    }   cout << endl;

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << CF[0][j] << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)// time
    {
        for(size_t j = 1; j < (selectZ-1); ++j)      // place
        {
            // -----Calculate layer heat exchenger model------
            PTV_N = (a0 * TV[i-1][j] * dt) / dh;

            TV[i][j] = (dt * RvT * TF[i-1][(selectZ-1)-j])
                    - (PTV_N * TV[i-1][j+1])
                    - TV[i-1][j] * (dt*RvT + PTV_L - PTV_N)
                    + (PTV_L * TV[i-1][j-1])
                    + TV[i-1][j];

            TF[i][j] = (dt * RfT * TV[i-1][(selectZ-1)-j])
                    + (PTF * TF[i-1][j-1])
                    - TF[i-1][j] * (dt*RfT + PTF)
                    + TF[i-1][j];

            // -----Calculate layer mass exchenger model------

            /* New schema: CV(i,i-1); CV(i,i-1). Error was increase..., but dynamic of process good*
            CV[i][j] = - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                    + (PTV_L + PTV_N) * CV[i-1][j-1]
                    - CV[i-1][j] * (- dt*RvM + PTV_L + PTV_N)
                    + CV[i-1][j];*/

            CF[i][j] = - (dt * RfM * CV[i-1][(selectZ-1)-j])
                    + (PTF * CF[i-1][j-1])
                    - CF[i-1][j] * (-(dt*RfM*E) + PTF)
                    + CF[i-1][j];

            /* Old schema: CV(i,i-1); CV(i+1,i)*/

            CV[i][j] = -CV[i-1][j] * (PTV_L - 1 - PTV_N + dt*RvM) + (PTV_L * CV[i-1][j-1]) - (PTV_N * CV[i-1][j-1])
                                                                                        - (dt * RvM * E * CF[i-1][(selectZ-1)-j]) + P_CV;

        }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TV[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TF[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << CV[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << CF[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    return true;
}

//---------------------------AIR-COOLING UNIT MODEL(ACU)------------------------------
bool ACUMM(vector<vector<double> > &TV, vector<vector<double> > &TB)
{
    // -----Model's heat parameters------
    double  RvT = 8.400, a0 = 0.06,     // a0 = 0.06 ?
            PTV_L = (a0 * 273.150) / dh;

    // -----Model's boarder parameters------
    double RB1 = 0.00661, RB2 = 0.480
         , TE = 12.380                  // Temperature of enviroment
         , CP = 134.0;                  // Count piplines;

    if((dt * PTV_L) > 0.5 )
    {
        qDebug() << "The scheme diverges!!!";
        return false;
    }

    vector <double> bmp;
    bmp.assign(selectZ, 0.0);

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TB.erase(TB.begin(), TB.end());
    }

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.push_back(bmp);
        TB.push_back(bmp);
    }

    std::thread threadInitialLayerTV(initialLayerTV, ref(TV));
    std::thread threadInitialLayerTB(initialLayerTB, ref(TB));

    threadInitialLayerTV.join();
    threadInitialLayerTB.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << TV[0][j] << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << TB[0][j] << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)    // time
    {
        for(size_t j = 1; j < (selectZ-1); ++j)         // place
        {
            // -----Calculate layer heat exchenger model------
            TV[i][j] = (dt * RvT * TB[i-1][(selectZ-1)-j])
                    - TV[i-1][j] * ((dt*RvT) + (dt*PTV_L))
                    + (dt * PTV_L * TV[i-1][j-1])
                    + TV[i-1][j];

            TB[i][j] = (dt * RB2 * TV[i-1][j])
                    + (CP*dt*RB1*TE)                        // ? TE - const or function?
                    - TB[i-1][j] * (CP*dt*RB1 + dt*RB2)
                    + TB[i-1][j];
        }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TV[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TB[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    return true;
}

bool EVAP(vector <vector <double> > &TF, vector <vector <double> > &TB, vector <vector <double> > &TFG)
{
    // -----Model's gas parameters------
    double  RFG = 0.470, PTFG = 4.60 / dh;

    // -----Model's fluide parameters------
    double  RF = 0.0030, PTF = 0.0030 / dh;

    // -----Model's boarder parameters------
    double  RFB = 0.220, RFGB = 0.0080;

    if((dt * PTFG) > 0.5 )
    {
        qDebug() << "The scheme diverges!!!";
        return false;
    }

    vector <double> bmp;
    bmp.assign(selectZ, 0.0);

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TF.erase(TF.begin(), TF.end());
        TB.erase(TB.begin(), TB.end());
        TFG.erase(TFG.begin(), TFG.end());
    }

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TF.push_back(bmp);
        TB.push_back(bmp);
        TFG.push_back(bmp);
    }

    std::thread threadInitialLayerTF(initialLayerTF, ref(TF));
    std::thread threadInitialLayerTB(initialLayerTB, ref(TB));
    std::thread threadInitialLayerTFG(initialLayerTFG, ref(TFG));

    threadInitialLayerTF.join();
    threadInitialLayerTB.join();
    threadInitialLayerTFG.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << TF[0][j] << " | ";
    }   cout << endl;

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << TB[0][j] << " | ";
    }   cout << endl;

    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << TFG[0][j] << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)    // time
    {
        for(size_t j = 1; j < (selectZ-1); ++j)         // place
        {
            TF[i][j] = (dt * RF * TB[i-1][j+1]) // ?
                    + (dt * PTF * TF[i-1][j+1])
                    - TF[i-1][j] * (dt*RF + dt*PTF)
                    + TF[i-1][j];

            TB[i][j] = (dt * RFB * TF[i-1][j]) // ?
                    + (dt * RFGB * TFG[i-1][(selectZ-1)-j])// ?
                    - TB[i-1][j] * (dt*RFGB + dt*RFB)
                    + TB[i-1][j];

            TFG[i][j] = (dt * RFG * TB[i-1][j+1]) // ?
                    + (dt * PTFG * TFG[i-1][j-1])
                    - TFG[i-1][j] * (dt*RFG + dt*PTFG)
                    + TFG[i-1][j];
        }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TF[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TB[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TFG[ static_cast <size_t>((selectN-2) / dt) ][j] << " | ";
    }cout << endl;



    string initTF = std::to_string(TF[0][3]);
    string nameModel = "MM_TF_" + initTF + ".dat";

    ofstream foutMM(nameModel, ios_base::out | ios_base::trunc);

    for(size_t i = 0; i < (selectN / dt); ++i)
    {
        foutMM << TF[i][3] << endl;
    }

    return true;
}

void initialLayerTV(vector <vector <double> > &TV)
{
    size_t i = 0, j = 0;

    // + perturbation of the temperature, the gas flow, the pressure differential

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV[i][0] = initLayerTV_0;
        TV[i][selectZ-1] = initLayerTV_1;
    }

    // Initial values
    for(j = 1; j <= (selectZ-2); ++j)
    {
        TV[0][j] = initLayerTV[j-1];
    }
}

void initialLayerTF(vector <vector <double> > &TF)
{
    size_t i = 0, j = 0;

    // + perturbation of the temperature, the gas flow, the pressure differential

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TF[i][0] = initLayerTF_0;
        TF[i][selectZ-1] = initLayerTF_1;
    }

    // Initial values
    for(j = 1; j <= (selectZ-2); ++j)
    {
        TF[0][j] = initLayerTF[j-1];
    }
}

void initialLayerCV(vector <vector <double> > &CV)
{
    size_t i = 0, j = 0;

    // + perturbation of the temperature, the gas flow, the pressure differential

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        CV[i][0] = initLayerCV_0;
        CV[i][selectZ-1] = initLayerCV_1;
    }

    // Initial values
    for(j = 1; j <= (selectZ-2); ++j)
    {
        CV[0][j] = initLayerCV[j-1];
    }
}

void initialLayerCF(vector <vector <double> > &CF)
{
    size_t i = 0, j = 0;

    // + perturbation of the temperature, the gas flow, the pressure differential

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        CF[i][0] = initLayerCF_0;
        CF[i][selectZ-1] = initLayerCF_1;
    }

    // Initial values
    for(j = 1; j <= (selectZ-2); ++j)
    {
        CF[0][j] = initLayerCF[j-1];
    }
}

void initialLayerTB(vector <vector <double> > &TB)
{
    size_t i = 0, j = 0;

    // + perturbation of the temperature, the gas flow, the pressure differential

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TB[i][0] = initLayerTB_0;
        TB[i][selectZ-1] = initLayerTB_1;
    }

    // Initial values
    for(j = 1; j <= (selectZ-2); ++j)
    {
        TB[0][j] = initLayerTB[j-1];
    }
}

void initialLayerTFG(vector <vector <double> > &TFG)
{
    size_t i = 0, j = 0;

    // + perturbation of the temperature, the gas flow, the pressure differential

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TFG[i][0] = initLayerTFG_0;
        TFG[i][selectZ-1] = initLayerTFG_1;
    }

    // Initial values
    for(j = 1; j <= (selectZ-2); ++j)
    {
        TFG[0][j] = initLayerTFG[j-1];
    }
}

void toFileMM(vector <vector <double> > MMM, string nameModel)
{
    nameModel = "MM_" + nameModel + ".dat";

    ofstream foutMM(nameModel, ios_base::out | ios_base::trunc);

    for(size_t i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        for(uint j = 1; j < (selectZ-1); j++)
        {
            foutMM << MMM[i][j] << " ";
        }
        foutMM << endl;
    }
    foutMM.close();
}

void MainWindow::on_spaceParametr_valueChanged(int countSpacePoints)
{
    if((uiMain->tableBordersAndInitialConditions_1->columnCount()-1) <= countSpacePoints)
    {
        uiMain->tableBordersAndInitialConditions_1->insertColumn(uiMain->tableBordersAndInitialConditions_1->columnCount()-1);
        uiMain->tableBordersAndInitialConditions_2->insertColumn(uiMain->tableBordersAndInitialConditions_2->columnCount()-1);
        uiMain->tableBordersAndInitialConditions_3->insertColumn(uiMain->tableBordersAndInitialConditions_3->columnCount()-1);
        uiMain->tableBordersAndInitialConditions_4->insertColumn(uiMain->tableBordersAndInitialConditions_4->columnCount()-1);
    }
    else
    {
        uiMain->tableBordersAndInitialConditions_1->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_1->columnCount()-1);
        uiMain->tableBordersAndInitialConditions_1->removeColumn(uiMain->tableBordersAndInitialConditions_1->currentColumn()-1);

        uiMain->tableBordersAndInitialConditions_2->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_2->columnCount()-1);
        uiMain->tableBordersAndInitialConditions_2->removeColumn(uiMain->tableBordersAndInitialConditions_2->currentColumn()-1);

        uiMain->tableBordersAndInitialConditions_3->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_3->columnCount()-1);
        uiMain->tableBordersAndInitialConditions_3->removeColumn(uiMain->tableBordersAndInitialConditions_3->currentColumn()-1);

        uiMain->tableBordersAndInitialConditions_4->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_4->columnCount()-1);
        uiMain->tableBordersAndInitialConditions_4->removeColumn(uiMain->tableBordersAndInitialConditions_4->currentColumn()-1);
    }

    dh = dRC/((double)countSpacePoints);
    uiMain->selectStepH->setText(QString::number(dh));
}

void MainWindow::on_selectDRC_textChanged(QString dRCNew)
{
    dRC = dRCNew.toDouble(&okey);

    dh = dRC/((double)(selectZ-2));
    uiMain->selectStepH->setText(QString::number(dh));
}

void MainWindow::on_valuePetrubationTVM_textChanged(QString P_TV_New)
{
    P_TV = P_TV_New.toDouble(&okey);
}

void MainWindow::on_valuePetrubationTFM_textChanged(QString P_TF_New)
{
    P_TF = P_TF_New.toDouble(&okey);
}

void MainWindow::on_valuePetrubationCVM_textChanged(QString P_CV_New)
{
    P_CV = P_CV_New.toDouble(&okey);
}

void MainWindow::on_valuePetrubationCFM_textChanged(QString P_CF_New)
{
    P_CF = P_CF_New.toDouble(&okey);
}

void MainWindow::on_action3D_model_triggered()
{
    QUrl data = QUrl::fromLocalFile("A:/Scientifics_work/Ph.D/Qt_dir/3d/my_distil.stl");

    Qt3DExtras::Qt3DWindow view;

    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;
    Qt3DCore::QEntity *flyingwedge = new Qt3DCore::QEntity(rootEntity);

    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse(QColor(254, 254, 254));

    Qt3DRender::QMesh *flyingwedgeMesh = new Qt3DRender::QMesh;
    flyingwedgeMesh->setMeshName("DistilationColumn");
    flyingwedgeMesh->setSource(data);
    flyingwedge->addComponent(flyingwedgeMesh);
    flyingwedge->addComponent(material);

    Qt3DRender::QCamera *camera = view.camera();
    float fieldOfView = 1000.0f,
          aspectRatio = 1.0f,
          nearPlane = 0.1f,
          farPlane = 1000.0f;
    camera->lens()->setPerspectiveProjection(fieldOfView, aspectRatio, nearPlane, farPlane);

    camera->setPosition(QVector3D(50, -230, 100));
    camera->setViewCenter(QVector3D(50, 100, 100));

    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("green");
    light->setIntensity(1.1f);
    lightEntity->addComponent(light);

    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(QVector3D(60, 0, 40.0f));
    lightEntity->addComponent(lightTransform);

    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setCamera(camera);

    view.setRootEntity(rootEntity);
    view.show();

}

void MainWindow::on_actionCheck_stat_of_values_triggered()
{
    // If you have something to check
    if ( ( !(TV.empty()) ) || ( !(CV.empty()) ) || ( !(TB.empty() )) || ( !(TFG.empty()) ) )
    {
        CheckWindow *checkingWindow = new CheckWindow;

        checkingWindow->show();
        checkingWindow->setParentToCheck(this);
    }
    else
    {
        cout << "Nothing else matter..." << endl;
    }

}
