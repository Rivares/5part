#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include "checkwindow.h"
#include "mainwindow.h"
#include "models.h"

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

using std::vector;
using std::cout;
using std::endl;

static bool okey = false;

static double dRC = 0.0;    // Hieght nozzles
static double dh = 0.0000;
static double dt = 0.0000;
static unsigned int beginPoint = 0;
static unsigned int endPoint = 0;
static unsigned int spaceParametrBP = 0;
static unsigned int spaceParametrTP = 0;
static unsigned int spaceParametrACU = 0;
static unsigned int spaceParametrEVAP = 0;
static unsigned long long selectN = 0;


static vector <double> initLayerTV;
static vector <double> initLayerTF;
static vector <double> initLayerCV;
static vector <double> initLayerCF;
static vector <double> initLayerTB;
static vector <double> initLayerTFG;

//---------------Borders--------------//
static vector <double> initLayerTV_0, initLayerTV_1;
static vector <double>  initLayerTF_0, initLayerTF_1;
static vector <double>  initLayerCV_0, initLayerCV_1;
static vector <double>  initLayerCF_0, initLayerCF_1;
static double initLayerTB_0 = 0.0, initLayerTB_1 = 0.0;
static double initLayerTFG_0 = 0.0, initLayerTFG_1 = 0.0;


bool TMTPL_MM(vector <vector <double> > &TV, vector <vector <double> > &TF);
bool TMTPN_MM(vector <vector <double> > &TV, vector <vector <double> > &TF);
bool ETMBP_MM(vector <vector <double> > &TV, vector <vector <double> > &TF,
             vector <vector <double> > &CV, vector <vector <double> > &CF);
bool ETMTP_MM(vector <vector <double> > &TV, vector <vector <double> > &TF,
             vector <vector <double> > &CV, vector <vector <double> > &CF);
bool ACU_MM(vector <vector <double> > &TV, vector <vector <double> > &TB);
bool EVAP_MM(vector <vector <double> > &TF, vector <vector <double> > &TB,
          vector <vector <double> > &TFG);

bool TOP_ACU_MM(vector <vector <double> > &TV, vector <vector <double> > &TF,
             vector <vector <double> > &CV, vector <vector <double> > &CF,
             vector<vector<double> > &TB);


void initialLayerTV(vector <vector <double> > &TV, unsigned int it);         // Would be relize as template
void initialLayerTF(vector <vector <double> > &TF, unsigned int it);
void initialLayerCV(vector <vector <double> > &CV, unsigned int it);
void initialLayerCF(vector <vector <double> > &CF, unsigned int it);
void initialLayerTB(vector <vector <double> > &TB, unsigned int it);
void initialLayerTFG(vector <vector <double> > &TFG);

