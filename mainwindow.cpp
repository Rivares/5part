#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <vector>
#include <cmath>
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
static double P_TB = 0.0;
static double P_TFG = 0.0;


void TMTPLMM(vector <vector <double> > &TV, vector <vector <double> > &TF);
void TMTPNMM(vector <vector <double> > &TV, vector <vector <double> > &TF);
void ETMBPMM(vector <vector <double> > &TV, vector <vector <double> > &TF,
             vector <vector <double> > &CV, vector <vector <double> > &CF);
void ETMTPMM(vector <vector <double> > &TV, vector <vector <double> > &TF,
             vector <vector <double> > &CV, vector <vector <double> > &CF);
void ACUMM(vector <vector <double> > &TV, vector <vector <double> > &TB);
void EVAP(vector <vector <double> > &TF, vector <vector <double> > &TB,
          vector <vector <double> > &TFG);

void initialLayerTV(vector <vector <double> > &TV);         // Would be relize as template
void initialLayerTF(vector <vector <double> > &TF);
void initialLayerCV(vector <vector <double> > &CV);
void initialLayerCF(vector <vector <double> > &CF);
void initialLayerTB(vector <vector <double> > &TB);
void initialLayerTFG(vector <vector <double> > &TFG);

void toFileMM(vector <vector <double> > MMM, string nameModel);

void Split(double a, int s, double& a_hi, double& a_lo);
double TwoSum(double a, double b, double& error);
double TwoProduct(double a, double b, double& err);



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
        clock_t timeMW_0 = clock();
        int msec = 0;

        ui->setupUi(this);

        ui->inputLeftX->setReadOnly(true);

        selectZ = static_cast <unsigned int> (ui->spaceParametr->value() + 2);
        selectN = ui->inputRightX->text().toULongLong(&okey, 10);
        dRC = ui->selectDRC->text().toDouble(&okey);

        dh = dRC/(selectZ-2);

        ui->inputLeftY->clear();
        ui->inputRightY->clear();

        ui->inputLeftY->insert("0");
        ui->inputRightY->insert("777");

        leftY = ui->inputLeftY->text().toDouble();
        rightY = ui->inputRightY->text().toDouble();

        getData();
        drawGraph();

        clock_t timeDiff = clock() - timeMW_0;
        msec = (double)(timeDiff * 1000) / (double)CLOCKS_PER_SEC;
        cout << "General time of program taken " << msec/1000 << " seconds, and " << msec%1000 <<" milliseconds!" << endl;
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getData()
{
    leftX = ui->inputLeftX->text().toDouble();
    rightX = ui->inputRightX->text().toDouble();
    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();
    selectN = ui->inputRightX->text().toULongLong(&okey, 10);

    if(ui->LVM_BP->isChecked())
    {
        initLayerTV_0 = ui->spinBoxInitLayer0_0->value(),      initLayerTV_1 = ui->spinBoxInitLayer0_1->value();
        initLayerTF_0 = ui->spinBoxInitLayer1_0->value(),      initLayerTF_1 = ui->spinBoxInitLayer1_1->value();
    }

    if(ui->NLVM_BP->isChecked())
    {
        initLayerTV_0 = ui->spinBoxInitLayer0_0->value(),      initLayerTV_1 = ui->spinBoxInitLayer0_1->value();
        initLayerTF_0 = ui->spinBoxInitLayer1_0->value(),      initLayerTF_1 = ui->spinBoxInitLayer1_1->value();
    }

    if( (ui->EVM_BP->isChecked()) | (ui->EFM_BP->isChecked()) )
    {
        initLayerTV_0 = ui->spinBoxInitLayer0_0->value(),      initLayerTV_1 = ui->spinBoxInitLayer0_1->value();
        initLayerTF_0 = ui->spinBoxInitLayer1_0->value(),      initLayerTF_1 = ui->spinBoxInitLayer1_1->value();
        initLayerCV_0 = ui->spinBoxInitLayer2_0->value(),      initLayerCV_1 = ui->spinBoxInitLayer2_1->value();
        initLayerCF_0 = ui->spinBoxInitLayer3_0->value(),      initLayerCF_1 = ui->spinBoxInitLayer3_1->value();
    }

    if( (ui->EVM_TP->isChecked()) | (ui->EFM_TP->isChecked()) )
    {
        initLayerTV_0 = ui->spinBoxInitLayer0_0->value(),      initLayerTV_1 = ui->spinBoxInitLayer0_1->value();
        initLayerTF_0 = ui->spinBoxInitLayer1_0->value(),      initLayerTF_1 = ui->spinBoxInitLayer1_1->value();
        initLayerCV_0 = ui->spinBoxInitLayer2_0->value(),      initLayerCV_1 = ui->spinBoxInitLayer2_1->value();
        initLayerCF_0 = ui->spinBoxInitLayer3_0->value(),      initLayerCF_1 = ui->spinBoxInitLayer3_1->value();
    }

    if(ui->ACU->isChecked())
    {
        initLayerTV_0 = ui->spinBoxInitLayer0_0->value(),      initLayerTV_1 = ui->spinBoxInitLayer0_1->value();
        initLayerTB_0 = ui->spinBoxInitLayer1_0->value(),      initLayerTB_1 = ui->spinBoxInitLayer1_1->value();
    }

    if(ui->EVAP->isChecked())
    {
        initLayerTF_0  = ui->spinBoxInitLayer0_0->value(),      initLayerTF_1  = ui->spinBoxInitLayer0_1->value();
        initLayerTB_0  = ui->spinBoxInitLayer1_0->value(),      initLayerTB_1  = ui->spinBoxInitLayer1_1->value();
        initLayerTFG_0 = ui->spinBoxInitLayer2_0->value(),      initLayerTFG_1 = ui->spinBoxInitLayer2_1->value();
    }

    dt = (ui->selectStepT->text().toDouble() <= 0.0)? 0.01 : abs(ui->selectStepT->text().toDouble());
    dh = dRC/(selectZ-2);
}