void toFileMM(vector <vector <double> > MMM, string nameModel);



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), uiMain(new Ui::MainWindow)
{
        uiMain->setupUi(this);

        uiMain->inputLeftX->setReadOnly(true);

        selectN = 1000;
        dRC = 0.0;
        dh = 0.0;

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

    if(uiMain->LVM_BP->isChecked() || uiMain->NLVM_BP->isChecked())
    {
        spaceParametrBP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_BP_1->columnCount());

        initLayerTV_0.assign(1, 0.0); initLayerTV_1.assign(1, 0.0);
        initLayerTF_0.assign(1, 0.0); initLayerTF_1.assign(1, 0.0);

        initLayerTV_0.at(0) = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, 0)->text()).toDouble();
        initLayerTV_1.at(0) = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1)->text()).toDouble();

        initLayerTF_0.at(0) = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, 0)->text()).toDouble();
        initLayerTF_1.at(0) = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1)->text()).toDouble();

        initLayerTV.assign((spaceParametrBP-2), 0.0);
        initLayerTF.assign((spaceParametrBP-2), 0.0);

        for(uint j = 1; j < (spaceParametrBP-1); ++j)
        {
            initLayerTV.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerTF.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, static_cast <int>(j))->text()).toDouble();
        }

        dh = static_cast <double> (dRC / (spaceParametrBP-2));
        qDebug() << "dh = " <<  dh;
    }

    if( uiMain->EVM_BP->isChecked() || uiMain->EFM_BP->isChecked() )
    {
        spaceParametrBP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_BP_1->columnCount());

        initLayerTV_0.assign(1, 0.0); initLayerTV_1.assign(1, 0.0);
        initLayerTF_0.assign(1, 0.0); initLayerTF_1.assign(1, 0.0);
        initLayerCV_0.assign(1, 0.0); initLayerCV_1.assign(1, 0.0);
        initLayerCF_0.assign(1, 0.0); initLayerCF_1.assign(1, 0.0);

        initLayerTV_0.at(0) = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, 0)->text()).toDouble();
        initLayerTV_1.at(0) = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1)->text()).toDouble();

        initLayerTF_0.at(0) = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, 0)->text()).toDouble();
        initLayerTF_1.at(0) = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1)->text()).toDouble();

        initLayerCV_0.at(0) = (uiMain->tableBordersAndInitialConditions_BP_3->item(0, 0)->text()).toDouble();
        initLayerCV_1.at(0) = (uiMain->tableBordersAndInitialConditions_BP_3->item(0, uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1)->text()).toDouble();

        initLayerCF_0.at(0) = (uiMain->tableBordersAndInitialConditions_BP_4->item(0, 0)->text()).toDouble();
        initLayerCF_1.at(0) = (uiMain->tableBordersAndInitialConditions_BP_4->item(0, uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1)->text()).toDouble();

        initLayerTV.assign((spaceParametrBP-2), 0.0);
        initLayerTF.assign((spaceParametrBP-2), 0.0);
        initLayerCV.assign((spaceParametrBP-2), 0.0);
        initLayerCF.assign((spaceParametrBP-2), 0.0);

        for(uint j = 1; j <= (spaceParametrBP-2); ++j)
        {
            initLayerTV.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerTF.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerCV.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_3->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerCF.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_4->item(0, static_cast <int>(j))->text()).toDouble();
        }

        dh = static_cast <double> (dRC / (spaceParametrBP-2));
    }

    if( uiMain->EVM_TP->isChecked() || uiMain->EFM_TP->isChecked() )
    {
        spaceParametrTP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_TP_1->columnCount());

        initLayerTV_0.assign(1, 0.0); initLayerTV_1.assign(1, 0.0);
        initLayerTF_0.assign(1, 0.0); initLayerTF_1.assign(1, 0.0);
        initLayerCV_0.assign(1, 0.0); initLayerCV_1.assign(1, 0.0);
        initLayerCF_0.assign(1, 0.0); initLayerCF_1.assign(1, 0.0);

        initLayerTV_0.at(0) = (uiMain->tableBordersAndInitialConditions_TP_1->item(0, 0)->text()).toDouble();
        initLayerTV_1.at(0) = (uiMain->tableBordersAndInitialConditions_TP_1->item(0, uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1)->text()).toDouble();

        initLayerTF_0.at(0) = (uiMain->tableBordersAndInitialConditions_TP_2->item(0, 0)->text()).toDouble();
        initLayerTF_1.at(0) = (uiMain->tableBordersAndInitialConditions_TP_2->item(0, uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1)->text()).toDouble();

        initLayerCV_0.at(0) = (uiMain->tableBordersAndInitialConditions_TP_3->item(0, 0)->text()).toDouble();
        initLayerCV_1.at(0) = (uiMain->tableBordersAndInitialConditions_TP_3->item(0, uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1)->text()).toDouble();

        initLayerCF_0.at(0) = (uiMain->tableBordersAndInitialConditions_TP_4->item(0, 0)->text()).toDouble();
        initLayerCF_1.at(0) = (uiMain->tableBordersAndInitialConditions_TP_4->item(0, uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1)->text()).toDouble();

        initLayerTV.assign((spaceParametrTP-2), 0.0);
        initLayerTF.assign((spaceParametrTP-2), 0.0);
        initLayerCV.assign((spaceParametrTP-2), 0.0);
        initLayerCF.assign((spaceParametrTP-2), 0.0);

        for(uint j = 1; j <= (spaceParametrTP-2); ++j)
        {
            initLayerTV.at(j-1) = (uiMain->tableBordersAndInitialConditions_TP_1->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerTF.at(j-1) = (uiMain->tableBordersAndInitialConditions_TP_2->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerCV.at(j-1) = (uiMain->tableBordersAndInitialConditions_TP_3->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerCF.at(j-1) = (uiMain->tableBordersAndInitialConditions_TP_4->item(0, static_cast <int>(j))->text()).toDouble();
        }

        dh = static_cast <double> (dRC/spaceParametrTP);
    }

    if(uiMain->ACU->isChecked())
    {
        spaceParametrACU = static_cast <uint>(uiMain->tableBordersAndInitialConditions_ACU_1->columnCount());

        initLayerTV_0.assign(1, 0.0);
        initLayerTV_1.assign(1, 0.0);

        initLayerTV_0.at(0) = (uiMain->tableBordersAndInitialConditions_ACU_1->item(0, 0)->text()).toDouble();
        initLayerTV_1.at(0) = (uiMain->tableBordersAndInitialConditions_ACU_1->item(0, uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1)->text()).toDouble();

        initLayerTB_0 = (uiMain->tableBordersAndInitialConditions_ACU_2->item(0, 0)->text()).toDouble();
        initLayerTB_1 = (uiMain->tableBordersAndInitialConditions_ACU_2->item(0, uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1)->text()).toDouble();

        initLayerTV.assign((spaceParametrACU-2), 0.0);
        initLayerTB.assign((spaceParametrACU-2), 0.0);

        for(uint j = 1; j <= (spaceParametrACU-2); ++j)
        {
            initLayerTV.at(j-1) = (uiMain->tableBordersAndInitialConditions_ACU_1->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerTB.at(j-1) = (uiMain->tableBordersAndInitialConditions_ACU_2->item(0, static_cast <int>(j))->text()).toDouble();
        }

        dh = static_cast <double> (dRC / (spaceParametrACU-2));
    }

    if(uiMain->EVAP->isChecked())
    {
        spaceParametrEVAP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount());

        initLayerTF_0.assign(1, 0.0);
        initLayerTF_1.assign(1, 0.0);

        initLayerTF_0.at(0) = (uiMain->tableBordersAndInitialConditions_EVAP_1->item(0, 0)->text()).toDouble();
        initLayerTF_1.at(0) = (uiMain->tableBordersAndInitialConditions_EVAP_1->item(0, uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1)->text()).toDouble();

        initLayerTB_0 = (uiMain->tableBordersAndInitialConditions_EVAP_2->item(0, 0)->text()).toDouble();
        initLayerTB_1 = (uiMain->tableBordersAndInitialConditions_EVAP_2->item(0, uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1)->text()).toDouble();

        initLayerTFG_0 = (uiMain->tableBordersAndInitialConditions_EVAP_3->item(0, 0)->text()).toDouble();
        initLayerTFG_1 = (uiMain->tableBordersAndInitialConditions_EVAP_3->item(0, uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1)->text()).toDouble();

        initLayerTF.assign((spaceParametrEVAP-2), 0.0);
        initLayerTB.assign((spaceParametrEVAP-2), 0.0);
        initLayerTFG.assign((spaceParametrEVAP-2), 0.0);

        for(uint j = 1; j <= (spaceParametrEVAP-2); ++j)
        {
            initLayerTF.at(j-1)  = (uiMain->tableBordersAndInitialConditions_EVAP_1->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerTB.at(j-1)  = (uiMain->tableBordersAndInitialConditions_EVAP_2->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerTFG.at(j-1) = (uiMain->tableBordersAndInitialConditions_EVAP_3->item(0, static_cast <int>(j))->text()).toDouble();
        }

        dh = static_cast <double> (dRC / (spaceParametrEVAP-2));
    }

    if( (uiMain->TP_ACU->isChecked()) )
    {
        spaceParametrTP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_TP_1->columnCount());
        spaceParametrACU = static_cast <uint>(uiMain->tableBordersAndInitialConditions_ACU_1->columnCount());

        initLayerTV_0.assign(1, 0.0); initLayerTV_1.assign(1, 0.0);
        initLayerTF_0.assign(1, 0.0); initLayerTF_1.assign(1, 0.0);
        initLayerCV_0.assign(1, 0.0); initLayerCV_1.assign(1, 0.0);
        initLayerCF_0.assign(1, 0.0); initLayerCF_1.assign(1, 0.0);

        initLayerTV_0.at(0) = (uiMain->tableBordersAndInitialConditions_TP_1->item(0, 0)->text()).toDouble();

        initLayerTF_0.at(0) = (uiMain->tableBordersAndInitialConditions_TP_2->item(0, 0)->text()).toDouble();
        initLayerTF_1.at(0) = (uiMain->tableBordersAndInitialConditions_TP_2->item(0, uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1)->text()).toDouble();

        initLayerCV_0.at(0) = (uiMain->tableBordersAndInitialConditions_TP_3->item(0, 0)->text()).toDouble();
        initLayerCV_1.at(0) = (uiMain->tableBordersAndInitialConditions_TP_3->item(0, uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1)->text()).toDouble();

        initLayerCF_0.at(0) = (uiMain->tableBordersAndInitialConditions_TP_4->item(0, 0)->text()).toDouble();
        initLayerCF_1.at(0) = (uiMain->tableBordersAndInitialConditions_TP_4->item(0, uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1)->text()).toDouble();

        initLayerTV_1.at(0) = (uiMain->tableBordersAndInitialConditions_ACU_1->item(0, uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1)->text()).toDouble();

        initLayerTB_0 = (uiMain->tableBordersAndInitialConditions_ACU_2->item(0, 0)->text()).toDouble();
        initLayerTB_1 = (uiMain->tableBordersAndInitialConditions_ACU_2->item(0, uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1)->text()).toDouble();


        initLayerTV.assign((spaceParametrTP + spaceParametrACU - 4), 0.0);
        initLayerTF.assign((spaceParametrTP-2), 0.0);
        initLayerCV.assign((spaceParametrTP-2), 0.0);
        initLayerCF.assign((spaceParametrTP-2), 0.0);

        initLayerTB.assign((spaceParametrACU-2), 0.0);

        for(uint j = 1; j <= (spaceParametrTP-2); ++j)
        {
            initLayerTV.at(j-1) = (uiMain->tableBordersAndInitialConditions_TP_1->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerTF.at(j-1) = (uiMain->tableBordersAndInitialConditions_TP_2->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerCV.at(j-1) = (uiMain->tableBordersAndInitialConditions_TP_3->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerCF.at(j-1) = (uiMain->tableBordersAndInitialConditions_TP_4->item(0, static_cast <int>(j))->text()).toDouble();
        }

        for(uint j = (spaceParametrTP-2); j < (spaceParametrTP + spaceParametrACU - 4); ++j)
        {
            initLayerTV.at(j) = (uiMain->tableBordersAndInitialConditions_ACU_1->item(0, static_cast <int>(j-(spaceParametrTP-2)+1))->text()).toDouble();
        }

        for(uint j = 1; j <= (spaceParametrACU-2); ++j)
        {
            initLayerTB.at(j-1) = (uiMain->tableBordersAndInitialConditions_ACU_2->item(0, static_cast <int>(j))->text()).toDouble();
        }

        dh = static_cast <double> (dRC / (spaceParametrTP + spaceParametrACU - 4));  // dRC = dRC_TP + dRC_ACU; spaceParametr = spaceParametrTP + spaceParametrACU
    }

    dt = (uiMain->selectStepT->text().toDouble() <= 0.0)? 0.01 : abs(uiMain->selectStepT->text().toDouble());
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
        if(!TMTPL_MM(TV, TF))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            uiMain->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));

            drawModel(0);

            for(uint j = 1; j < spaceParametrBP-1; ++j)
            {
                listStatesFirst.append(QString::number(TV.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
                listStatesSecond.append(QString::number(TF.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
            }
        }
    }

    if (uiMain->NLVM_BP->isChecked())
    {       
        if(!TMTPN_MM(TV, TF))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            uiMain->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));

            drawModel(1);

            for(uint j = 1; j < spaceParametrBP-1; ++j)
            {
                listStatesFirst.append(QString::number(TV.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
                listStatesSecond.append(QString::number(TF.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
            }
        }
    }

    if((uiMain->EVM_BP->isChecked()) || (uiMain->EFM_BP->isChecked()))
    {       
        if(!ETMBP_MM(TV, TF, CV, CF))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            if((uiMain->EVM_BP->isChecked()))
            {
                drawModel(2);

                for(uint j = 1; j < spaceParametrBP-1; ++j)
                {
                    listStatesFirst.append(QString::number(TV.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
                    listStatesSecond.append(QString::number(TF.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
                }
            }
            else
            {
                drawModel(3);

                // Out to display steady-state concentration of absorbent
                for(uint j = 1; j < spaceParametrBP-1; ++j)
                {
                    listStatesFirst.append(QString::number(CV.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
                    listStatesSecond.append(QString::number(CF.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
                }
            }

            uiMain->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));
        }
    }

    if((uiMain->EVM_TP->isChecked()) || (uiMain->EFM_TP->isChecked()))
    {       
        if(!ETMTP_MM(TV, TF, CV, CF))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            if((uiMain->EVM_TP->isChecked()))
            {
                // Heat part
                drawModel(4);

                for(uint j = 1; j < spaceParametrTP-1; ++j)
                {
                    listStatesFirst.append(QString::number(TV.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
                    listStatesSecond.append(QString::number(TF.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
                }
            }
            else
            {
                // Concentration part
                drawModel(5);

                // Out to display steady-state concentration of absorbent
                for(uint j = 1; j < spaceParametrTP-1; ++j)
                {
                    listStatesFirst.append(QString::number(CV.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
                    listStatesSecond.append(QString::number(CF.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
                }
            }

            uiMain->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));
        }
    }

    if((uiMain->ACU->isChecked()))
    {       
        if(!ACU_MM(TV, TB))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            uiMain->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));

            drawModel(6);

            // Out to display steady-state value temperature (ACU)
            for(uint j = 1; j < spaceParametrACU-1; ++j)
            {
                listStatesFirst.append(QString::number(TV.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
                listStatesSecond.append(QString::number(TB.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
            }
        }
    }

    if((uiMain->EVAP->isChecked()))
    {
        if(!EVAP_MM(TF, TB, TFG))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            uiMain->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));

            drawModel(7);

            // Out to display steady-state value temperature (EVAP)
            for(uint j = 1; j < spaceParametrEVAP-1; ++j)
            {
                listStatesFirst.append(QString::number(TF.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
                listStatesSecond.append(QString::number(TB.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
                listStatesThird.append(QString::number(TFG.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
            }
        }
    }

    if((uiMain->TP_ACU->isChecked()))
    {
        if(!TOP_ACU_MM(TV, TF, CV, CF, TB))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            uiMain->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));

            drawModel(8);
            //drawModel(9); // Not work

            // Out to display steady-state value temperature
            for(uint j = 1; j < (spaceParametrTP + spaceParametrACU - 2); ++j)
            {
                listStatesFirst.append(QString::number(TV.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
            }

            for(uint j = 1; j < spaceParametrTP-1; ++j)
            {
                listStatesSecond.append(QString::number(TF.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
            }

            for(uint j = 1; j < spaceParametrACU-1; ++j)
            {
                listStatesThird.append(QString::number(TB.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
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
        msec = static_cast <double>( (timeDiff * 1000) / static_cast <double>(CLOCKS_PER_SEC) );
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

    double selectZ = 0.0;
    QVector <double> t(static_cast <size_t>(selectN / dt));

    QVector <QVector <double>> M_mat0, M_mat1, M_mat2, M_mat3;

    // Define selectZ
    switch(choiceModel)
    {
        case 0:                                                             // H_L_BP
        case 1:                                                             // H_N_BP
        case 2:                                                             // H_BP
        case 3:  selectZ = spaceParametrBP-2;                       break;  // M_BP
        case 4:                                                             // H_TP
        case 5:  selectZ = spaceParametrTP-2;                       break;  // M_TP
        case 6:  selectZ = spaceParametrACU-2;                      break;  // ACU
        case 7:  selectZ = spaceParametrEVAP-2;                     break;  // EVAP
        case 8:                                                             // H_TP_ACU
        case 9:  selectZ = spaceParametrTP + spaceParametrACU-4;    break;  // M_TP_ACU
        case 10:                                                            // H_BP_EVAP
        case 11: selectZ =  spaceParametrBP + spaceParametrEVAP-4;  break;  // M_BP_EVAP
        case 12:                                                            // H_TP_BP
        case 13: selectZ =  spaceParametrTP + spaceParametrBP-4;    break;  // M_TP_BP
        case 14:                                                            // H_FULL_RC
        case 15: selectZ =  spaceParametrTP
                            + spaceParametrACU
                            + spaceParametrBP
                            + spaceParametrEVAP
                            - 16;                                   break;  // M_FULL_RC
        default:
        {
            QMessageBox msgBox;
            msgBox.setText("Driwing error! Unknow type choiceModel!");
            msgBox.exec();
        }
    }

    // Convert from std::vector <std::vector <double>> to QVector <QVector <double>>    
    for(uint i = 1; i <= selectZ; ++i)
    {
        QVector <double> tmpVectorM0, tmpVectorM1, tmpVectorM2, tmpVectorM3;

        for(uint j = 0; j < (static_cast <size_t>((selectN / dt))); ++j)
        {
            switch(choiceModel)
            {
                case 0:                                                                                             // H_L_BP
                case 1:                                                                                             // H_N_BP
                case 2:  tmpVectorM0.push_back(TV.at(j).at(i-1)); tmpVectorM1.push_back(TF.at(j).at(i-1)); break;   // H_BP
                case 3:  tmpVectorM0.push_back(CV.at(j).at(i-1)); tmpVectorM1.push_back(CF.at(j).at(i-1)); break;   // M_BP
                case 4:  tmpVectorM0.push_back(TV.at(j).at(i-1)); tmpVectorM1.push_back(TF.at(j).at(i-1)); break;   // H_TP
                case 5:  tmpVectorM0.push_back(CV.at(j).at(i-1)); tmpVectorM1.push_back(CF.at(j).at(i-1)); break;   // M_TP
                case 6:  tmpVectorM0.push_back(TV.at(j).at(i-1)); tmpVectorM1.push_back(TB.at(j).at(i-1)); break;   // ACU
                case 7:  tmpVectorM0.push_back(TF.at(j).at(i-1)); tmpVectorM1.push_back(TB.at(j).at(i-1)); tmpVectorM2.push_back(TFG.at(j).at(i-1)); break; // EVAP
                case 8:  tmpVectorM0.push_back(TV.at(j).at(i-1)); tmpVectorM1.push_back(TF.at(j).at(i-1)); tmpVectorM2.push_back(TB.at(j).at(i-1)); break;  // H_TP_ACU
                case 9:  tmpVectorM0.push_back(CV.at(j).at(i-1)); tmpVectorM1.push_back(CF.at(j).at(i-1)); break;  // M_TP_ACU
                case 10: tmpVectorM0.push_back(TV.at(j).at(i-1)); tmpVectorM1.push_back(TF.at(j).at(i-1)); tmpVectorM2.push_back(TB.at(j).at(i-1)); tmpVectorM3.push_back(TFG.at(j).at(i-1)); break; // H_BP_EVAP
                case 11: tmpVectorM0.push_back(CV.at(j).at(i-1)); tmpVectorM1.push_back(CF.at(j).at(i-1)); break;  // M_BP_EVAP
                case 12: tmpVectorM0.push_back(TV.at(j).at(i-1)); tmpVectorM1.push_back(TF.at(j).at(i-1)); break;  // H_TP_BP
                case 13: tmpVectorM0.push_back(CV.at(j).at(i-1)); tmpVectorM1.push_back(CF.at(j).at(i-1)); break;  // M_TP_BP
                case 14: /*Problem! Need two TB[][]! */ break;                                                     // H_FULL_RC
                case 15: tmpVectorM0.push_back(CV.at(j).at(i-1)); tmpVectorM1.push_back(CF.at(j).at(i-1)); break;  // M_FULL_RC
                default:
                {
                    QMessageBox msgBox;
                    msgBox.setText("Driwing error! Unknow type choiceModel!");
                    msgBox.exec();
                }
            }

            t[static_cast <size_t>(j)] = j;
        }

        switch(choiceModel)
        {

            case 0:                                                                                             // H_L_BP
            case 1:                                                                                             // H_N_BP
            case 2:                                                                                             // H_BP
            case 3:                                                                                             // M_BP
            case 4:                                                                                             // H_TP
            case 5:                                                                                             // M_TP
            case 6:  M_mat0.push_back(tmpVectorM0); M_mat1.push_back(tmpVectorM1); break;                       // ACU
            case 7:                                                                                             // EVAP
            case 8:  M_mat0.push_back(tmpVectorM0); M_mat1.push_back(tmpVectorM1); M_mat2.push_back(tmpVectorM2); break;  // H_TP_ACU
            case 9:  M_mat0.push_back(tmpVectorM0); M_mat1.push_back(tmpVectorM1); break;                       // M_TP_ACU
            case 10: M_mat0.push_back(tmpVectorM0); M_mat1.push_back(tmpVectorM1); M_mat2.push_back(tmpVectorM2); M_mat3.push_back(tmpVectorM3); break; // H_BP_EVAP
            case 11:                                                                                            // M_BP_EVAP
            case 12:                                                                                            // H_TP_BP
            case 13: M_mat0.push_back(tmpVectorM0); M_mat1.push_back(tmpVectorM1); break;                       // M_TP_BP
            case 14: /*Problem! Need two TB[][]! */ break;                                                      // H_FULL_RC
            case 15: M_mat0.push_back(tmpVectorM0); M_mat1.push_back(tmpVectorM1); break;                       // M_FULL_RC
            default:
            {
                QMessageBox msgBox;
                msgBox.setText("Driwing error! Unknow type choiceModel!");
                msgBox.exec();
            }
        }

    }


    /*-------------Choice drawing processes------------------*/

    uint countModels = 0;
    QVector <QVector <double>> drawingProcces_0, drawingProcces_1, drawingProcces_2, drawingProcces_3;

    switch(choiceModel)
    {

        case 0:                                                                                                 // H_L_BP: TV; TF
        case 1:                                                                                                 // H_N_BP: TV; TF
        case 2:                                                                                                 // H_BP: TV; TF
        case 3:                                                                                                 // M_BP: CV; CF
        case 4:                                                                                                 // H_TP: TV; TF
        case 5:                                                                                                 // M_TP: CV; CF
        case 6:  drawingProcces_0 = M_mat0; drawingProcces_1 = M_mat1; countModels = 2; break;                  // ACU: TV; TB
        case 7:                                                                                                 // EVAP: TF; TB; TFG
        case 8:  drawingProcces_0 = M_mat0; drawingProcces_1 = M_mat1; drawingProcces_2 = M_mat2; countModels = 3; break;  // H_TP_ACU: TV; TF; TB
        case 9:  drawingProcces_0 = M_mat0; drawingProcces_1 = M_mat1; countModels = 2; break;                  // M_TP_ACU: CV; CF
        case 10: drawingProcces_0 = M_mat0; drawingProcces_1 = M_mat1; drawingProcces_2 = M_mat2; drawingProcces_3 = M_mat3; countModels = 4; break; // H_BP_EVAP: TV; TF; TB; TFG
        case 11:                                                                                                // M_BP_EVAP: CV; CF
        case 12:                                                                                                // H_TP_BP: TV; TF
        case 13: drawingProcces_0 = M_mat0; drawingProcces_1 = M_mat1; countModels = 2; break;                  // M_TP_BP: CV; CF
        case 14: /*Problem! Need two TB[][]! */ break;                                                          // H_FULL_RC
        case 15: drawingProcces_0 = M_mat0; drawingProcces_1 = M_mat1; countModels = 2; break;                  // M_FULL_RC: CV; CF

        default:
        {
            QMessageBox msgBox;
            msgBox.setText("Driwing error! Unknow type choiceModel!");
            msgBox.exec();
        }
    }


    uiMain->customPlot->clearGraphs();
    uiMain->customPlot->clearItems();


    /*-------------Rendering graphics------------------*/

    uiMain->customPlot->legend->setVisible(true);

    srand(static_cast <uint>(time(nullptr)));
    int randColorR, randColorG, randColorB;

    QPen pen;
    qreal widthPen = 2.2;
    int translucent = 35;

    for(uint i = 0, j = 0, k = 0, counter = 1; i < selectZ * countModels; ++i, ++counter)
    {
        uiMain->customPlot->addGraph();

        randColorR = rand() % 255;
        randColorG = rand() % 255;
        randColorB = rand() % 255;

        pen.setColor(QColor(randColorR, randColorG, randColorB));

        if(counter <= selectZ)
        {
            /*-------------Customize of pen to drawing first model------------------*/

            pen.setStyle(Qt::SolidLine);
            uiMain->customPlot->graph(static_cast <int>(i))->setName(QString(tr("First phase_%1")).arg(i));
        }
        else
            if(counter <= selectZ * (countModels-1))
            {
                /*-------------Customize of pen to drawing second model------------------*/

                pen.setStyle(Qt::DashLine);
                uiMain->customPlot->graph(static_cast <int>(i))->setName(QString(tr("Third phase_%1")).arg(k));
                ++k;
            }
            else
            {
                /*-------------Customize of pen to drawing third model------------------*/

                pen.setStyle(Qt::DotLine);
                uiMain->customPlot->graph(static_cast <int>(i))->setName(QString(tr("Second phase_%1")).arg(j));
                ++j;
            }

        pen.setWidthF(widthPen);
        uiMain->customPlot->graph(static_cast <int>(i))->setPen(pen);
        uiMain->customPlot->graph(static_cast <int>(i))->setBrush(QBrush(QColor (randColorR,
                                                          randColorG,
                                                          randColorB, translucent) ));
    }

    clock_t timeMW_Cust = clock();
    int msec = 0;

    for(uint i = 0, j = 0, k = 0, counter = 1; i < selectZ * countModels; ++i, ++counter)
    {
        if(counter <= selectZ)
        {
            uiMain->customPlot->graph(static_cast <int>(i))->setData(t, drawingProcces_0[static_cast <int>(i)]);
        }
        else
            if(counter <= selectZ * (countModels-1))
            {
                uiMain->customPlot->graph(static_cast <int>(i))->setData(t, drawingProcces_2[static_cast <int>(k)]);
                ++k;
            }
            else
            {
                uiMain->customPlot->graph(static_cast <int>(i))->setData(t, drawingProcces_1[static_cast <int>(j)]);
                ++j;
            }
    }

    uiMain->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    uiMain->customPlot->replot();

    clock_t timeDiff = clock() - timeMW_Cust;
    msec = static_cast <double>( (timeDiff * 1000) / static_cast <double>(CLOCKS_PER_SEC) );
    cout << endl <<"(QCustomPlot) Drawing time of program taken " << msec/1000 << " seconds, and " << msec%1000 <<" milliseconds!" << endl;
}

void MainWindow::on_save_clicked()
{
    if(uiMain->LVM_BP->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM,std::ref(TV), "TV_BP");
        std::thread threadToFileTFMM(toFileMM,std::ref(TF), "TF_BP");

        threadToFileTVMM.join();
        threadToFileTFMM.join();
    }

    if(uiMain->NLVM_BP->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM,std::ref(TV), "NTV_BP");
        std::thread threadToFileTFMM(toFileMM, std::ref(TF), "NTF_BP");

        threadToFileTVMM.join();
        threadToFileTFMM.join();
    }

    if(uiMain->EVM_BP->isChecked() || uiMain->EFM_BP->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, std::ref(TV), "TV_BP");
        std::thread threadToFileTFMM(toFileMM, std::ref(TF), "TF_BP");
        std::thread threadToFileCVMM(toFileMM, std::ref(CV), "CV_BP");
        std::thread threadToFileCFMM(toFileMM, std::ref(CF), "CF_BP");

        threadToFileTVMM.join();
        threadToFileTFMM.join();
        threadToFileCVMM.join();
        threadToFileCFMM.join();
    }

    if(uiMain->EVM_TP->isChecked() || uiMain->EFM_TP->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, std::ref(TV), "TV_TP");
        std::thread threadToFileTFMM(toFileMM, std::ref(TF), "TF_TP");
        std::thread threadToFileCVMM(toFileMM, std::ref(CV), "CV_TP");
        std::thread threadToFileCFMM(toFileMM, std::ref(CF), "CF_TP");

        threadToFileTVMM.join();
        threadToFileTFMM.join();
        threadToFileCVMM.join();
        threadToFileCFMM.join();
    }

    if(uiMain->ACU->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, std::ref(TV), "TV_ACU");
        std::thread threadToFileTBMM(toFileMM, std::ref(TB), "TB_ACU");

        threadToFileTVMM.join();
        threadToFileTBMM.join();
    }

    if(uiMain->EVAP->isChecked())
    {
        std::thread threadToFileTFMM(toFileMM, std::ref(TF), "TF_EVAP");
        std::thread threadToFileTBMM(toFileMM, std::ref(TB), "TB_EVAP");
        std::thread threadToFileTFGMM(toFileMM, std::ref(TFG), "TFG_EVAP");

        threadToFileTFMM.join();
        threadToFileTBMM.join();
        threadToFileTFGMM.join();
    }

    if(uiMain->TP_ACU->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, std::ref(TV), "TV_TP_ACU");
        std::thread threadToFileTFMM(toFileMM, std::ref(TF), "TF_TP_ACU");
        std::thread threadToFileCVMM(toFileMM, std::ref(CV), "CV_TP_ACU");
        std::thread threadToFileCFMM(toFileMM, std::ref(CF), "CF_TP_ACU");

        std::thread threadToFileTBMM(toFileMM, std::ref(TB), "TB_TP_ACU");

        threadToFileTVMM.join();
        threadToFileTFMM.join();
        threadToFileCVMM.join();
        threadToFileCFMM.join();

        threadToFileTVMM.join();
        threadToFileTBMM.join();
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
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->inputLeftY->insert("100");
    uiMain->inputRightY->insert("170");
    uiMain->inputRightX->insert("10000");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(1.400));
    uiMain->spaceParametrBP->setValue(3);


    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 0, new QTableWidgetItem(tr("160.000")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, 0, new QTableWidgetItem(tr("160.000")));

    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));

    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 1, new QTableWidgetItem(tr("156.999750")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 2, new QTableWidgetItem(tr("153.999500")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 3, new QTableWidgetItem(tr("150.999250")));


    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 0, new QTableWidgetItem(tr("120.377000")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, 0, new QTableWidgetItem(tr("120.377000")));

    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));

    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 1, new QTableWidgetItem(tr("123.382500")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 2, new QTableWidgetItem(tr("126.388000")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 3, new QTableWidgetItem(tr("129.393500")));


    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));


    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_BP_1->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_BP_2->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_BP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_4->setDisabled(true);

    // Clearing other tabs (TP, ACU, EVAP)
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_4->setDisabled(true);
}

void MainWindow::on_NLVM_BP_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->inputLeftY->insert("100");
    uiMain->inputRightY->insert("170");
    uiMain->inputRightX->insert("10000");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(1.400));
    uiMain->spaceParametrBP->setValue(3);

    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 0, new QTableWidgetItem(tr("160.000")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, 0, new QTableWidgetItem(tr("160.000")));

    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));

    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 1, new QTableWidgetItem(tr("156.999750")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 2, new QTableWidgetItem(tr("153.999500")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 3, new QTableWidgetItem(tr("150.999250")));


    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 0, new QTableWidgetItem(tr("120.377000")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, 0, new QTableWidgetItem(tr("120.377000")));

    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));

    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 1, new QTableWidgetItem(tr("123.382500")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 2, new QTableWidgetItem(tr("126.388000")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 3, new QTableWidgetItem(tr("129.393500")));

    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));


    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_BP_1->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_BP_2->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_BP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_4->setDisabled(true);

    // Clearing other tabs (TP, ACU, EVAP)
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_4->setDisabled(true);
}

void MainWindow::on_EVM_BP_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->inputLeftY->insert("100");
    uiMain->inputRightY->insert("170");
    uiMain->inputRightX->insert("10000");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(1.400));
    uiMain->spaceParametrBP->setValue(3);

    //---------------Temperature part------------//
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 0, new QTableWidgetItem(tr("160.000")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, 0, new QTableWidgetItem(tr("160.000")));

    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));

    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 1, new QTableWidgetItem(tr("156.999750")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 2, new QTableWidgetItem(tr("153.999500")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 3, new QTableWidgetItem(tr("150.999250")));


    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 0, new QTableWidgetItem(tr("120.377000")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, 0, new QTableWidgetItem(tr("120.377000")));

    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));

    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 1, new QTableWidgetItem(tr("123.382500")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 2, new QTableWidgetItem(tr("126.388000")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 3, new QTableWidgetItem(tr("129.393500")));

    //---------------Concentration part------------//
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 0, new QTableWidgetItem(tr("67.9440")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, 0, new QTableWidgetItem(tr("67.9440")));

    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("72.0440")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("72.0440")));

    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 1, new QTableWidgetItem(tr("68.9690")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 2, new QTableWidgetItem(tr("69.9940")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 3, new QTableWidgetItem(tr("71.0190")));


    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 0, new QTableWidgetItem(tr("6.550")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, 0, new QTableWidgetItem(tr("6.550")));

    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("2.7880")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("2.7880")));

    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 1, new QTableWidgetItem(tr("5.60950")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 2, new QTableWidgetItem(tr("4.66900")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 3, new QTableWidgetItem(tr("3.72850")));

    uiMain->tableBordersAndInitialConditions_BP_1->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_BP_2->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_BP_3->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_BP_4->setDisabled(false);

    // Clearing other tabs (TP, ACU, EVAP)
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_4->setDisabled(true);
}

void MainWindow::on_EFM_BP_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->inputLeftY->insert("0");
    uiMain->inputRightY->insert("90");
    uiMain->inputRightX->insert("10000");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(1.400));
    uiMain->spaceParametrBP->setValue(3);


    //---------------Temperature part------------//
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 0, new QTableWidgetItem(tr("160.000")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, 0, new QTableWidgetItem(tr("160.000")));

    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("147.999")));

    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 1, new QTableWidgetItem(tr("156.999750")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 2, new QTableWidgetItem(tr("153.999500")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 3, new QTableWidgetItem(tr("150.999250")));


    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 0, new QTableWidgetItem(tr("120.377000")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, 0, new QTableWidgetItem(tr("120.377000")));

    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("132.399000")));

    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 1, new QTableWidgetItem(tr("123.382500")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 2, new QTableWidgetItem(tr("126.388000")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 3, new QTableWidgetItem(tr("129.393500")));

    //---------------Concentration part------------//
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 0, new QTableWidgetItem(tr("67.9440")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, 0, new QTableWidgetItem(tr("67.9440")));

    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("72.0440")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("72.0440")));

    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 1, new QTableWidgetItem(tr("68.9690")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 2, new QTableWidgetItem(tr("69.9940")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 3, new QTableWidgetItem(tr("71.0190")));


    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 0, new QTableWidgetItem(tr("6.550")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, 0, new QTableWidgetItem(tr("6.550")));

    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("2.7880")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("2.7880")));

    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 1, new QTableWidgetItem(tr("5.60950")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 2, new QTableWidgetItem(tr("4.66900")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 3, new QTableWidgetItem(tr("3.72850")));

    uiMain->tableBordersAndInitialConditions_BP_1->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_BP_2->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_BP_3->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_BP_4->setDisabled(false);


    // Clearing other tabs (TP, ACU, EVAP)
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_4->setDisabled(true);
}