void MainWindow::drawGraph()
{
    clock_t timeMW_1 = clock();
    int msec = 0;

    ui->vaporState->clear();
    ui->fluidState->clear();

    // configure right and top axis to show ticks but no labels:
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->yAxis2->setTickLabels(false);

    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("t, sec");

    ui->customPlot->yAxis->setLabel("Магия, dB");

    if(ui->LVM_BP->isChecked() || ui->NLVM_BP->isChecked() || ui->EVM_BP->isChecked() || ui->EVM_TP->isChecked())
        ui->customPlot->yAxis->setLabel("Tемпература, 'C");

    if(ui->EFM_BP->isChecked() || ui->EFM_TP->isChecked())
        ui->customPlot->yAxis->setLabel("Концентрация, %");

    ui->customPlot->xAxis->setRange(0, (selectN / dt));
    ui->customPlot->yAxis->setRange(ui->inputLeftY->text().toDouble(), ui->inputRightY->text().toDouble());

    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    /*----------------------------------------------------------------------*/

    ui->statusBar->showMessage(QString("(!) Calculating the mathematical model... (!)"));

    if (ui->LVM_BP->isChecked())
    {
        TMTPLMM(TV, TF);
        ui->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));
        drawModel(0);

        ui->spinBoxInitLayer0_0->setValue(initLayerTV_0);
        ui->spinBoxInitLayer1_0->setValue(initLayerTF_0);
    }

    if (ui->NLVM_BP->isChecked())
    {
        TMTPNMM(TV, TF);
        ui->statusBar->showMessage(QString("(!) Drawing physical processes on the graph...... (!)"));
        drawModel(0);

        ui->spinBoxInitLayer0_0->setValue(initLayerTV_0);
        ui->spinBoxInitLayer1_0->setValue(initLayerTF_0);
    }

    if((ui->EVM_BP->isChecked()) || (ui->EFM_BP->isChecked()))
    {
        ETMBPMM(TV, TF, CV, CF);
        ui->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));

        if((ui->EVM_BP->isChecked()))
            drawModel(0);
        else
            drawModel(1);

        ui->spinBoxInitLayer0_0->setValue(initLayerTV_0);
        ui->spinBoxInitLayer1_0->setValue(initLayerTF_0);
        ui->spinBoxInitLayer2_0->setValue(initLayerCV_0);
        ui->spinBoxInitLayer3_0->setValue(initLayerCF_0);
    }

    if((ui->EVM_TP->isChecked()) || (ui->EFM_TP->isChecked()))
    {
        ETMTPMM(TV, TF, CV, CF);
        ui->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));

        if((ui->EVM_TP->isChecked()))
            drawModel(0);
        else
            drawModel(1);

        ui->spinBoxInitLayer0_0->setValue(initLayerTV_0);
        ui->spinBoxInitLayer1_0->setValue(initLayerTF_0);
        ui->spinBoxInitLayer2_0->setValue(initLayerCV_0);
        ui->spinBoxInitLayer3_0->setValue(initLayerCF_0);
    }

    if((ui->ACU->isChecked()))
    {
        ACUMM(TV, TB);
        ui->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));

        drawModel(2);

        ui->spinBoxInitLayer0_0->setValue(initLayerTV_0);
        ui->spinBoxInitLayer1_0->setValue(initLayerTB_0);
    }

    if((ui->EVAP->isChecked()))
    {
        EVAP(TF, TB, TFG);
        ui->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));

        drawModel(3);

        ui->spinBoxInitLayer0_0->setValue(initLayerTF_0);
        ui->spinBoxInitLayer1_0->setValue(initLayerTB_0);
        ui->spinBoxInitLayer2_0->setValue(initLayerTFG_0);
    }

    ui->statusBar->showMessage(QString("Ready!"));

    /*----------------------------------------------------------------------*/

    // Out to display steady-state value temperature
    QList <QString> listStatesVapor, listStatesFluid;
    if((ui->LVM_BP->isChecked()) || (ui->NLVM_BP->isChecked()) || (ui->EVM_BP->isChecked()) || (ui->EVM_TP->isChecked()))
    {
        for(uint j = 1; j < selectZ-1; ++j)
        {
            listStatesVapor.append(QString::number(TV[ static_cast <size_t> ((selectN-1) / dt) ][j]));
            listStatesFluid.append(QString::number(TF[ static_cast <size_t> ((selectN-1) / dt) ][j]));
        }
    }

    // Out to display steady-state concentration of absorbent
    if(ui->EFM_BP->isChecked() || ui->EFM_TP->isChecked())
    {
        for(uint j = 1; j < selectZ-1; ++j)
        {
            listStatesVapor.append(QString::number(CV[ static_cast <size_t> ((selectN-1) / dt) ][j]));
            listStatesFluid.append(QString::number(CF[ static_cast <size_t> ((selectN-1) / dt) ][j]));
        }
    }

    // Out to display steady-state value temperature (ACU)
    if(ui->ACU->isChecked())
    {
        for(uint j = 1; j < selectZ-1; ++j)
        {
            listStatesVapor.append(QString::number(TV[ static_cast <size_t> ((selectN-1) / dt) ][j]));
            listStatesFluid.append(QString::number(TB[ static_cast <size_t> ((selectN-1) / dt) ][j]));
        }
    }

    // Out to display steady-state value temperature (EVAP)
    if(ui->EVAP->isChecked())
    {
        for(uint j = 1; j < selectZ-1; ++j)
        {
            listStatesVapor.append(QString::number(TF[ static_cast <size_t> ((selectN-1) / dt) ][j])); // ?
            listStatesFluid.append(QString::number(TB[ static_cast <size_t> ((selectN-1) / dt) ][j]));
        }
    }
    QStringList vaporStates(listStatesVapor);
    QStringList fluidStates(listStatesFluid);

    ui->vaporState->addItems(vaporStates);
    ui->fluidState->addItems(fluidStates);

    clock_t timeDiff = clock() - timeMW_1;
    msec = (double)(timeDiff * 1000) / (double)CLOCKS_PER_SEC;
    cout << endl <<"Drawing time of program taken " << msec/1000 << " seconds, and " << msec%1000 <<" milliseconds!" << endl;
    return;
}

void MainWindow::on_exit_clicked()
{
    this->close();
}

void MainWindow::on_clear_clicked()
{
    ui->customPlot->clearGraphs();
    ui->customPlot->replot();
}

void MainWindow::on_draw_clicked()
{
    getData();
    drawGraph();
}

void MainWindow::drawModel(int choiceModel)
{
    /*-------------Prepare data to output to screen------------------*/

    QVector <double> t((selectN / dt));

    QVector <QVector <double>> M_mat0, M_mat1, M_mat2;

    // Convert from std::vector <std::vector <double>> to QVector <QVector <double>>
    for(uint i = 0; i < selectZ; ++i)
    {
        QVector <double> tmpVectorM0, tmpVectorM1, tmpVectorM2;

        for(uint j = 0; j < (selectN / dt); ++j)
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

    ui->customPlot->clearGraphs();
    ui->customPlot->legend->setVisible(true);

    srand(time(NULL));
    int randColorR, randColorG, randColorB;

    QPen pen;
    qreal widthPen = 2.2;
    int translucent = 35;

    uint counter = 1;
    for(uint j = 0; j < (selectZ-2) * countModels; ++j)
    {
        ui->customPlot->addGraph();

        randColorR = rand() % 255,
        randColorG = rand() % 255,
        randColorB = rand() % 255;

        pen.setColor(QColor(randColorR, randColorG, randColorB));

        if(counter <= (selectZ-2))
        {
            /*-------------Customize of pen to drawing first model------------------*/

            pen.setStyle(Qt::SolidLine);
            ui->customPlot->graph(j)->setName(QString(tr("Vapor phase_%1")).arg(j));
        }
        else
            if(counter <= (selectZ-2) * countModels)
            {
                /*-------------Customize of pen to drawing second model------------------*/

                pen.setStyle(Qt::DotLine);
                ui->customPlot->graph(j)->setName(QString(tr("Liquid phase_%1")).arg(j - (selectZ-2)));
            }

        pen.setWidthF(widthPen);
        ui->customPlot->graph(j)->setPen(pen);
        ui->customPlot->graph(j)->setBrush(QBrush(QColor (randColorR,
                                                           randColorG,
                                                           randColorB, translucent) ));
        ++counter;
    }

    /*-------------Drawing graphs------------------*/
/*
    // Test outputing data
    for(uint j = 1; j < selectZ-3; ++j)
    {
        for(uint i = 0; i < selectN; ++i)
        {
            cout << drawingProcces_0[j][i] << " , ";
        }
        cout << endl;
    }
*/

    clock_t timeMW_Cust = clock();
    int msec = 0;

    counter = 1;
    uint j = 1;
    for(uint i = 0; i < (selectZ-2) * 2; ++i)
    {
        if(counter <= (selectZ-2))
        {
            ui->customPlot->graph(i)->setData(t, drawingProcces_0[i+1]);
        }

        else
        {
            ui->customPlot->graph(i)->setData(t, drawingProcces_1[j]);
            ++j;
        }
        ++counter;
    }

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->customPlot->replot();

    clock_t timeDiff = clock() - timeMW_Cust;
    msec = (double)(timeDiff * 1000) / (double)CLOCKS_PER_SEC;
    cout << endl <<"(QCustomPlot) Drawing time of program taken " << msec/1000 << " seconds, and " << msec%1000 <<" milliseconds!" << endl;
}

void MainWindow::on_save_clicked()
{
    if(ui->LVM_BP->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, ref(TV), "TV_BP");
        std::thread threadToFileTFMM(toFileMM, ref(TF), "TF_BP");

        threadToFileTVMM.join();
        threadToFileTFMM.join();
    }

    if(ui->NLVM_BP->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, ref(TV), "NTV_BP");
        std::thread threadToFileTFMM(toFileMM, ref(TF), "NTF_BP");

        threadToFileTVMM.join();
        threadToFileTFMM.join();
    }

    if(ui->EVM_BP->isChecked() || ui->EFM_BP->isChecked())
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

    if(ui->EVM_TP->isChecked() || ui->EFM_TP->isChecked())
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

    if(ui->ACU->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, ref(TV), "TV_ACU");
        std::thread threadToFileTBMM(toFileMM, ref(TB), "TB_ACU");

        threadToFileTVMM.join();
        threadToFileTBMM.join();
    }

    if(ui->EVAP->isChecked())
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

    if(ui->customPlot->savePng(name + ".png", 0, 0, 1.0, -1))
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
    ui->valuePetrubationCVM->hide();
    ui->valuePetrubationCFM->hide();

    ui->inputLeftY->clear();
    ui->inputRightY->clear();

    ui->inputLeftY->insert("100");
    ui->inputRightY->insert("170");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();

    ui->selectDRC->setText(QString::number(1.400));
    ui->spaceParametr->setValue(3);

    ui->spinBoxInitLayer0_0->setValue(160.000);    ui->spinBoxInitLayer0_1->setValue(147.999);
    ui->spinBoxInitLayer1_0->setValue(120.377);    ui->spinBoxInitLayer1_1->setValue(132.399);
    ui->spinBoxInitLayer2_0->setValue(0.0);        ui->spinBoxInitLayer2_1->setValue(0.0);
    ui->spinBoxInitLayer3_0->setValue(0.0);        ui->spinBoxInitLayer3_1->setValue(0.0);

    ui->spinBoxInitLayer2_0->setDisabled(true);    ui->spinBoxInitLayer2_1->setDisabled(true);
    ui->spinBoxInitLayer3_0->setDisabled(true);    ui->spinBoxInitLayer3_1->setDisabled(true);
}