void MainWindow::on_EVM_TP_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->inputLeftY->insert("20");
    uiMain->inputRightY->insert("150");
    uiMain->inputRightX->insert("50000");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(3.510));
    uiMain->spaceParametrTP->setValue(3);


    //---------------Temperature part------------//
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 0, new QTableWidgetItem(tr("142.500")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, 0, new QTableWidgetItem(tr("142.500")));

    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("45.300")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("45.300")));

    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 1, new QTableWidgetItem(tr("118.200000")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 2, new QTableWidgetItem(tr("93.9000000")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 3, new QTableWidgetItem(tr("69.6000000")));


    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 0, new QTableWidgetItem(tr("30.0000000")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, 0, new QTableWidgetItem(tr("30.0000000")));

    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("139.000000")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("139.000000")));

    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 1, new QTableWidgetItem(tr("57.2500000")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 2, new QTableWidgetItem(tr("84.5000000")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 3, new QTableWidgetItem(tr("111.750000")));

    //---------------Concentration part------------//
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 0, new QTableWidgetItem(tr("0.5300000")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, 0, new QTableWidgetItem(tr("0.5300000")));

    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("1.138000")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("1.138000")));

    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 1, new QTableWidgetItem(tr("0.6820000")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 2, new QTableWidgetItem(tr("0.8340000")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 3, new QTableWidgetItem(tr("0.9860000")));


    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 0, new QTableWidgetItem(tr("1.0000000")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, 0, new QTableWidgetItem(tr("1.0000000")));

    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.5530000")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.5530000")));

    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 1, new QTableWidgetItem(tr("0.8882500")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 2, new QTableWidgetItem(tr("0.7765000")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 3, new QTableWidgetItem(tr("0.6647500")));

    uiMain->tableBordersAndInitialConditions_TP_1->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_TP_2->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_TP_3->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_TP_4->setDisabled(false);


    // Clearing other tabs (BP, ACU, EVAP)
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_4->setDisabled(true);
}