void MainWindow::on_NLVM_BP_clicked()
{
    ui->valuePetrubationCVM->hide();
    ui->valuePetrubationCFM->hide();

    ui->inputLeftY->clear();
    ui->inputRightY->clear();

    ui->inputLeftY->insert("100");
    ui->inputRightY->insert("170");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();

    ui->selectDRC->setText(QString::number(1.400));
    ui->spaceParametr->setValue(3);

    ui->spinBoxInitLayer0_0->setValue(160.000);    ui->spinBoxInitLayer0_1->setValue(147.999);
    ui->spinBoxInitLayer1_0->setValue(120.377);    ui->spinBoxInitLayer1_1->setValue(132.399);
    ui->spinBoxInitLayer2_0->setValue(0.0);        ui->spinBoxInitLayer2_1->setValue(0.0);
    ui->spinBoxInitLayer3_0->setValue(0.0);        ui->spinBoxInitLayer3_1->setValue(0.0);

    ui->spinBoxInitLayer2_0->setDisabled(true);    ui->spinBoxInitLayer2_1->setDisabled(true);
    ui->spinBoxInitLayer3_0->setDisabled(true);    ui->spinBoxInitLayer3_1->setDisabled(true);
}

void MainWindow::on_EVM_BP_clicked()
{
    ui->inputLeftY->clear();
    ui->inputRightY->clear();

    ui->valuePetrubationCVM->show();
    ui->valuePetrubationCFM->show();

    ui->inputLeftY->insert("100");
    ui->inputRightY->insert("170");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();

    ui->selectDRC->setText(QString::number(1.400));
    ui->spaceParametr->setValue(3);

    ui->spinBoxInitLayer0_0->setValue(160.000);    ui->spinBoxInitLayer0_1->setValue(147.999);
    ui->spinBoxInitLayer1_0->setValue(120.377);    ui->spinBoxInitLayer1_1->setValue(132.399);
    ui->spinBoxInitLayer2_0->setValue(67.9440);    ui->spinBoxInitLayer2_1->setValue(72.044);
    ui->spinBoxInitLayer3_0->setValue(6.550);      ui->spinBoxInitLayer3_1->setValue(2.788);

    ui->spinBoxInitLayer2_0->setDisabled(false);   ui->spinBoxInitLayer2_1->setDisabled(false);
    ui->spinBoxInitLayer3_0->setDisabled(false);   ui->spinBoxInitLayer3_1->setDisabled(false);
}