void MainWindow::on_EFM_TP_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->inputLeftY->insert("0");
    uiMain->inputRightY->insert("1.5");
    uiMain->inputRightX->insert("50000");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(3.510));
    uiMain->spaceParametrTP->setValue(3);


    //---------------Temperature part------------//
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 0, new QTableWidgetItem(tr("142.500")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, 0, new QTableWidgetItem(tr("142.500")));

    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("45.300")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("45.300")));

    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 1, new QTableWidgetItem(tr("118.200000")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 2, new QTableWidgetItem(tr("93.9000000")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 3, new QTableWidgetItem(tr("69.6000000")));


    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 0, new QTableWidgetItem(tr("30.0000000")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, 0, new QTableWidgetItem(tr("30.0000000")));

    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("139.000000")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("139.000000")));

    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 1, new QTableWidgetItem(tr("57.2500000")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 2, new QTableWidgetItem(tr("84.5000000")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 3, new QTableWidgetItem(tr("111.750000")));

    //---------------Concentration part------------//
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 0, new QTableWidgetItem(tr("0.5300000")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, 0, new QTableWidgetItem(tr("0.5300000")));

    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("1.138000")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("1.138000")));

    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 1, new QTableWidgetItem(tr("0.6820000")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 2, new QTableWidgetItem(tr("0.8340000")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 3, new QTableWidgetItem(tr("0.9860000")));


    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 0, new QTableWidgetItem(tr("1.0000000")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, 0, new QTableWidgetItem(tr("1.0000000")));

    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.5530000")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.5530000")));

    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 1, new QTableWidgetItem(tr("0.8882500")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 2, new QTableWidgetItem(tr("0.7765000")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 3, new QTableWidgetItem(tr("0.6647500")));

    uiMain->tableBordersAndInitialConditions_TP_1->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_TP_2->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_TP_3->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_TP_4->setDisabled(false);


    // Clearing other tabs (BP, ACU, EVAP)
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_4->setDisabled(true);
}

void MainWindow::on_ACU_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->inputLeftY->insert("0");
    uiMain->inputRightY->insert("80");
    uiMain->inputRightX->insert("30");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(4.0));
    uiMain->spaceParametrACU->setValue(3);

    //---------------Temperature part------------//
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 0, new QTableWidgetItem(tr("72.500")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, 0, new QTableWidgetItem(tr("72.500")));

    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("42.49400")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("42.49400")));

    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 1, new QTableWidgetItem(tr("64.9985000")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 2, new QTableWidgetItem(tr("57.4970000")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 3, new QTableWidgetItem(tr("49.9955000")));


    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 0, new QTableWidgetItem(tr("30.7500000")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, 0, new QTableWidgetItem(tr("30.7500000")));

    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("23.2900000")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("23.2900000")));

    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 1, new QTableWidgetItem(tr("28.8850000")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 2, new QTableWidgetItem(tr("27.0200000")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 3, new QTableWidgetItem(tr("25.1550000")));


    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));


    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_ACU_1->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_ACU_2->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_ACU_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_4->setDisabled(true);


    // Clearing other tabs (BP, TP, EVAP)
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_4->setDisabled(true);
}

void MainWindow::on_EVAP_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->inputLeftY->insert("140");
    uiMain->inputRightY->insert("300");
    uiMain->inputRightX->insert("3000");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(5.5));
    uiMain->spaceParametrEVAP->setValue(4);


    if((uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1) == 4)
    {
        uiMain->tableBordersAndInitialConditions_EVAP_1->insertColumn(4);
        uiMain->tableBordersAndInitialConditions_EVAP_2->insertColumn(4);
        uiMain->tableBordersAndInitialConditions_EVAP_3->insertColumn(4);
        uiMain->tableBordersAndInitialConditions_EVAP_4->insertColumn(4);
    }

    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 0, new QTableWidgetItem(tr("160.00")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, 0, new QTableWidgetItem(tr("160.00")));

    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("139.00")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("139.00")));

    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 1, new QTableWidgetItem(tr("160.161650")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 2, new QTableWidgetItem(tr("154.220450")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 3, new QTableWidgetItem(tr("148.736080")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 4, new QTableWidgetItem(tr("143.673400")));


    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 0, new QTableWidgetItem(tr("160.200")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, 0, new QTableWidgetItem(tr("160.200")));

    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("147.07220")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("147.07220")));

    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 1, new QTableWidgetItem(tr("164.48250")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 2, new QTableWidgetItem(tr("158.20900")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 3, new QTableWidgetItem(tr("152.41800")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 4, new QTableWidgetItem(tr("147.07220")));


    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 0, new QTableWidgetItem(tr("300.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, 0, new QTableWidgetItem(tr("300.0")));

    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("240.54030")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("240.54030")));

    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 1, new QTableWidgetItem(tr("283.30650")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 2, new QTableWidgetItem(tr("267.89660")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 3, new QTableWidgetItem(tr("253.67150")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 4, new QTableWidgetItem(tr("240.54030")));


    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 4, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_EVAP_1->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_EVAP_2->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_EVAP_3->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_EVAP_4->setDisabled(true);


    // Clearing other tabs (BP, TP, ACU)
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_TP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_TP_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_4->setDisabled(true);
}

void MainWindow::on_TP_ACU_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->inputLeftY->insert("0");
    uiMain->inputRightY->insert("150");
    uiMain->inputRightX->insert("200");

    leftY = uiMain->inputLeftY->text().toDouble();
    rightY = uiMain->inputRightY->text().toDouble();

    uiMain->selectDRC->setText(QString::number(7.510)); // (RC_TOP)3.51 + (ACU)4
    uiMain->spaceParametrTP->setValue(3);
    uiMain->spaceParametrACU->setValue(3);

    // RC_TOP:
    //---------------Temperature part------------//
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 0, new QTableWidgetItem(tr("142.500")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(1, 0, new QTableWidgetItem(tr("142.500")));

    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1), new QTableWidgetItem(tr("...")));

    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 1, new QTableWidgetItem(tr("118.200000")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 2, new QTableWidgetItem(tr("93.9000000")));
    uiMain->tableBordersAndInitialConditions_TP_1->setItem(0, 3, new QTableWidgetItem(tr("69.6000000")));


    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 0, new QTableWidgetItem(tr("30.0000000")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, 0, new QTableWidgetItem(tr("30.0000000")));

    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("139.000000")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1), new QTableWidgetItem(tr("139.000000")));

    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 1, new QTableWidgetItem(tr("57.2500000")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 2, new QTableWidgetItem(tr("84.5000000")));
    uiMain->tableBordersAndInitialConditions_TP_2->setItem(0, 3, new QTableWidgetItem(tr("111.750000")));

    //---------------Concentration part------------//
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 0, new QTableWidgetItem(tr("0.5300000")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, 0, new QTableWidgetItem(tr("0.5300000")));

    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("1.138000")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1), new QTableWidgetItem(tr("1.138000")));

    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 1, new QTableWidgetItem(tr("0.6820000")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 2, new QTableWidgetItem(tr("0.8340000")));
    uiMain->tableBordersAndInitialConditions_TP_3->setItem(0, 3, new QTableWidgetItem(tr("0.9860000")));


    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 0, new QTableWidgetItem(tr("1.0000000")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, 0, new QTableWidgetItem(tr("1.0000000")));

    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.5530000")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1), new QTableWidgetItem(tr("0.5530000")));

    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 1, new QTableWidgetItem(tr("0.8882500")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 2, new QTableWidgetItem(tr("0.7765000")));
    uiMain->tableBordersAndInitialConditions_TP_4->setItem(0, 3, new QTableWidgetItem(tr("0.6647500")));

    uiMain->tableBordersAndInitialConditions_TP_1->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_TP_2->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_TP_3->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_TP_4->setDisabled(false);


    // ACU:
    //---------------Temperature part------------//
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 0, new QTableWidgetItem(tr("...")));

    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("42.49400")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1), new QTableWidgetItem(tr("42.49400")));

    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 1, new QTableWidgetItem(tr("64.9985000")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 2, new QTableWidgetItem(tr("57.4970000")));
    uiMain->tableBordersAndInitialConditions_ACU_1->setItem(0, 3, new QTableWidgetItem(tr("49.9955000")));


    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 0, new QTableWidgetItem(tr("30.7500000")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, 0, new QTableWidgetItem(tr("30.7500000")));

    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("23.2900000")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1), new QTableWidgetItem(tr("23.2900000")));

    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 1, new QTableWidgetItem(tr("28.8850000")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 2, new QTableWidgetItem(tr("27.0200000")));
    uiMain->tableBordersAndInitialConditions_ACU_2->setItem(0, 3, new QTableWidgetItem(tr("25.1550000")));


    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));


    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(1, (uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_ACU_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));

    uiMain->tableBordersAndInitialConditions_ACU_1->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_ACU_2->setDisabled(false);
    uiMain->tableBordersAndInitialConditions_ACU_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_ACU_4->setDisabled(true);


    // Clearing other tabs (BP, EVAP)
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_BP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_BP_4->setDisabled(true);

    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_2->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_3->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, 0, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(1, (uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1), new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 1, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 2, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_4->setItem(0, 3, new QTableWidgetItem(tr("0.0")));
    uiMain->tableBordersAndInitialConditions_EVAP_1->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_2->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_3->setDisabled(true);
    uiMain->tableBordersAndInitialConditions_EVAP_4->setDisabled(true);
}