void MainWindow::on_EFM_BP_clicked()
{
    ui->inputLeftY->clear();
    ui->inputRightY->clear();

    ui->valuePetrubationCVM->show();
    ui->valuePetrubationCFM->show();

    ui->inputLeftY->insert("0");
    ui->inputRightY->insert("90");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();

    ui->selectDRC->setText(QString::number(1.400));
    ui->spaceParametr->setValue(3);

    ui->spinBoxInitLayer0_0->setValue(160.000);    ui->spinBoxInitLayer0_1->setValue(147.999);
    ui->spinBoxInitLayer1_0->setValue(120.377);    ui->spinBoxInitLayer1_1->setValue(132.399);
    ui->spinBoxInitLayer2_0->setValue(67.9440);    ui->spinBoxInitLayer2_1->setValue(72.044);
    ui->spinBoxInitLayer3_0->setValue(6.550);      ui->spinBoxInitLayer3_1->setValue(2.788);

    ui->spinBoxInitLayer2_0->setDisabled(false);   ui->spinBoxInitLayer2_1->setDisabled(false);
    ui->spinBoxInitLayer3_0->setDisabled(false);   ui->spinBoxInitLayer3_1->setDisabled(false);
}

void MainWindow::on_EVM_TP_clicked()
{
    ui->inputLeftY->clear();
    ui->inputRightY->clear();

    ui->valuePetrubationCVM->show();
    ui->valuePetrubationCFM->show();

    ui->inputLeftY->insert("20");
    ui->inputRightY->insert("150");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();

    ui->selectDRC->setText(QString::number(3.510));
    ui->spaceParametr->setValue(3);

    ui->spinBoxInitLayer0_0->setValue(142.500);    ui->spinBoxInitLayer0_1->setValue(45.300);
    ui->spinBoxInitLayer1_0->setValue(30.000);     ui->spinBoxInitLayer1_1->setValue(139.000);
    ui->spinBoxInitLayer2_0->setValue(0.5300);     ui->spinBoxInitLayer2_1->setValue(1.1380);
    ui->spinBoxInitLayer3_0->setValue(1.000);      ui->spinBoxInitLayer3_1->setValue(0.5525330);

    ui->spinBoxInitLayer2_0->setDisabled(false);   ui->spinBoxInitLayer2_1->setDisabled(false);
    ui->spinBoxInitLayer3_0->setDisabled(false);   ui->spinBoxInitLayer3_1->setDisabled(false);
}

void MainWindow::on_EFM_TP_clicked()
{
    ui->inputLeftY->clear();
    ui->inputRightY->clear();

    ui->valuePetrubationCVM->show();
    ui->valuePetrubationCFM->show();

    ui->inputLeftY->insert("0");
    ui->inputRightY->insert("1.5");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();

    ui->selectDRC->setText(QString::number(3.510));
    ui->spaceParametr->setValue(3);

    ui->spinBoxInitLayer0_0->setValue(142.500);    ui->spinBoxInitLayer0_1->setValue(45.300);
    ui->spinBoxInitLayer1_0->setValue(30.000);     ui->spinBoxInitLayer1_1->setValue(139.000);
    ui->spinBoxInitLayer2_0->setValue(0.5300);     ui->spinBoxInitLayer2_1->setValue(1.1380);
    ui->spinBoxInitLayer3_0->setValue(1.000);      ui->spinBoxInitLayer3_1->setValue(0.5525330);

    ui->spinBoxInitLayer2_0->setDisabled(false);   ui->spinBoxInitLayer2_1->setDisabled(false);
    ui->spinBoxInitLayer3_0->setDisabled(false);   ui->spinBoxInitLayer3_1->setDisabled(false);
}

void MainWindow::on_ACU_clicked()
{
    ui->inputLeftY->clear();
    ui->inputRightY->clear();
    ui->inputRightX->clear();

    ui->valuePetrubationCVM->show();
    ui->valuePetrubationCFM->show();

    ui->inputLeftY->insert("0");
    ui->inputRightY->insert("80");
    ui->inputRightX->insert("30");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();

    ui->selectDRC->setText(QString::number(4.0));
    ui->spaceParametr->setValue(3);

    ui->spinBoxInitLayer0_0->setValue(72.50);       ui->spinBoxInitLayer0_1->setValue(42.4940);
    ui->spinBoxInitLayer1_0->setValue(30.750);     ui->spinBoxInitLayer1_1->setValue(23.2897);
    ui->spinBoxInitLayer2_0->setValue(0.0);         ui->spinBoxInitLayer2_1->setValue(0.0);
    ui->spinBoxInitLayer3_0->setValue(0.0);         ui->spinBoxInitLayer3_1->setValue(0.0);

    ui->spinBoxInitLayer2_0->setDisabled(true);     ui->spinBoxInitLayer2_1->setDisabled(true);
    ui->spinBoxInitLayer3_0->setDisabled(true);     ui->spinBoxInitLayer3_1->setDisabled(true);

}

void MainWindow::on_EVAP_clicked()
{
    ui->inputLeftY->clear();
    ui->inputRightY->clear();
    ui->inputRightX->clear();

    ui->valuePetrubationCVM->show();
    ui->valuePetrubationCFM->show();

    ui->inputLeftY->insert("140");
    ui->inputRightY->insert("300");
    ui->inputRightX->insert("3000");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();

    ui->selectDRC->setText(QString::number(5.5));
    ui->spaceParametr->setValue(4);

    ui->spinBoxInitLayer0_0->setValue(160.1616);       ui->spinBoxInitLayer0_1->setValue(139.0);
    ui->spinBoxInitLayer1_0->setValue(147.0722);     ui->spinBoxInitLayer1_1->setValue(164.4825);
    ui->spinBoxInitLayer2_0->setValue(300.0);         ui->spinBoxInitLayer2_1->setValue(240.5403);
    ui->spinBoxInitLayer3_0->setValue(0.0);         ui->spinBoxInitLayer3_1->setValue(0.0);

    ui->spinBoxInitLayer2_0->setDisabled(false);   ui->spinBoxInitLayer2_1->setDisabled(false);
    ui->spinBoxInitLayer3_0->setDisabled(true);   ui->spinBoxInitLayer3_1->setDisabled(true);
}