//--------------------------LINER MODEL-----------------------------------
bool TMTPL_MM(vector <vector <double> > &TV, vector <vector <double> > &TF)
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
    bmp.assign(spaceParametrBP, 0.0);

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

    beginPoint = 0;

    std::thread threadInitialLayerTV(initialLayerTV, std::ref(TV), spaceParametrBP);
    std::thread threadInitialLayerTF(initialLayerTF, std::ref(TF), spaceParametrBP);

    threadInitialLayerTV.join();
    threadInitialLayerTF.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << TV.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << TF.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)// time
    {
       for(uint j = 1; j < (spaceParametrBP-1); ++j)        // place
       {
           TV.at(i).at(j) =
                   (dt * RvT * TF.at(i-1).at((spaceParametrBP-1)-j))
                   + (PTV * TV.at(i-1).at(j-1))
                   + TV.at(i-1).at(j) * (-(dt * RvT) - PTV)
                   + TV.at(i-1).at(j);

           TF.at(i).at(j) =
                   (dt * RfT * TV.at(i-1).at((spaceParametrBP-1)-j))
                   + (PTF * TF.at(i-1).at(j-1))
                   + TF.at(i-1).at(j) * (-PTF - (dt * RfT))
                   + TF.at(i-1).at(j);
       }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << TV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << TF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    return true;
}

//------------------------NON-LINER MODEL---------------------------------
bool TMTPN_MM(vector <vector <double> > &TV, vector <vector <double> > &TF)
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
    bmp.assign(spaceParametrBP, 0.0);

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

    beginPoint = 0;

    std::thread threadInitialLayerTV(initialLayerTV, std::ref(TV), spaceParametrBP);
    std::thread threadInitialLayerTF(initialLayerTF, std::ref(TF), spaceParametrBP);

    threadInitialLayerTV.join();
    threadInitialLayerTF.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << TV.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << TF.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)
    {
       for(uint j = 1; j < (spaceParametrBP-1); ++j)
       {
           PTV_N = (a0 * TV.at(i-1).at(j) * dt) / dh;

           TV.at(i).at(j) =
                   (dt * RvT * TF.at(i-1).at((spaceParametrBP-1)-j))
                   - (PTV_N * TV.at(i-1).at(j+1))
                   - TV.at(i-1).at(j) * (dt*RvT + PTV_L - PTV_N)
                   + (PTV_L * TV.at(i-1).at(j-1))
                   + TV.at(i-1).at(j);

           TF.at(i).at(j) =
                   (dt * RfT * TV.at(i-1).at((spaceParametrBP-1)-j))
                   + (PTF * TF.at(i-1).at(j-1))
                   - TF.at(i-1).at(j) * (dt*RfT + PTF)
                   + TF.at(i-1).at(j);
        }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << TV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << TF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    return true;
}

//----------------INTERCONNECTED MODEL(BOTTOM PART)-----------------------
bool ETMBP_MM(vector <vector <double> > &TV, vector <vector <double> > &TF,
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
    bmp.assign(spaceParametrBP, 0.0);

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

    beginPoint = 0;

    std::thread threadInitialLayerTV(initialLayerTV, std::ref(TV), spaceParametrBP);
    std::thread threadInitialLayerTF(initialLayerTF, std::ref(TF), spaceParametrBP);
    std::thread threadInitialLayerCV(initialLayerCV, std::ref(CV), spaceParametrBP);
    std::thread threadInitialLayerCF(initialLayerCF, std::ref(CF), spaceParametrBP);

    threadInitialLayerTV.join();
    threadInitialLayerTF.join();
    threadInitialLayerCV.join();
    threadInitialLayerCF.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << TV.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << TF.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << CV.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << CF.at(0).at(j) << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)// time
    {
        for(size_t j = 1; j < (spaceParametrBP-1); ++j)      // place
        {
            // -----Calculate layer heat exchenger model------
            PTV_N = (a0 * TV.at(i-1).at(j) * dt) / dh;

            TV.at(i).at(j) =
                    (dt * RvT * TF.at(i-1).at((spaceParametrBP-1)-j))
                    - (PTV_N * TV.at(i-1).at(j+1))
                    - TV.at(i-1).at(j) * (dt*RvT + PTV_L - PTV_N)
                    + (PTV_L * TV.at(i-1).at(j-1))
                    + TV.at(i-1).at(j);

            TF.at(i).at(j) =
                    (dt * RfT * TV.at(i-1).at((spaceParametrBP-1)-j))
                    + (PTF * TF.at(i-1).at(j-1))
                    - TF.at(i-1).at(j) * (dt*RfT + PTF)
                    + TF.at(i-1).at(j);

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
            CV.at(i).at(j) =
                    - (dt * RvM * E * CF.at(i-1).at((spaceParametrBP-1)-j))
                    + CV.at(i-1).at(j) * (dt*RvM - PTV_L - PTV_N + 1)
                    + CV.at(i-1).at(j+1) * (PTV_L + PTV_N);

            /* 4 B-
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM + PTV_L + PTV_N + 1)
                        - CV[i-1][j+1] * (PTV_L + PTV_N);*/

            /* 1 A-
            CF[i][j] =  - (dt * RfM * CV[i-1][(selectZ-1)-j])
                        + CF[i-1][j] * (dt*RfM*E + PTF + 1)
                        - (PTF * CF[i-1][j-1]);*/

            /* 1 B+*/
            CF.at(i).at(j) =
                    - (dt * RfM * CV.at(i-1).at(j))
                    + CF.at(i-1).at(j) * (dt*RfM*E - PTF + 1)
                    + (PTF * CF.at(i-1).at(j-1));

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
    for(uint j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << TV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << TF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << CV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << CF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    return true;
}

//-----------------INTERCONNECTED MODEL(TOP PART)-------------------------
bool ETMTP_MM(vector <vector <double> > &TV, vector <vector <double> > &TF,
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
    bmp.assign(spaceParametrTP, 0.0);

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

    beginPoint = 0;

    std::thread threadInitialLayerTV(initialLayerTV, std::ref(TV), spaceParametrTP);
    std::thread threadInitialLayerTF(initialLayerTF, std::ref(TF), spaceParametrTP);
    std::thread threadInitialLayerCV(initialLayerCV, std::ref(CV), spaceParametrTP);
    std::thread threadInitialLayerCF(initialLayerCF, std::ref(CF), spaceParametrTP);

    threadInitialLayerTV.join();
    threadInitialLayerTF.join();
    threadInitialLayerCV.join();
    threadInitialLayerCF.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < spaceParametrTP; ++j)
    {
        cout << TV.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrTP; ++j)
    {
        cout << TF.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrTP; ++j)
    {
        cout << CV.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrTP; ++j)
    {
        cout << CF.at(0).at(j) << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)// time
    {
        for(size_t j = 1; j < (spaceParametrTP-1); ++j)      // place
        {
            // -----Calculate layer heat exchenger model------
            PTV_N = (a0 * TV.at(i-1).at(j) * dt) / dh;

            TV.at(i).at(j) =
                    (dt * RvT * TF.at(i-1).at((spaceParametrTP-1)-j))
                    - (PTV_N * TV.at(i-1).at(j+1))
                    - TV.at(i-1).at(j) * (dt*RvT + PTV_L - PTV_N)
                    + (PTV_L * TV.at(i-1).at(j-1))
                    + TV.at(i-1).at(j);

            TF.at(i).at(j) =
                    (dt * RfT * TV.at(i-1).at((spaceParametrTP-1)-j))
                    + (PTF * TF.at(i-1).at(j-1))
                    - TF.at(i-1).at(j) * (dt*RfT + PTF)
                    + TF.at(i-1).at(j);

            // -----Calculate layer mass exchenger model------

            /* New schema: CV(i,i-1); CV(i,i-1). Error was increase..., but dynamic of process good*
            CV[i][j] = - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                    + (PTV_L + PTV_N) * CV[i-1][j-1]
                    - CV[i-1][j] * (- dt*RvM + PTV_L + PTV_N)
                    + CV[i-1][j];*/

            CF.at(i).at(j) =
                    - (dt * RfM * CV.at(i-1).at((spaceParametrTP-1)-j))
                    + (PTF * CF.at(i-1).at(j-1))
                    - CF.at(i-1).at(j) * (-(dt*RfM*E) + PTF)
                    + CF.at(i-1).at(j);

            /* Old schema: CV(i,i-1); CV(i+1,i)*/

            CV.at(i).at(j) =
                    - (dt * RvM * E * CF.at(i-1).at((spaceParametrTP-1)-j))
                    - CV.at(i-1).at(j) * (PTV_L - PTV_N + dt*RvM)
                    + (PTV_L * CV.at(i-1).at(j-1))
                    - (PTV_N * CV.at(i-1).at(j-1))
                    + CV.at(i-1).at(j);

        }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint j = 1; j < (spaceParametrTP-1); ++j)
    {
        cout << TV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint j = 1; j < (spaceParametrTP-1); ++j)
    {
        cout << TF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint j = 1; j < (spaceParametrTP-1); ++j)
    {
        cout << CV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint j = 1; j < (spaceParametrTP-1); ++j)
    {
        cout << CF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    return true;
}

//------------------AIR-COOLING UNIT MODEL(ACU)---------------------------
bool ACU_MM(vector<vector<double> > &TV, vector<vector<double> > &TB)
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
    bmp.assign(spaceParametrACU, 0.0);

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

    beginPoint = 0;

    std::thread threadInitialLayerTV(initialLayerTV, std::ref(TV), spaceParametrACU);
    std::thread threadInitialLayerTB(initialLayerTB, std::ref(TB), spaceParametrACU);

    threadInitialLayerTV.join();
    threadInitialLayerTB.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < spaceParametrACU; ++j)
    {
        cout << TV.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrACU; ++j)
    {
        cout << TB.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)
    {
        for(size_t j = 1; j < (spaceParametrACU-1); ++j)
        {
            // -----Calculate layer heat exchenger model------
            TV.at(i).at(j) =
                    (dt * RvT * TB.at(i-1).at((spaceParametrACU-1)-j))
                    - TV.at(i-1).at(j) * ((dt*RvT) + (dt*PTV_L))
                    + (dt * PTV_L * TV.at(i-1).at(j-1))
                    + TV.at(i-1).at(j);

            TB.at(i).at(j) =
                    (dt * RB2 * TV.at(i-1).at(j))              // TV[i-1][j] ??? j - ????
                    + (CP*dt*RB1*TE)                        // ? TE - const or function?
                    - TB.at(i-1).at(j) * (CP*dt*RB1 + dt*RB2)
                    + TB.at(i-1).at(j);
        }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint j = 1; j < (spaceParametrACU-1); ++j)
    {
        cout << TV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint j = 1; j < (spaceParametrACU-1); ++j)
    {
        cout << TB.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    return true;
}

bool EVAP_MM(vector <vector <double> > &TF, vector <vector <double> > &TB, vector <vector <double> > &TFG)
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
    bmp.assign(spaceParametrEVAP, 0.0);

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

    beginPoint = 0;

    std::thread threadInitialLayerTF(initialLayerTF, std::ref(TF), spaceParametrEVAP);
    std::thread threadInitialLayerTB(initialLayerTB, std::ref(TB), spaceParametrEVAP);
    std::thread threadInitialLayerTFG(initialLayerTFG, std::ref(TFG));

    threadInitialLayerTF.join();
    threadInitialLayerTB.join();
    threadInitialLayerTFG.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < spaceParametrEVAP; ++j)
    {
        cout << TF.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrEVAP; ++j)
    {
        cout << TB.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrEVAP; ++j)
    {
        cout << TFG.at(0).at(j) << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)
    {
        for(size_t j = 1; j < (spaceParametrEVAP-1); ++j)
        {
            TF.at(i).at(j) =
                    (dt * RF * TB.at(i-1).at(j+1)) // ?
                    + (dt * PTF * TF.at(i-1).at(j+1))
                    - TF.at(i-1).at(j) * (dt*RF + dt*PTF)
                    + TF.at(i-1).at(j);

            TB.at(i).at(j) =
                    (dt * RFB * TF.at(i-1).at(j)) // ?
                    + (dt * RFGB * TFG.at(i-1).at((spaceParametrEVAP-1)-j))// ?
                    - TB.at(i-1).at(j) * (dt*RFGB + dt*RFB)
                    + TB.at(i-1).at(j);

            TFG.at(i).at(j) =
                    (dt * RFG * TB.at(i-1).at(j+1)) // ?
                    + (dt * PTFG * TFG.at(i-1).at(j-1))
                    - TFG.at(i-1).at(j) * (dt*RFG + dt*PTFG)
                    + TFG.at(i-1).at(j);
        }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint j = 1; j < (spaceParametrEVAP-1); ++j)
    {
        cout << TF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint j = 1; j < (spaceParametrEVAP-1); ++j)
    {
        cout << TB.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint j = 1; j < (spaceParametrEVAP-1); ++j)
    {
        cout << TFG.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;


    return true;
}

bool TOP_ACU_MM(vector <vector <double> > &TV, vector <vector <double> > &TF,
             vector <vector <double> > &CV, vector <vector <double> > &CF,
             vector<vector<double> > &TB)
{
    // RC_TOP:
    // -----Model's heat exchenger parameters------
    double  RC_TOP_RvT = 2.500, RC_TOP_RfT = 0.000085500, RC_TOP_a0 = 0.0034868520 // (a0 = 0.0034868520) != (a0_Simulink = 0.002702752)
          , RC_TOP_PTV_L = (RC_TOP_a0 * 273.15 * dt) / 1.17 // dh
          , RC_TOP_PTV_N = 0.0
          , RC_TOP_PTF = (0.0000400 * dt) / 1.17; // dh

    // -----Model's mass exchenger parameters------
    double RC_TOP_E = 1.0E-9, RC_TOP_RvM = 0.1450, RC_TOP_RfM = 6.0E-6;

    if( (RC_TOP_PTV_L > 0.5) || (RC_TOP_PTF > 0.5) )
    {
        qDebug() << "The scheme RC_TOP diverges!!!";
        return false;
    }

    //--------------------------------------------------------

    // ACU:
    // -----Model's heat parameters------
    double  ACU_RvT = 8.400, ACU_a0 = 0.06     // a0 = 0.06 ?
          , ACU_PTV_L = (ACU_a0 * 273.150) / 1.333; // dh

    // -----Model's boarder parameters------
    double ACU_RB1 = 0.00661, ACU_RB2 = 0.480
         , ACU_TE = 12.380                  // Temperature of enviroment
         , ACU_CP = 134.0;                  // Count piplines;

    if((dt * ACU_PTV_L) > 0.5 )
    {
        qDebug() << "The scheme ACU diverges!!!";
        return false;
    }

    //--------------------------------------------------------

    vector <double> countSpacePoints_1, countSpacePoints_2;
    countSpacePoints_1.assign(spaceParametrTP + spaceParametrACU - 2, 0.0);
    countSpacePoints_2.assign(spaceParametrTP, 0.0);

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TF.erase(TF.begin(), TF.end());
        CV.erase(CV.begin(), CV.end());
        CF.erase(CF.begin(), CF.end());

        TB.erase(TB.begin(), TB.end());
    }

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.push_back(countSpacePoints_1);
        TF.push_back(countSpacePoints_2);
        CV.push_back(countSpacePoints_2);
        CF.push_back(countSpacePoints_2);

        TB.push_back(countSpacePoints_2);
    }

    //--------------------------------------------------------

    beginPoint = 0;

    std::thread threadInitialLayerTV(initialLayerTV, std::ref(TV), spaceParametrTP + spaceParametrACU - 2);
    std::thread threadInitialLayerTF(initialLayerTF, std::ref(TF), spaceParametrTP);
    std::thread threadInitialLayerCV(initialLayerCV, std::ref(CV), spaceParametrTP);
    std::thread threadInitialLayerCF(initialLayerCF, std::ref(CF), spaceParametrTP);

    std::thread threadInitialLayerTB(initialLayerTB, std::ref(TB), spaceParametrACU);


    threadInitialLayerTV.join();
    threadInitialLayerTF.join();
    threadInitialLayerCV.join();
    threadInitialLayerCF.join();

    threadInitialLayerTB.join();


    cout << endl << "Initial values RC_TOP:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < spaceParametrTP + spaceParametrACU - 2; ++j)
    {
        cout << TV.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrTP; ++j)
    {
        cout << TF.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrTP; ++j)
    {
        cout << CV.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrTP; ++j)
    {
        cout << CF.at(0).at(j) << " | ";
    }   cout << endl;


    beginPoint = spaceParametrTP - 1;
    endPoint = spaceParametrTP + spaceParametrACU - 2;
    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)// time
    {
        for(size_t j = 1; j < (spaceParametrTP-1); ++j)      // place
        {
            // -----Calculate layer heat exchenger model------
            RC_TOP_PTV_N = (RC_TOP_a0 * TV.at(i-1).at(j) * dt) / 1.17; //dh;

            TV.at(i).at(j) =
                    (dt * RC_TOP_RvT * TF.at(i-1).at((spaceParametrTP-1)-j))
                    - (RC_TOP_PTV_N * TV.at(i-1).at(j+1))
                    - TV.at(i-1).at(j) * (dt*RC_TOP_RvT + RC_TOP_PTV_L - RC_TOP_PTV_N)
                    + (RC_TOP_PTV_L * TV.at(i-1).at(j-1))
                    + TV.at(i-1).at(j);

            cout << TV[i][j] << "; ";

            TF.at(i).at(j) =
                    (dt * RC_TOP_RfT * TV.at(i-1).at((spaceParametrTP-1)-j))
                    + (RC_TOP_PTF * TF.at(i-1).at(j-1))
                    - TF.at(i-1).at(j) * (dt*RC_TOP_RfT + RC_TOP_PTF)
                    + TF.at(i-1).at(j);


            CF.at(i).at(j) =
                    - (dt * RC_TOP_RfM * CV.at(i-1).at((spaceParametrTP-1)-j))
                    + (RC_TOP_PTF * CF.at(i-1).at(j-1))
                    - CF.at(i-1).at(j) * (-(dt*RC_TOP_RfM*RC_TOP_E) + RC_TOP_PTF)
                    + CF.at(i-1).at(j);


            CV.at(i).at(j) =
                    - CV.at(i-1).at(j) * (RC_TOP_PTV_L - 1 - RC_TOP_PTV_N + dt*RC_TOP_RvM)
                    + (RC_TOP_PTV_L * CV.at(i-1).at(j-1))
                    - (RC_TOP_PTV_N * CV.at(i-1).at(j-1))
                    - (dt * RC_TOP_RvM * RC_TOP_E * CF.at(i-1).at((spaceParametrTP-1)-j));
        }

        for(size_t j = beginPoint; j < endPoint - 1; ++j)
        {
            // -----Calculate layer heat exchenger model------
            TV.at(i).at(j) =
                    (dt*ACU_RvT * TB.at(i-1).at((endPoint - 1) - j))
                    - TV.at(i-1).at(j) * ((dt*ACU_RvT) + (dt*ACU_PTV_L))
                    + (dt * ACU_PTV_L * TV.at(i-1).at(j-1))
                    + TV.at(i-1).at(j);

            cout << TV[i][j] << "; ";

            TB.at(i).at(j-beginPoint + 1) =
                    (dt * ACU_RB2 * TV.at(i-1).at(endPoint + 2 - j))
                    + (ACU_CP*dt*ACU_RB1*ACU_TE)
                    - TB.at(i-1).at(j-beginPoint) * (ACU_CP*dt*ACU_RB1 + dt*ACU_RB2)
                    + TB.at(i-1).at(j-beginPoint);
        }
        cout << endl;

        TF.at(i).at(0) = TV.at(i).at(endPoint-2);   // Connection between ACU(TV) -> RC_TOP(TF)
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint j = 0; j < (endPoint); ++j)
    {
        cout << TV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint j = 0; j < (spaceParametrTP); ++j)
    {
        cout << TF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint j = 1; j < (spaceParametrTP-1); ++j)
    {
        cout << CV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint j = 1; j < (spaceParametrTP-1); ++j)
    {
        cout << CF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;


    cout << endl << "Steady-state values TB:" << endl;
    for(uint j = 1; j < (spaceParametrACU-1); ++j)
    {
        cout << TB.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    return true;
}

void initialLayerTV(vector <vector <double> > &TV, unsigned int it)
{
    size_t i = 0, j = 0, k = 1;

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.at(i).at(beginPoint) = initLayerTV_0.at(0);
        TV.at(i).at(it-1) = initLayerTV_1.at(0);
    }

    // Initial values
    for(j = beginPoint + 1; j <= (it-2); ++j)
    {
        TV.at(0).at(j) = initLayerTV.at(j-k);
    }
}