//--------------------------LINER MODEL-----------------------------------
void TMTPLMM(vector <vector <double> > &TV, vector <vector <double> > &TF)
{
    double  RvT = 0.0191806, RfT = 0.0000777,
            PTV = (0.05654433 * dt) / dh,
            PTF = (0.0002291314 * dt) / dh;

    vector <double> bmp;
    bmp.assign(selectZ, 0.0);

    for (unsigned long long i = 0; i < size_t(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TF.erase(TF.begin(), TF.end());
    }

    for (unsigned long long i = 0; i < size_t(selectN / dt); ++i)
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
    for(size_t i = 1; i < size_t(selectN / dt); ++i)// time
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
        cout << TV[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TF[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;
}

//--------------------------NON-LINER MODEL-----------------------------------
void TMTPNMM(vector <vector <double> > &TV, vector <vector <double> > &TF)
{
    //double a0 = 0.0001152735759 or 0.00016966, What is TRUE?????????

    // -----Model's heat exchenger parameters------
    double  RvT = 0.0191806, RfT = 0.0000777, a0 = 1.6966E-4,
            PTV_L = (a0 * 273.15 * dt) / dh,
            PTV_N = 0.0,
            PTF = (0.0002291314 * dt) / dh;

    vector <double> bmp;
    bmp.assign(selectZ, 0.0);

    for (unsigned long long i = 0; i < size_t(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TF.erase(TF.begin(), TF.end());
    }

    for (unsigned long long i = 0; i < size_t(selectN / dt); ++i)
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
    for(size_t i = 1; i < size_t(selectN / dt); ++i)
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
        cout << TV[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TF[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;
}

//---------------------------INTERCONNECTED MODEL(BOTTOM PART)------------------------------
void ETMBPMM(vector <vector <double> > &TV, vector <vector <double> > &TF,
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

    vector <double> bmp;
    bmp.assign(selectZ, 0.0);

    for (unsigned long long i = 0; i < size_t(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TF.erase(TF.begin(), TF.end());
        CV.erase(CV.begin(), CV.end());
        CF.erase(CF.begin(), CF.end());
    }

    for (unsigned long long i = 0; i < size_t(selectN / dt); ++i)
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
    for(size_t i = 1; i < size_t(selectN / dt); ++i)// time
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
            CV[i][j] = - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                    + (PTV_L + PTV_N) * CV[i-1][j-1]
                    - CV[i-1][j] * (- dt*RvM + PTV_L + PTV_N)
                    + CV[i-1][j];

            CF[i][j] = - (dt * RfM * CV[i-1][(selectZ-1)-j])
                    + (PTF * CF[i-1][j-1])
                    - CF[i-1][j] * (-(dt*RfM*E) + PTF)
                    + CF[i-1][j];
        }

    }

    cout << endl << "Steady-state values:" << endl;
    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TV[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TF[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << CV[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << CF[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;
}

//---------------------------INTERCONNECTED MODEL(TOP PART)------------------------------
void ETMTPMM(vector <vector <double> > &TV, vector <vector <double> > &TF,
             vector <vector <double> > &CV, vector <vector <double> > &CF)
{
    // -----Model's heat exchenger parameters------
    double  RvT = 2.500, RfT = 0.000085500, a0 = 0.0034868520, // (a0 = 0.0034868520) != (a0_Simulink = 0.002702752)
            PTV_L = (a0 * 273.15 * dt) / dh,
            PTV_N = 0.0,
            PTF = (0.0000400 * dt) / dh;

    // -----Model's mass exchenger parameters------
    double E = 1.0E-9, RvM = 0.1450, RfM = 6.0E-6;

    vector <double> bmp;
    bmp.assign(selectZ, 0.0);

    for (unsigned long long i = 0; i < size_t(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TF.erase(TF.begin(), TF.end());
        CV.erase(CV.begin(), CV.end());
        CF.erase(CF.begin(), CF.end());
    }

    for (unsigned long long i = 0; i < size_t(selectN / dt); ++i)
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
    for(size_t i = 1; i < size_t(selectN / dt); ++i)// time
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
            CV[i][j] = - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                    + (PTV_L + PTV_N) * CV[i-1][j-1]
                    - CV[i-1][j] * (- dt*RvM + PTV_L + PTV_N)
                    + CV[i-1][j];

            CF[i][j] = - (dt * RfM * CV[i-1][(selectZ-1)-j])
                    + (PTF * CF[i-1][j-1])
                    - CF[i-1][j] * (-(dt*RfM*E) + PTF)
                    + CF[i-1][j];

            /* Old schema: CV(i,i-1); CV(i+1,i)

            CV[i][j] = -CV[i-1][j] * (PTV_L - 1 - PTV_N - dt*RvM) + (PTV_L * CV[i-1][j-1]) - (PTV_N * CV[i-1][j+1])
                                                                                        - (dt * RvM * E * CF[i-1][(selectZ-1)-j]) + P_CV;*/

        }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TV[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TF[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << CV[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << CF[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;
}

//---------------------------AIR-COOLING UNIT MODEL(ACU)------------------------------
void ACUMM(vector<vector<double> > &TV, vector<vector<double> > &TB)
{
    // -----Model's heat parameters------
    double  RvT = 8.400, a0 = 0.06,     // a0 = 0.06 ?
            PTV_L = (a0 * 273.150) / dh;

    // -----Model's boarder parameters------
    double RB1 = 0.00661, RB2 = 0.480   // Boarders
         , TE = 12.380                  // Temperature of enviroment
         , CP = 134.0;                  // Count piplines;

    vector <double> bmp;
    bmp.assign(selectZ, 0.0);

    for (unsigned long long i = 0; i < size_t(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TB.erase(TB.begin(), TB.end());
    }

    for (unsigned long long i = 0; i < size_t(selectN / dt); ++i)
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
    for(size_t i = 1; i < size_t(selectN / dt); ++i)    // time
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
                    - TB[i-1][j] * (CP*dt*RB1 + dt*RB2)    // ? not optimization, yet
                    + TB[i-1][j];
        }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TV[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TB[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;
}

void EVAP(vector <vector <double> > &TF, vector <vector <double> > &TB, vector <vector <double> > &TFG)
{
    // -----Model's gas parameters------
    double  RFG = 0.470, PTFG = 4.60 / dh;

    // -----Model's fluide parameters------
    double  RF = 0.0030, PTF = 0.0030 / dh;

    // -----Model's boarder parameters------
    double  RFB = 0.220, RFGB = 0.0080;

    vector <double> bmp;
    bmp.assign(selectZ, 0.0);

    for (unsigned long long i = 0; i < size_t(selectN / dt); ++i)
    {
        TF.erase(TF.begin(), TF.end());
        TB.erase(TB.begin(), TB.end());
        TFG.erase(TFG.begin(), TFG.end());
    }

    for (unsigned long long i = 0; i < size_t(selectN / dt); ++i)
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
    for(size_t i = 1; i < size_t(selectN / dt); ++i)    // time
    {
        for(size_t j = 1; j < (selectZ-1); ++j)         // place
        {
            TF[i][j] = (dt * RF * TB[i-1][j+1]) // ?
                    + (dt * PTF * TF[i-1][j+1])
                    - TF[i-1][j] * (dt*RF + (dt*PTF))
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
        cout << TF[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TB[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;

    for(uint j = 1; j < (selectZ-1); ++j)
    {
        cout << TFG[size_t((selectN-1) / dt)][j] << " | ";
    }cout << endl;
}

void initialLayerTV(vector <vector <double> > &TV)
{
    double *initLayerTV = new double [selectZ];
    double stepInit = 0.0;
    size_t i = 0;

    initLayerTV_0 += P_TV;  // + perturbation of the temperature, the gas flow, the pressure differential

    for(i = 0; i < size_t(selectN / dt); ++i)
    {
        TV[i][0] = initLayerTV_0;
        TV[i][selectZ-1] = initLayerTV_1;
    }

    stepInit = abs(initLayerTV_0 - initLayerTV_1)/(selectZ-1);

    initLayerTV[0] = initLayerTV_0;
    initLayerTV[selectZ-1] = initLayerTV_1;

    for(i = 1; i < selectZ; ++i)
    {
        initLayerTV[i] = initLayerTV[i-1] - stepInit;
        TV[0][i-1] = initLayerTV[i-1];
    }

    delete [] initLayerTV;
}

void initialLayerTF(vector <vector <double> > &TF)
{
    double *initLayerTF = new double [selectZ];
    double stepInit = 0.0;
    size_t i = 0;

    initLayerTF_0 += P_TF;  // adding perturbation of the temperature

    for(i = 0; i < size_t(selectN / dt); ++i)
    {
        TF[i][0] = initLayerTF_0;
        TF[i][selectZ-1] = initLayerTF_1;
    }

    stepInit = abs(initLayerTF_0 - initLayerTF_1)/(selectZ-1);

    initLayerTF[0] = initLayerTF_0;
    initLayerTF[selectZ-1] = initLayerTF_1;

    for(i = 1; i < selectZ; ++i)
    {
        initLayerTF[i] = initLayerTF[i-1] + stepInit;
        TF[0][i-1] = initLayerTF[i-1];
    }

    delete [] initLayerTF;
}

void initialLayerCV(vector <vector <double> > &CV)
{
    double *initLayerCV = new double [selectZ];
    double stepInit = 0.0;
    size_t i = 0;

    initLayerCV_0 += P_CV;  // adding perturbation of the concentration

    for(i = 0; i < size_t(selectN / dt); ++i)
    {
        CV[i][0] = initLayerCV_0;
        CV[i][selectZ-1] = initLayerCV_1;
    }

    stepInit = abs(initLayerCV_0-initLayerCV_1)/(selectZ-1);

    initLayerCV[0] = initLayerCV_0;
    initLayerCV[selectZ-1] = initLayerCV_1;

    for(i = 1; i < selectZ; ++i)
    {
        initLayerCV[i] = initLayerCV[i-1] + stepInit;
        CV[0][i-1] = initLayerCV[i-1];
    }

    delete [] initLayerCV;
}

void initialLayerCF(vector <vector <double> > &CF)
{
    double *initLayerCF = new double [selectZ];
    double stepInit = 0.0;
    size_t i = 0;

    initLayerCF_0 += P_CF;  // adding perturbation of the concentration

    for(i = 0; i < size_t(selectN / dt); ++i)
    {
        CF[i][0] = initLayerCF_0;
        CF[i][selectZ-1] = initLayerCF_1;
    }

    stepInit = abs(initLayerCF_0-initLayerCF_1)/(selectZ-1);

    initLayerCF[0] = initLayerCF_0;
    initLayerCF[selectZ-1] = initLayerCF_1;

    for(i = 1; i < selectZ; ++i)
    {
        initLayerCF[i] = initLayerCF[i-1] - stepInit;
        CF[0][i-1] = initLayerCF[i-1];
    }

    delete [] initLayerCF;
}

void initialLayerTB(vector <vector <double> > &TB)
{
    double *initLayerTB = new double [selectZ];
    double stepInit = 0.0;
    size_t i = 0;

    initLayerTB_0 += P_TB;  // + perturbation of the temperature, the gas flow, the pressure differential

    for(i = 0; i < size_t(selectN / dt); ++i)
    {
        TB[i][0] = initLayerTB_0;
        TB[i][selectZ-1] = initLayerTB_1;
    }

    stepInit = abs(initLayerTB_0 - initLayerTB_1)/(selectZ-1);

    initLayerTB[0] = initLayerTB_0;
    initLayerTB[selectZ-1] = initLayerTB_1;

    for(i = 1; i < selectZ; ++i)
    {
        initLayerTB[i] = initLayerTB[i-1] - stepInit;
        TB[0][i-1] = initLayerTB[i-1];
    }

    delete [] initLayerTB;
}

void initialLayerTFG(vector <vector <double> > &TFG)
{
    double *initLayerTFG = new double [selectZ];
    double stepInit = 0.0;
    size_t i = 0;

    initLayerTFG_0 += P_TFG;  // + perturbation of the temperature, the gas flow, the pressure differential

    for(i = 0; i < size_t(selectN / dt); ++i)
    {
        TFG[i][0] = initLayerTFG_0;
        TFG[i][selectZ-1] = initLayerTFG_1;
    }

    stepInit = abs(initLayerTFG_0 - initLayerTFG_1)/(selectZ-1);

    initLayerTFG[0] = initLayerTFG_0;
    initLayerTFG[selectZ-1] = initLayerTFG_1;

    for(i = 1; i < selectZ; ++i)
    {
        initLayerTFG[i] = initLayerTFG[i-1] - stepInit;
        TFG[0][i-1] = initLayerTFG[i-1];
    }

    delete [] initLayerTFG;
}

void toFileMM(vector <vector <double> > MMM, string nameModel)
{
    nameModel = "MM_" + nameModel + ".txt";

    ofstream foutMM(nameModel, ios_base::out | ios_base::trunc);

    for(size_t i = 0; i < (selectN / dt); ++i)
    {
        for(uint j = 0; j < selectZ; j++)
        {
            foutMM << MMM[i][j] << " | ";
        }
        foutMM << endl;
    }
    foutMM.close();
}

void Split(double a, int s, double& a_hi, double& a_lo)
{
    double c = (pow(2, s) + 1)*a;
    double a_big = c - a;

    a_hi = c - a_big;
    a_lo = a - a_hi;
}

double TwoSum(double a, double b, double& error)
{
    double x = a + b;
    double b_virt = x - a;
    double a_virt = x - b_virt;
    double b_roundoff = b - b_virt;
    double a_roudnoff = a - a_virt;
    double y = a_roudnoff + b_roundoff;

    error += y;

    return x;
}

double TwoProduct(double a, double b, double& err)
{
    double x = a*b;
    double a_hi, a_low, b_hi, b_low;

    Split(a, 12, a_hi, a_low);
    Split(b, 12, b_hi, b_low);

    double err1, err2, err3;

    err1 = x - (a_hi*b_hi);
    err2 = err1 - (a_low*b_hi);
    err3 = err2 - (a_hi*b_low);

    err += ((a_low * b_low) - err3);

    return x;
}

void MainWindow::on_spaceParametr_valueChanged(int countSpacePoints)
{
    selectZ = static_cast <unsigned int> (countSpacePoints + 2);

    dh = dRC/((double)countSpacePoints);
    ui->selectStepH->setText(QString::number(dh));
}

void MainWindow::on_selectDRC_textChanged(QString dRCNew)
{
    dRC = dRCNew.toDouble(&okey);

    dh = dRC/((double)(selectZ-2));
    ui->selectStepH->setText(QString::number(dh));
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