void initialLayerTF(vector <vector <double> > &TF, unsigned int it)
{
    size_t i = 0, j = 0;

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TF.at(i).at(beginPoint) = initLayerTF_0.at(0);
        TF.at(i).at(it-1) = initLayerTF_1.at(0);
    }

    // Initial values
    for(j = beginPoint + 1; j <= (it-2); ++j)
    {
        TF.at(0).at(j) = initLayerTF.at(j-1);
    }
}

void initialLayerCV(vector <vector <double> > &CV, unsigned int it)
{
    size_t i = 0, j = 0;

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        CV.at(i).at(beginPoint) = initLayerCV_0.at(0);
        CV.at(i).at(it-1) = initLayerCV_1.at(0);
    }

    // Initial values
    for(j = beginPoint + 1; j <= (it-2); ++j)
    {
        CV.at(0).at(j) = initLayerCV.at(j-1);
    }
}

void initialLayerCF(vector <vector <double> > &CF, unsigned int it)
{
    size_t i = 0, j = 0;

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        CF.at(i).at(beginPoint) = initLayerCF_0.at(0);
        CF.at(i).at(it-1) = initLayerCF_1.at(0);
    }

    // Initial values
    for(j = beginPoint + 1; j <= (it-2); ++j)
    {
        CF.at(0).at(j) = initLayerCF.at(j-1);
    }
}

void initialLayerTB(vector <vector <double> > &TB, unsigned int it)
{
    size_t i = 0, j = 0;

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TB.at(i).at(0) = initLayerTB_0;
        TB.at(i).at(it-1) = initLayerTB_1;
    }

    // Initial values
    for(j = 1; j <= (it-2); ++j)
    {
        TB.at(0).at(j) = initLayerTB.at(j-1);
    }
}

void initialLayerTFG(vector <vector <double> > &TFG)
{
    size_t i = 0, j = 0;

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TFG.at(i).at(0) = initLayerTFG_0;
        TFG.at(i).at(spaceParametrEVAP-1) = initLayerTFG_1;
    }

    // Initial values
    for(j = 1; j <= (spaceParametrEVAP-2); ++j)
    {
        TFG.at(0).at(j) = initLayerTFG.at(j-1);
    }
}

void toFileMM(vector <vector <double> > MMM, string nameModel)
{
    uint sizeModel = 0;

    // In future - RegExp
    if(nameModel == "TP_ACU_TV")
    {
        sizeModel = spaceParametrTP + spaceParametrACU - 2;
    }
    else
    {
        sizeModel = spaceParametrTP - 1;
    }

    nameModel = "MM_" + nameModel + ".dat";

    ofstream foutMM(nameModel, ios_base::out | ios_base::trunc);

    for(size_t i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        for(uint j = 1; j < (sizeModel); j++)
        {
            foutMM << MMM.at(i).at(j) << " ";
        }
        foutMM << endl;
    }
    foutMM.close();
}

void MainWindow::on_selectDRC_textChanged(QString dRCNew)
{
    dRC = dRCNew.toDouble(&okey);

    if(uiMain->LVM_BP->isChecked() || uiMain->NLVM_BP->isChecked() || uiMain->EVM_BP->isChecked() || uiMain->EFM_BP->isChecked())
    {
        spaceParametrBP = static_cast <uint> (uiMain->spaceParametrBP->value());
        dh = dRC / ( static_cast <double> (spaceParametrBP) );
    }

    if(uiMain->EVM_TP->isChecked() || uiMain->EFM_TP->isChecked())
    {
        spaceParametrTP = static_cast <uint> (uiMain->spaceParametrTP->value());
        dh = dRC / ( static_cast <double> (spaceParametrTP) );
    }

    if(uiMain->ACU->isChecked())
    {
        spaceParametrACU = static_cast <uint> (uiMain->spaceParametrACU->value());
        dh = dRC / ( static_cast <double> (spaceParametrACU) );
    }

    if(uiMain->EVAP->isChecked())
    {
        spaceParametrEVAP = static_cast <uint> (uiMain->spaceParametrEVAP->value());
        dh = dRC / ( static_cast <double> (spaceParametrEVAP) );
    }

    if(uiMain->TP_ACU->isChecked())
    {
        spaceParametrTP = static_cast <uint> (uiMain->spaceParametrTP->value());
        spaceParametrACU = static_cast <uint> (uiMain->spaceParametrACU->value());
        dh = dRC / ( static_cast <double> (spaceParametrTP + spaceParametrACU) );
    }

    uiMain->selectStepH->setText(QString::number(dh));
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

void MainWindow::on_spaceParametrBP_valueChanged(int countSpacePoints)
{
    if(uiMain->LVM_BP->isChecked() || uiMain->NLVM_BP->isChecked() || uiMain->EVM_BP->isChecked() || uiMain->EFM_BP->isChecked())
    {
        if((uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1) <= countSpacePoints)
        {
            uiMain->tableBordersAndInitialConditions_BP_1->insertColumn(uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_BP_2->insertColumn(uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_BP_3->insertColumn(uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_BP_4->insertColumn(uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1);
        }
        else
        {
            uiMain->tableBordersAndInitialConditions_BP_1->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_BP_1->removeColumn(uiMain->tableBordersAndInitialConditions_BP_1->currentColumn()-1);

            uiMain->tableBordersAndInitialConditions_BP_2->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_BP_2->removeColumn(uiMain->tableBordersAndInitialConditions_BP_2->currentColumn()-1);

            uiMain->tableBordersAndInitialConditions_BP_3->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_BP_3->removeColumn(uiMain->tableBordersAndInitialConditions_BP_3->currentColumn()-1);

            uiMain->tableBordersAndInitialConditions_BP_4->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_BP_4->removeColumn(uiMain->tableBordersAndInitialConditions_BP_4->currentColumn()-1);
        }

        dh = dRC/( static_cast <double> (countSpacePoints) );
        uiMain->selectStepH->setText(QString::number(dh));
    }
}

void MainWindow::on_spaceParametrTP_valueChanged(int countSpacePoints)
{
    if(uiMain->EVM_TP->isChecked() || uiMain->EFM_TP->isChecked())
    {
        if((uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1) <= countSpacePoints)
        {
            uiMain->tableBordersAndInitialConditions_TP_1->insertColumn(uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_TP_2->insertColumn(uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_TP_3->insertColumn(uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_TP_4->insertColumn(uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1);
        }
        else
        {
            uiMain->tableBordersAndInitialConditions_TP_1->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_TP_1->removeColumn(uiMain->tableBordersAndInitialConditions_TP_1->currentColumn()-1);

            uiMain->tableBordersAndInitialConditions_TP_2->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_TP_2->removeColumn(uiMain->tableBordersAndInitialConditions_TP_2->currentColumn()-1);

            uiMain->tableBordersAndInitialConditions_TP_3->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_TP_3->removeColumn(uiMain->tableBordersAndInitialConditions_TP_3->currentColumn()-1);

            uiMain->tableBordersAndInitialConditions_TP_4->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_TP_4->removeColumn(uiMain->tableBordersAndInitialConditions_TP_4->currentColumn()-1);
        }

        dh = dRC/( static_cast <double> (countSpacePoints) );
        uiMain->selectStepH->setText(QString::number(dh));
    }
}

void MainWindow::on_spaceParametrACU_valueChanged(int countSpacePoints)
{
    if(uiMain->ACU->isChecked())
    {
        if((uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1) <= countSpacePoints)
        {
            uiMain->tableBordersAndInitialConditions_ACU_1->insertColumn(uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_ACU_2->insertColumn(uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_ACU_3->insertColumn(uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_ACU_4->insertColumn(uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1);
        }
        else
        {
            uiMain->tableBordersAndInitialConditions_ACU_1->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_ACU_1->removeColumn(uiMain->tableBordersAndInitialConditions_ACU_1->currentColumn()-1);

            uiMain->tableBordersAndInitialConditions_ACU_2->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_ACU_2->removeColumn(uiMain->tableBordersAndInitialConditions_ACU_2->currentColumn()-1);

            uiMain->tableBordersAndInitialConditions_ACU_3->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_ACU_3->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_ACU_3->removeColumn(uiMain->tableBordersAndInitialConditions_ACU_3->currentColumn()-1);

            uiMain->tableBordersAndInitialConditions_ACU_4->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_ACU_4->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_ACU_4->removeColumn(uiMain->tableBordersAndInitialConditions_ACU_4->currentColumn()-1);
        }

        dh = dRC/( static_cast <double> (countSpacePoints) );
        uiMain->selectStepH->setText(QString::number(dh));
    }
}

void MainWindow::on_spaceParametrEVAP_valueChanged(int countSpacePoints)
{
    if(uiMain->EVAP->isChecked())
    {
        if((uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1) <= countSpacePoints)
        {
            uiMain->tableBordersAndInitialConditions_EVAP_1->insertColumn(uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_EVAP_2->insertColumn(uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_EVAP_3->insertColumn(uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_EVAP_4->insertColumn(uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1);
        }
        else
        {
            uiMain->tableBordersAndInitialConditions_EVAP_1->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_EVAP_1->removeColumn(uiMain->tableBordersAndInitialConditions_EVAP_1->currentColumn()-1);

            uiMain->tableBordersAndInitialConditions_EVAP_2->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_EVAP_2->removeColumn(uiMain->tableBordersAndInitialConditions_EVAP_2->currentColumn()-1);

            uiMain->tableBordersAndInitialConditions_EVAP_3->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_EVAP_3->removeColumn(uiMain->tableBordersAndInitialConditions_EVAP_3->currentColumn()-1);

            uiMain->tableBordersAndInitialConditions_EVAP_4->setCurrentCell(0, uiMain->tableBordersAndInitialConditions_EVAP_4->columnCount()-1);
            uiMain->tableBordersAndInitialConditions_EVAP_4->removeColumn(uiMain->tableBordersAndInitialConditions_EVAP_4->currentColumn()-1);
        }

        dh = dRC/( static_cast <double> (countSpacePoints) );
        uiMain->selectStepH->setText(QString::number(dh));
    }
}
