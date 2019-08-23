#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include "choicetrend.h"
#include "checkwindow.h"
#include "initlayer.h"
#include "models.h"

#include "mainwindow.h"

#include <QMessageBox>
#include <Qt3DExtras>
#include <QString>
#include <QDebug>
#include <QUrl>
#include <cmath>
#include <array>
#include <vector>
#include <thread>
#include <time.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <functional>   //ref()


using std::vector;
using std::cout;
using std::endl;

static bool okey = false;

double dRC;    // Hieght nozzles
double dh;

uint32_t beginPoint;
uint32_t endPoint;


//-----------Initial layers-----------//
vector <double> initLayerTV;
vector <double> initLayerTF;
vector <double> initLayerCV;
vector <double> initLayerCF;
vector <double> initLayerTB_ACU;
vector <double> initLayerTB_EVAP;
vector <double> initLayerTFG;


//---------------Borders--------------//
double  initLayerTV_0, initLayerTV_1;
double  initLayerTF_0, initLayerTF_1;
double  initLayerCV_0, initLayerCV_1;
double  initLayerCF_0, initLayerCF_1;
double  initLayerTB_ACU_0, initLayerTB_ACU_1;
double  initLayerTB_EVAP_0, initLayerTB_EVAP_1;
double  initLayerTFG_0,initLayerTFG_1;


uint32_t spaceParametrBP;
uint32_t spaceParametrTP;
uint32_t spaceParametrACU;
uint32_t spaceParametrEVAP;

double dt;
unsigned long long selectN;




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


    //-------------------LINER MODEL & NON-LINER MODEL------------------------
    if(uiMain->LVM_BP->isChecked() || uiMain->NLVM_BP->isChecked())
    {
        spaceParametrBP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_BP_1->columnCount());

        initLayerTV_0 = 0.0; initLayerTV_1 = 0.0;
        initLayerTF_0 = 0.0; initLayerTF_1 = 0.0;

        initLayerTV_0 = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, 0)->text()).toDouble();
        initLayerTV_1 = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1)->text()).toDouble();

        initLayerTF_0 = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, 0)->text()).toDouble();
        initLayerTF_1 = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1)->text()).toDouble();

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
    //------------------------------------------------------------------------


    //----------------INTERCONNECTED MODEL(BOTTOM PART)-----------------------
    if( uiMain->EVM_BP->isChecked() || uiMain->EFM_BP->isChecked() )
    {
        spaceParametrBP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_BP_1->columnCount());

        initLayerTV_0 = 0.0; initLayerTV_1 = 0.0;
        initLayerTF_0 = 0.0; initLayerTF_1 = 0.0;
        initLayerCV_0 = 0.0; initLayerCV_1 = 0.0;
        initLayerCF_0 = 0.0; initLayerCF_1 = 0.0;

        initLayerTV_0 = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, 0)->text()).toDouble();
        initLayerTV_1 = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1)->text()).toDouble();

        initLayerTF_0 = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, 0)->text()).toDouble();
        initLayerTF_1 = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, uiMain->tableBordersAndInitialConditions_BP_2->columnCount()-1)->text()).toDouble();

        initLayerCV_0 = (uiMain->tableBordersAndInitialConditions_BP_3->item(0, 0)->text()).toDouble();
        initLayerCV_1 = (uiMain->tableBordersAndInitialConditions_BP_3->item(0, uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1)->text()).toDouble();

        initLayerCF_0 = (uiMain->tableBordersAndInitialConditions_BP_4->item(0, 0)->text()).toDouble();
        initLayerCF_1 = (uiMain->tableBordersAndInitialConditions_BP_4->item(0, uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1)->text()).toDouble();

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
    //------------------------------------------------------------------------


    //-----------------INTERCONNECTED MODEL(TOP PART)-------------------------
    if( uiMain->EVM_TP->isChecked() || uiMain->EFM_TP->isChecked() )
    {
        spaceParametrTP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_TP_1->columnCount());

        initLayerTV_0 = 0.0; initLayerTV_1 = 0.0;
        initLayerTF_0 = 0.0; initLayerTF_1 = 0.0;
        initLayerCV_0 = 0.0; initLayerCV_1 = 0.0;
        initLayerCF_0 = 0.0; initLayerCF_1 = 0.0;

        initLayerTV_0 = (uiMain->tableBordersAndInitialConditions_TP_1->item(0, 0)->text()).toDouble();
        initLayerTV_1 = (uiMain->tableBordersAndInitialConditions_TP_1->item(0, uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1)->text()).toDouble();

        initLayerTF_0 = (uiMain->tableBordersAndInitialConditions_TP_2->item(0, 0)->text()).toDouble();
        initLayerTF_1 = (uiMain->tableBordersAndInitialConditions_TP_2->item(0, uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1)->text()).toDouble();

        initLayerCV_0 = (uiMain->tableBordersAndInitialConditions_TP_3->item(0, 0)->text()).toDouble();
        initLayerCV_1 = (uiMain->tableBordersAndInitialConditions_TP_3->item(0, uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1)->text()).toDouble();

        initLayerCF_0 = (uiMain->tableBordersAndInitialConditions_TP_4->item(0, 0)->text()).toDouble();
        initLayerCF_1 = (uiMain->tableBordersAndInitialConditions_TP_4->item(0, uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1)->text()).toDouble();

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
    //------------------------------------------------------------------------


    //------------------AIR-COOLING UNIT MODEL(ACU)---------------------------
    if(uiMain->ACU->isChecked())
    {
        spaceParametrACU = static_cast <uint>(uiMain->tableBordersAndInitialConditions_ACU_1->columnCount());

        initLayerTV_0 = 0.0;
        initLayerTV_1 = 0.0;

        initLayerTV_0 = (uiMain->tableBordersAndInitialConditions_ACU_1->item(0, 0)->text()).toDouble();
        initLayerTV_1 = (uiMain->tableBordersAndInitialConditions_ACU_1->item(0, uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1)->text()).toDouble();

        initLayerTB_ACU_0 = (uiMain->tableBordersAndInitialConditions_ACU_2->item(0, 0)->text()).toDouble();
        initLayerTB_ACU_1 = (uiMain->tableBordersAndInitialConditions_ACU_2->item(0, uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1)->text()).toDouble();

        initLayerTV.assign((spaceParametrACU-2), 0.0);
        initLayerTB_ACU.assign((spaceParametrACU-2), 0.0);

        for(uint j = 1; j <= (spaceParametrACU-2); ++j)
        {
            initLayerTV.at(j-1) = (uiMain->tableBordersAndInitialConditions_ACU_1->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerTB_ACU.at(j-1) = (uiMain->tableBordersAndInitialConditions_ACU_2->item(0, static_cast <int>(j))->text()).toDouble();
        }

        dh = static_cast <double> (dRC / (spaceParametrACU-2));
    }
    //------------------------------------------------------------------------


    //---------------------EVAPORATOR MODEL(EVAP)-----------------------------
    if(uiMain->EVAP->isChecked())
    {
        spaceParametrEVAP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount());

        initLayerTF_0 = 0.0;
        initLayerTF_1 = 0.0;

        initLayerTF_0 = (uiMain->tableBordersAndInitialConditions_EVAP_1->item(0, 0)->text()).toDouble();
        initLayerTF_1 = (uiMain->tableBordersAndInitialConditions_EVAP_1->item(0, uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1)->text()).toDouble();

        initLayerTB_EVAP_0 = (uiMain->tableBordersAndInitialConditions_EVAP_2->item(0, 0)->text()).toDouble();
        initLayerTB_EVAP_1 = (uiMain->tableBordersAndInitialConditions_EVAP_2->item(0, uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1)->text()).toDouble();

        initLayerTFG_0 = (uiMain->tableBordersAndInitialConditions_EVAP_3->item(0, 0)->text()).toDouble();
        initLayerTFG_1 = (uiMain->tableBordersAndInitialConditions_EVAP_3->item(0, uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1)->text()).toDouble();

        initLayerTF.assign((spaceParametrEVAP-2), 0.0);
        initLayerTB_EVAP.assign((spaceParametrEVAP-2), 0.0);
        initLayerTFG.assign((spaceParametrEVAP-2), 0.0);

        for(uint j = 1; j <= (spaceParametrEVAP-2); ++j)
        {
            initLayerTF.at(j-1)  = (uiMain->tableBordersAndInitialConditions_EVAP_1->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerTB_EVAP.at(j-1)  = (uiMain->tableBordersAndInitialConditions_EVAP_2->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerTFG.at(j-1) = (uiMain->tableBordersAndInitialConditions_EVAP_3->item(0, static_cast <int>(j))->text()).toDouble();
        }

        dh = static_cast <double> (dRC / (spaceParametrEVAP-2));
    }
    //------------------------------------------------------------------------


    //-----INTERCONNECTED MODEL(TOP PART) + AIR-COOLING UNIT MODEL(ACU)-------
    if( (uiMain->TP_ACU->isChecked()) )
    {
        spaceParametrTP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_TP_1->columnCount());
        spaceParametrACU = static_cast <uint>(uiMain->tableBordersAndInitialConditions_ACU_1->columnCount());

        initLayerTV_0 = 0.0; initLayerTV_1 = 0.0;
        initLayerTF_0 = 0.0; initLayerTF_1 = 0.0;
        initLayerCV_0 = 0.0; initLayerCV_1 = 0.0;
        initLayerCF_0 = 0.0; initLayerCF_1 = 0.0;

        initLayerTV_0 = (uiMain->tableBordersAndInitialConditions_TP_1->item(0, 0)->text()).toDouble();

        initLayerTF_0 = (uiMain->tableBordersAndInitialConditions_TP_2->item(0, 0)->text()).toDouble();
        initLayerTF_1 = (uiMain->tableBordersAndInitialConditions_TP_2->item(0, uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1)->text()).toDouble();

        initLayerCV_0 = (uiMain->tableBordersAndInitialConditions_TP_3->item(0, 0)->text()).toDouble();
        initLayerCV_1 = (uiMain->tableBordersAndInitialConditions_TP_3->item(0, uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1)->text()).toDouble();

        initLayerCF_0 = (uiMain->tableBordersAndInitialConditions_TP_4->item(0, 0)->text()).toDouble();
        initLayerCF_1 = (uiMain->tableBordersAndInitialConditions_TP_4->item(0, uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1)->text()).toDouble();

        initLayerTV_1 = (uiMain->tableBordersAndInitialConditions_ACU_1->item(0, uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1)->text()).toDouble();

        initLayerTB_ACU_0 = (uiMain->tableBordersAndInitialConditions_ACU_2->item(0, 0)->text()).toDouble();
        initLayerTB_ACU_1 = (uiMain->tableBordersAndInitialConditions_ACU_2->item(0, uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1)->text()).toDouble();


        initLayerTV.assign((spaceParametrTP + spaceParametrACU - 4), 0.0);
        initLayerTF.assign((spaceParametrTP-2), 0.0);
        initLayerCV.assign((spaceParametrTP-2), 0.0);
        initLayerCF.assign((spaceParametrTP-2), 0.0);

        initLayerTB_ACU.assign((spaceParametrACU-2), 0.0);

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
            initLayerTB_ACU.at(j-1) = (uiMain->tableBordersAndInitialConditions_ACU_2->item(0, static_cast <int>(j))->text()).toDouble();
        }

        dh = static_cast <double> (dRC / (spaceParametrTP + spaceParametrACU - 4));  // dRC = dRC_TP + dRC_ACU; spaceParametr = spaceParametrTP + spaceParametrACU
    }
    //------------------------------------------------------------------------


    //---INTERCONNECTED MODEL(TOP PART) + INTERCONNECTED MODEL(BOTTOM PART)---
    if( (uiMain->TP_BP->isChecked()) )
    {
        spaceParametrTP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_TP_1->columnCount());
        spaceParametrBP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_BP_1->columnCount());

        initLayerTV_0 = 0.0; initLayerTV_1 = 0.0;
        initLayerTF_0 = 0.0; initLayerTF_1 = 0.0;
        initLayerCV_0 = 0.0; initLayerCV_1 = 0.0;
        initLayerCF_0 = 0.0; initLayerCF_1 = 0.0;

        initLayerTV_0 = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, 0)->text()).toDouble();
        initLayerTV_1 = (uiMain->tableBordersAndInitialConditions_TP_1->item(0, uiMain->tableBordersAndInitialConditions_TP_1->columnCount()-1)->text()).toDouble();

        initLayerTF_0 = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, 0)->text()).toDouble();
        initLayerTF_1 = (uiMain->tableBordersAndInitialConditions_TP_2->item(0, uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1)->text()).toDouble();

        initLayerCV_0 = (uiMain->tableBordersAndInitialConditions_BP_3->item(0, 0)->text()).toDouble();
        initLayerCV_1 = (uiMain->tableBordersAndInitialConditions_TP_3->item(0, uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1)->text()).toDouble();

        initLayerCF_0 = (uiMain->tableBordersAndInitialConditions_BP_4->item(0, 0)->text()).toDouble();
        initLayerCF_1 = (uiMain->tableBordersAndInitialConditions_TP_4->item(0, uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1)->text()).toDouble();


        initLayerTV.assign((spaceParametrTP + spaceParametrBP - 4), 0.0);
        initLayerTF.assign((spaceParametrTP + spaceParametrBP - 4), 0.0);
        initLayerCV.assign((spaceParametrTP + spaceParametrBP - 4), 0.0);
        initLayerCF.assign((spaceParametrTP + spaceParametrBP - 4), 0.0);


        for(uint j = 1; j <= (spaceParametrBP-2); ++j)
        {
            initLayerTV.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerTF.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerCV.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_3->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerCF.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_4->item(0, static_cast <int>(j))->text()).toDouble();
        }


        for(uint j = (spaceParametrBP-2); j < (spaceParametrTP + spaceParametrBP - 4); ++j)
        {
            initLayerTV.at(j) = (uiMain->tableBordersAndInitialConditions_TP_1->item(0, static_cast <int>(j-spaceParametrBP+3))->text()).toDouble();
            initLayerTF.at(j) = (uiMain->tableBordersAndInitialConditions_TP_2->item(0, static_cast <int>(j-spaceParametrBP+3))->text()).toDouble();
            initLayerCV.at(j) = (uiMain->tableBordersAndInitialConditions_TP_3->item(0, static_cast <int>(j-spaceParametrBP+3))->text()).toDouble();
            initLayerCF.at(j) = (uiMain->tableBordersAndInitialConditions_TP_4->item(0, static_cast <int>(j-spaceParametrBP+3))->text()).toDouble();
        }

        dh = static_cast <double> (dRC / (spaceParametrTP + spaceParametrBP - 4));  // dRC = dRC_TP + dRC_TP; spaceParametr = spaceParametrTP + spaceParametrBP
    }

    //------------------------------------------------------------------------


    //-------INTERCONNECTED MODEL(BOTTOM PART) + EVAPORATOR MODEL(EVAP)-------
    if( (uiMain->BP_EVAP->isChecked()) )
    {
        spaceParametrBP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_BP_1->columnCount());
        spaceParametrEVAP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount());

        initLayerTV_0 = 0.0; initLayerTV_1 = 0.0;
        initLayerTF_0 = 0.0; initLayerTF_1 = 0.0;
        initLayerCV_0 = 0.0; initLayerCV_1 = 0.0;
        initLayerCF_0 = 0.0; initLayerCF_1 = 0.0;

        initLayerTB_EVAP_0 = 0.0; initLayerTB_EVAP_1 = 0.0;
        initLayerTFG_0 = 0.0; initLayerTFG_1 = 0.0;

        initLayerTV_0 = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, 0)->text()).toDouble();

        initLayerTV_1 = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, uiMain->tableBordersAndInitialConditions_BP_1->columnCount()-1)->text()).toDouble();

        initLayerTF_0 = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, 0)->text()).toDouble();
        initLayerTF_1 = (uiMain->tableBordersAndInitialConditions_EVAP_1->item(0, uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1)->text()).toDouble();

        initLayerCV_0 = (uiMain->tableBordersAndInitialConditions_BP_3->item(0, 0)->text()).toDouble();
        initLayerCV_1 = (uiMain->tableBordersAndInitialConditions_BP_3->item(0, uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1)->text()).toDouble();

        initLayerCF_0 = (uiMain->tableBordersAndInitialConditions_BP_4->item(0, 0)->text()).toDouble();
        initLayerCF_1 = (uiMain->tableBordersAndInitialConditions_BP_4->item(0, uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1)->text()).toDouble();



        initLayerTB_EVAP_0 = (uiMain->tableBordersAndInitialConditions_EVAP_2->item(0, 0)->text()).toDouble();
        initLayerTB_EVAP_1 = (uiMain->tableBordersAndInitialConditions_EVAP_2->item(0, uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1)->text()).toDouble();

        initLayerTFG_0 = (uiMain->tableBordersAndInitialConditions_EVAP_3->item(0, 0)->text()).toDouble();
        initLayerTFG_1 = (uiMain->tableBordersAndInitialConditions_EVAP_3->item(0, uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1)->text()).toDouble();


        initLayerTV.assign((spaceParametrBP - 2), 0.0);
        initLayerTF.assign((spaceParametrBP + spaceParametrEVAP - 4), 0.0);
        initLayerCV.assign((spaceParametrBP - 2), 0.0);
        initLayerCF.assign((spaceParametrBP - 2), 0.0);

        initLayerTB_EVAP.assign((spaceParametrEVAP-2), 0.0);
        initLayerTFG.assign((spaceParametrEVAP-2), 0.0);

        for(uint j = 1; j <= (spaceParametrBP-2); ++j)
        {
            initLayerTV.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerTF.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerCV.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_3->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerCF.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_4->item(0, static_cast <int>(j))->text()).toDouble();
        }

        for(uint j = (spaceParametrBP-2); j < (spaceParametrBP + spaceParametrEVAP - 4); ++j)
        {
            initLayerTF.at(j) = (uiMain->tableBordersAndInitialConditions_EVAP_1->item(0, static_cast <int>(j-(spaceParametrBP-2)+1))->text()).toDouble();
        }

        for(uint j = 1; j <= (spaceParametrEVAP-2); ++j)
        {
            initLayerTB_EVAP.at(j-1) = (uiMain->tableBordersAndInitialConditions_EVAP_2->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerTFG.at(j-1) = (uiMain->tableBordersAndInitialConditions_EVAP_3->item(0, static_cast <int>(j))->text()).toDouble();
        }

        dh = static_cast <double> (dRC / (spaceParametrBP + spaceParametrEVAP- 4));  // dRC = dRC_TP + dRC_TP; spaceParametr = spaceParametrTP + spaceParametrBP
    }
    //------------------------------------------------------------------------


    //--------------------------------FULL RC---------------------------------
    if( (uiMain->FULL_RC->isChecked()) )
    {
        /*
        spaceParametrACU = static_cast <uint>(uiMain->tableBordersAndInitialConditions_ACU_1->columnCount());
        spaceParametrTP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_TP_1->columnCount());
        spaceParametrBP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_BP_1->columnCount());
        spaceParametrEVAP = static_cast <uint>(uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount());

        initLayerRC_ACU_TB_0 = 0.0, initLayerRC_ACU_TB_1 = 0.0;

        initLayerRC_BOT_TV_0 = 0.0, initLayerRC_ACU_TV_1 = 0.0;

        initLayerRC_TOP_TF_0 = 0.0, initLayerRC_TOP_TF_1 = 0.0;
        initLayerRC_BOT_TF_0 = 0.0, initLayerRC_EVAP_TF_1 = 0.0;

        initLayerRC_BOT_CV_0 = 0.0, initLayerRC_BOT_CV_1 = 0.0;
        initLayerRC_BOT_CF_0 = 0.0, initLayerRC_BOT_CF_1 = 0.0;
        initLayerRC_TOP_CV_0 = 0.0, initLayerRC_TOP_CV_1 = 0.0;
        initLayerRC_TOP_CF_0 = 0.0, initLayerRC_TOP_CF_1 = 0.0;

        initLayerRC_EVAP_TB_0 = 0.0, initLayerRC_EVAP_TB_1 = 0.0;
        initLayerRC_EVAP_TFG_0 = 0.0, initLayerRC_EVAP_TFG_1 = 0.0;


        initLayerRC_ACU_TB_0 = (uiMain->tableBordersAndInitialConditions_ACU_2->item(0, 0)->text()).toDouble();
        initLayerRC_ACU_TB_1 = (uiMain->tableBordersAndInitialConditions_ACU_2->item(0, uiMain->tableBordersAndInitialConditions_ACU_2->columnCount()-1)->text()).toDouble();

        initLayerRC_BOT_TV_0 = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, 0)->text()).toDouble();
        initLayerRC_ACU_TV_1 = (uiMain->tableBordersAndInitialConditions_ACU_1->item(0, uiMain->tableBordersAndInitialConditions_ACU_1->columnCount()-1)->text()).toDouble();

        initLayerRC_TOP_TF_0 = (uiMain->tableBordersAndInitialConditions_TP_2->item(0, 0)->text()).toDouble();
        initLayerRC_TOP_TF_1 = (uiMain->tableBordersAndInitialConditions_TP_2->item(0, uiMain->tableBordersAndInitialConditions_TP_2->columnCount()-1)->text()).toDouble();
        initLayerRC_BOT_TF_0 = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, 0)->text()).toDouble();;
        initLayerRC_EVAP_TF_1 = (uiMain->tableBordersAndInitialConditions_EVAP_1->item(0, uiMain->tableBordersAndInitialConditions_EVAP_1->columnCount()-1)->text()).toDouble();

        initLayerRC_BOT_CV_0 = (uiMain->tableBordersAndInitialConditions_BP_3->item(0, 0)->text()).toDouble();
        initLayerRC_BOT_CV_1 = (uiMain->tableBordersAndInitialConditions_BP_3->item(0, uiMain->tableBordersAndInitialConditions_BP_3->columnCount()-1)->text()).toDouble();
        initLayerRC_BOT_CF_0 = (uiMain->tableBordersAndInitialConditions_BP_4->item(0, 0)->text()).toDouble();
        initLayerRC_BOT_CF_1 = (uiMain->tableBordersAndInitialConditions_BP_4->item(0, uiMain->tableBordersAndInitialConditions_BP_4->columnCount()-1)->text()).toDouble();
        initLayerRC_TOP_CV_0 = (uiMain->tableBordersAndInitialConditions_TP_3->item(0, 0)->text()).toDouble();
        initLayerRC_TOP_CV_1 = (uiMain->tableBordersAndInitialConditions_TP_3->item(0, uiMain->tableBordersAndInitialConditions_TP_3->columnCount()-1)->text()).toDouble();
        initLayerRC_TOP_CF_0 = (uiMain->tableBordersAndInitialConditions_TP_4->item(0, 0)->text()).toDouble();
        initLayerRC_TOP_CF_1 = (uiMain->tableBordersAndInitialConditions_TP_4->item(0, uiMain->tableBordersAndInitialConditions_TP_4->columnCount()-1)->text()).toDouble();

        initLayerRC_EVAP_TB_0 = (uiMain->tableBordersAndInitialConditions_EVAP_2->item(0, 0)->text()).toDouble();
        initLayerRC_EVAP_TB_1 = (uiMain->tableBordersAndInitialConditions_EVAP_2->item(0, uiMain->tableBordersAndInitialConditions_EVAP_2->columnCount()-1)->text()).toDouble();
        initLayerRC_EVAP_TFG_0 = (uiMain->tableBordersAndInitialConditions_EVAP_3->item(0, 0)->text()).toDouble();
        initLayerRC_EVAP_TFG_1 = (uiMain->tableBordersAndInitialConditions_EVAP_3->item(0, uiMain->tableBordersAndInitialConditions_EVAP_3->columnCount()-1)->text()).toDouble();


        initLayerRC_ACU_TB.assign((spaceParametrACU - 2), 0.0);
        initLayerRC_BOT_TOP_ACU_TV.assign((spaceParametrBP + spaceParametrTP + spaceParametrACU - 6), 0.0);

        initLayerRC_TOP_TF.assign((spaceParametrTP - 2), 0.0);
        initLayerRC_BOT_EVAP_TF.assign((spaceParametrBP + spaceParametrEVAP - 4), 0.0);

        initLayerRC_BOT_CV.assign((spaceParametrBP - 2), 0.0);
        initLayerRC_BOT_CF.assign((spaceParametrBP - 2), 0.0);
        initLayerRC_TOP_CV.assign((spaceParametrTP - 2), 0.0);
        initLayerRC_TOP_CF.assign((spaceParametrTP - 2), 0.0);

        initLayerRC_EVAP_TB.assign((spaceParametrEVAP - 2), 0.0);
        initLayerRC_EVAP_TFG.assign((spaceParametrEVAP - 2), 0.0);

        // ACU
        for(uint j = 1; j <= (spaceParametrACU-2); ++j)
        {
            initLayerRC_ACU_TB.at(j-1) = (uiMain->tableBordersAndInitialConditions_ACU_2->item(0, static_cast <int>(j))->text()).toDouble();
        }

        // BP
        for(uint j = 1; j <= (spaceParametrBP-2); ++j)
        {
            initLayerRC_BOT_TOP_ACU_TV.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_1->item(0, static_cast <int>(j))->text()).toDouble();

            initLayerRC_BOT_EVAP_TF.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_2->item(0, static_cast <int>(j))->text()).toDouble();

            initLayerRC_BOT_CV.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_3->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerRC_BOT_CF.at(j-1) = (uiMain->tableBordersAndInitialConditions_BP_4->item(0, static_cast <int>(j))->text()).toDouble();
        }
        // TP
        for(uint j = 1; j <= (spaceParametrTP-2); ++j)
        {
            initLayerRC_TOP_TF.at(j-1) = (uiMain->tableBordersAndInitialConditions_TP_2->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerRC_TOP_CV.at(j-1) = (uiMain->tableBordersAndInitialConditions_TP_3->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerRC_TOP_CF.at(j-1) = (uiMain->tableBordersAndInitialConditions_TP_4->item(0, static_cast <int>(j))->text()).toDouble();
        }
        // EVAP
        for(uint j = 1; j <= (spaceParametrEVAP-2); ++j)
        {
            initLayerRC_EVAP_TB.at(j-1) = (uiMain->tableBordersAndInitialConditions_EVAP_2->item(0, static_cast <int>(j))->text()).toDouble();
            initLayerRC_EVAP_TFG.at(j-1) = (uiMain->tableBordersAndInitialConditions_EVAP_3->item(0, static_cast <int>(j))->text()).toDouble();
        }

        // BP->TP
        for(uint j = (spaceParametrBP-2); j < (spaceParametrBP + spaceParametrTP - 4); ++j)
        {
            initLayerRC_BOT_TOP_ACU_TV.at(j) = (uiMain->tableBordersAndInitialConditions_TP_1->item(0, static_cast <int>(j-(spaceParametrBP-2)+1))->text()).toDouble();
        }

        // TP->ACU
        for(uint j = (spaceParametrBP + spaceParametrTP - 4); j < (spaceParametrBP + spaceParametrTP + spaceParametrACU - 4); ++j)
        {
            initLayerRC_BOT_TOP_ACU_TV.at(j) = (uiMain->tableBordersAndInitialConditions_ACU_1->item(0, static_cast <int>(j-(spaceParametrBP-2)+1))->text()).toDouble();
        }

        // BP->EVAP
        for(uint j = (spaceParametrBP-2); j < (spaceParametrBP + spaceParametrEVAP - 4); ++j)
        {
            initLayerRC_BOT_EVAP_TF.at(j) = (uiMain->tableBordersAndInitialConditions_EVAP_1->item(0, static_cast <int>(j-(spaceParametrBP-2)+1))->text()).toDouble();
        }

        */
        dh = static_cast <double> (dRC / (spaceParametrACU + spaceParametrTP + spaceParametrBP + spaceParametrEVAP - 8));  // dRC = dRC_ACU + dRC_TP + ...;
    }
    //------------------------------------------------------------------------

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

    if(uiMain->LVM_BP->isChecked() || uiMain->NLVM_BP->isChecked() ||
       uiMain->EVM_BP->isChecked() || uiMain->EVM_TP->isChecked() ||
       uiMain->ACU->isChecked() || uiMain->EVAP->isChecked())
        uiMain->customPlot->yAxis->setLabel("Tемпература, 'C");

    if(uiMain->EFM_BP->isChecked() || uiMain->EFM_TP->isChecked())
        uiMain->customPlot->yAxis->setLabel("Концентрация, %");

    if( (!uiMain->TP_ACU->isChecked())
         && (!uiMain->TP_BP->isChecked())
         && (!uiMain->BP_EVAP->isChecked())
         && (!uiMain->FULL_RC->isChecked()) )
    {
        uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
        uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

        // make left and bottom axes always transfer their ranges to right and top axes:
        connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
        connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));
    }

    /*----------------------------------------------------------------------*/

    // Out to display steady-state value temperature
    QList <QString> listStatesFirst, listStatesSecond, listStatesThird;

    uiMain->statusBar->showMessage(QString("(!) Calculating the mathematical model... (!)"));

    //--------------------------LINER MODEL-----------------------------------

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
    //------------------------------------------------------------------------


    //------------------------NON-LINER MODEL---------------------------------
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
    //------------------------------------------------------------------------


    //----------------INTERCONNECTED MODEL(BOTTOM PART)-----------------------
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
    //------------------------------------------------------------------------


    //-----------------INTERCONNECTED MODEL(TOP PART)-------------------------
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
    //------------------------------------------------------------------------


    //------------------AIR-COOLING UNIT MODEL(ACU)---------------------------
    if((uiMain->ACU->isChecked()))
    {
        if(!ACU_MM(TV, TB_ACU))
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
                listStatesSecond.append(QString::number(TB_ACU.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
            }
        }
    }
    //------------------------------------------------------------------------


    //---------------------EVAPORATOR MODEL(EVAP)-----------------------------
    if((uiMain->EVAP->isChecked()))
    {
        if(!EVAP_MM(TF, TB_EVAP, TFG))
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
                listStatesSecond.append(QString::number(TB_EVAP.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
                listStatesThird.append(QString::number(TFG.at(static_cast <size_t> ((selectN / dt) - 1)).at(j)));
            }
        }
    }
    //------------------------------------------------------------------------

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
        msec = static_cast <int>( (timeDiff * 1000)/static_cast <double>(CLOCKS_PER_SEC) );
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

    double sizeListWithTrends = 0.0;
    QVector <double> t(static_cast <int>(selectN / dt));

    QVector <QVector <double>> M_mat0, M_mat1, M_mat2, M_mat3;

    // Define selectZ
    switch(choiceModel)
    {
        case 0:                                                                          // H_L_BP
        case 1:                                                                          // H_N_BP
        case 2:                                                                          // H_BP
        case 3:  sizeListWithTrends = spaceParametrBP-2;                         break;  // M_BP
        case 4:                                                                          // H_TP
        case 5:  sizeListWithTrends = spaceParametrTP-2;                         break;  // M_TP
        case 6:  sizeListWithTrends = spaceParametrACU-2;                        break;  // ACU
        case 7:  sizeListWithTrends = spaceParametrEVAP-2;                       break;  // EVAP
        case 8:  sizeListWithTrends = spaceParametrTP-2;                         break;  // H_TP_ACU_TV_TF
        case 9:  sizeListWithTrends = spaceParametrACU-2;                        break;  // H_TP_ACU_TB
        case 10: sizeListWithTrends = spaceParametrTP-2;                         break;  // M_TP_ACU_CV_CF
        case 11: sizeListWithTrends = spaceParametrBP-2;                         break;  // H_BP_EVAP_TF
        case 12: sizeListWithTrends = spaceParametrBP-2;                         break;  // H_BP_EVAP_TV
        case 13: sizeListWithTrends = spaceParametrBP-2;                         break;  // M_BP_EVAP_CV_CF
        case 14: sizeListWithTrends = spaceParametrEVAP-2;                       break;  // H_BP_EVAP_TB_TFG
        case 15:
        case 16:                                                                         // H_TP_BP
        case 17:
        case 18: sizeListWithTrends =  spaceParametrTP + spaceParametrBP-4;      break;  // M_TP_BP
        case 19:                                                                         // H_FULL_RC
        case 20:
        case 21: sizeListWithTrends =  spaceParametrTP
                              + spaceParametrACU
                              + spaceParametrBP
                              + spaceParametrEVAP
                              - 16;                                     break;           // M_FULL_RC
        default:
        {
            QMessageBox msgBox;
            msgBox.setText("Driwing error! Unknow type choiceModel!");
            msgBox.exec();
        }
    }

    // Convert from std::vector <std::vector <double>> to QVector <QVector <double>>
    for(uint i = 1; i <= sizeListWithTrends; ++i)
    {
        QVector <double> tmpVectorM0, tmpVectorM1, tmpVectorM2;

        for(uint j = 0; j < (static_cast <size_t>((selectN / dt))); ++j)
        {
            switch(choiceModel)
            {
                case 0:                                                                                         // H_L_BP
                case 1:                                                                                         // H_N_BP
                case 2:  tmpVectorM0.push_back(TV.at(j).at(i)); tmpVectorM1.push_back(TF.at(j).at(i)); break;   // H_BP
                case 3:  tmpVectorM0.push_back(CV.at(j).at(i)); tmpVectorM1.push_back(CF.at(j).at(i)); break;   // M_BP
                case 4:  tmpVectorM0.push_back(TV.at(j).at(i)); tmpVectorM1.push_back(TF.at(j).at(i)); break;   // H_TP
                case 5:  tmpVectorM0.push_back(CV.at(j).at(i)); tmpVectorM1.push_back(CF.at(j).at(i)); break;   // M_TP
                case 6:  tmpVectorM0.push_back(TV.at(j).at(i)); tmpVectorM1.push_back(TB_ACU.at(j).at(i)); break;   // ACU
                case 7:  tmpVectorM0.push_back(TF.at(j).at(i)); tmpVectorM1.push_back(TB_EVAP.at(j).at(i)); tmpVectorM2.push_back(TFG.at(j).at(i)); break; // EVAP
                case 8:  tmpVectorM0.push_back(TV.at(j).at(i)); tmpVectorM1.push_back(TF.at(j).at(i)); break;   // H_TP_ACU: TV; TF
                case 9:  tmpVectorM0.push_back(TB_ACU.at(j).at(i)); break;                                          // H_TP_ACU: TB;
                case 10: tmpVectorM0.push_back(CV.at(j).at(i)); tmpVectorM1.push_back(CF.at(j).at(i)); break;   // M_TP_ACU
                case 11: tmpVectorM0.push_back(TF.at(j).at(i)); break;                                          // H_BP_EVAP: TF;
                case 12: tmpVectorM0.push_back(TV.at(j).at(i)); break;                                          // H_BP_EVAP: TV;
                case 13: tmpVectorM0.push_back(CV.at(j).at(i)); tmpVectorM1.push_back(CF.at(j).at(i)); break;   // M_BP_EVAP: CV; CF
                case 14: tmpVectorM0.push_back(TB_EVAP.at(j).at(i)); tmpVectorM1.push_back(TFG.at(j).at(i));break;   // H_BP_EVAP: TB; TFG
                case 15: tmpVectorM0.push_back(TV.at(j).at(i)); break;  // H_TP_BP: TV
                case 16: tmpVectorM0.push_back(TF.at(j).at(i)); break;  // H_TP_BP: TF
                case 17: tmpVectorM0.push_back(CV.at(j).at(i)); break;  // M_TP_BP: CV
                case 18: tmpVectorM0.push_back(CF.at(j).at(i)); break;  // M_TP_BP: CF
                case 19: /*Problem! Need two TB[][]! */ break;            // H_FULL_RC
                case 20: tmpVectorM0.push_back(CV.at(j).at(i)); break;  // M_FULL_RC: CV
                case 21: tmpVectorM0.push_back(CF.at(j).at(i)); break;  // M_FULL_RC: CF
            }

            t[static_cast <int>(j)] = j;
        }

        switch(choiceModel)
        {
            case 0:                                                                         // H_L_BP
            case 1:                                                                         // H_N_BP
            case 2:                                                                         // H_BP
            case 3:                                                                         // M_BP
            case 4:                                                                         // H_TP
            case 5:                                                                         // M_TP
            case 6:  M_mat0.push_back(tmpVectorM0); M_mat1.push_back(tmpVectorM1); break;   // ACU
            case 7:  M_mat0.push_back(tmpVectorM0); M_mat1.push_back(tmpVectorM1); M_mat2.push_back(tmpVectorM2); break;  // EVAP
            case 8:  M_mat0.push_back(tmpVectorM0); M_mat1.push_back(tmpVectorM1); break;   // H_TP_ACU: TV; TF
            case 9:  M_mat0.push_back(tmpVectorM0); break;                                  // H_TP_ACU: TB
            case 10: M_mat0.push_back(tmpVectorM0); M_mat1.push_back(tmpVectorM1); break;   // M_TP_ACU
            case 11: M_mat0.push_back(tmpVectorM0); break;                                  // H_BP_EVAP: TF;
            case 12: M_mat0.push_back(tmpVectorM0); break;                                  // H_BP_EVAP: TV;
            case 13: M_mat0.push_back(tmpVectorM0); M_mat1.push_back(tmpVectorM1); break;   // M_BP_EVAP: CV; CF
            case 14: M_mat0.push_back(tmpVectorM0); M_mat1.push_back(tmpVectorM1); break;   // H_BP_EVAP: TB; TFG
            case 15: M_mat0.push_back(tmpVectorM0); break;                                  // H_TP_BP: TV
            case 16: M_mat0.push_back(tmpVectorM0); break;                                  // H_TP_BP: FV
            case 17: M_mat0.push_back(tmpVectorM0); break;                                  // M_TP_BP: CV
            case 18: M_mat0.push_back(tmpVectorM0); break;                                  // M_TP_BP: CF
            case 19: /*Problem! Need two TB[][]! */ break;                                  // H_FULL_RC
            case 20: M_mat0.push_back(tmpVectorM0); break;                                  // M_FULL_RC: CV
            case 21: M_mat0.push_back(tmpVectorM0); break;                                  // M_FULL_RC: CF
        }

    }

    // crutch...
    switch(choiceModel)
    {
        case 8:

        for(uint i = static_cast <uint> (sizeListWithTrends+1.0); i <= static_cast <uint> (sizeListWithTrends + spaceParametrACU-2); ++i)
        {
            QVector <double> tmpVectorM0;

            for(uint j = 0; j < (static_cast <size_t>((selectN / dt))); ++j)
            {
                tmpVectorM0.push_back(TV.at(j).at(i));   // H_TP_ACU: TV
            }

            M_mat0.push_back(tmpVectorM0);   // H_TP_ACU: TV
        }
        sizeListWithTrends = spaceParametrTP - 2 + spaceParametrACU - 2 + spaceParametrACU - 2;
        break;

        case 11:

        for(uint i = static_cast <uint> (sizeListWithTrends+1.0); i <= static_cast <uint> (sizeListWithTrends+ spaceParametrEVAP-2); ++i)
        {
            QVector <double> tmpVectorM0;

            for(uint j = 0; j < (static_cast <size_t>((selectN / dt))); ++j)
            {
                tmpVectorM0.push_back(TF.at(j).at(i));   // H_BP_EVAP: TF
            }

            M_mat0.push_back(tmpVectorM0);   // H_BP_EVAP: TF
        }
        sizeListWithTrends = spaceParametrBP - 2 + spaceParametrEVAP - 2 + spaceParametrEVAP - 2;

        break;
    }



    /*-------------Choice drawing processes------------------*/

    uint countModels = 0;
    QVector <QVector <double>> drawingProcces_0, drawingProcces_1, drawingProcces_2;

    switch(choiceModel)
    {
        case 0:                                                                                                 // H_L_BP: TV; TF
        case 1:                                                                                                 // H_N_BP: TV; TF
        case 2:                                                                                                 // H_BP: TV; TF
        case 3:                                                                                                 // M_BP: CV; CF
        case 4:                                                                                                 // H_TP: TV; TF
        case 5:                                                                                                 // M_TP: CV; CF
        case 6:  drawingProcces_0 = M_mat0; drawingProcces_1 = M_mat1; countModels = 2; break;                  // ACU: TV; TB
        case 7:  drawingProcces_0 = M_mat0; drawingProcces_1 = M_mat1; drawingProcces_2 = M_mat2; countModels = 3; break;  // EVAP: TF; TB; TFG
        case 8:  drawingProcces_0 = M_mat0; drawingProcces_1 = M_mat1; countModels = 2; break;                  // H_TP_ACU: TV; TF;
        case 9:  drawingProcces_0 = M_mat0; countModels = 1; break;                                             // H_TP_ACU: TB
        case 10: drawingProcces_0 = M_mat0; drawingProcces_1 = M_mat1; countModels = 2; break;                  // M_TP_ACU: CV; CF
        case 11: drawingProcces_0 = M_mat0; countModels = 1; break;                                                              // H_BP_EVAP: TF;
        case 12: drawingProcces_0 = M_mat0; countModels = 1; break;                                                              // H_BP_EVAP: TV;
        case 13: drawingProcces_0 = M_mat0; drawingProcces_1 = M_mat1; countModels = 2; break;                  // M_BP_EVAP: CV; CF
        case 14: drawingProcces_0 = M_mat0; drawingProcces_1 = M_mat1; countModels = 2; break;                  // H_BP_EVAP: TB; TFG
        case 15: drawingProcces_0 = M_mat0; countModels = 1; break;                                             // H_TP_BP: TV
        case 16: drawingProcces_0 = M_mat0; countModels = 1; break;                                             // H_TP_BP: TF
        case 17: drawingProcces_0 = M_mat0; countModels = 1; break;                                             // H_TP_BP: CV
        case 18: drawingProcces_0 = M_mat0; countModels = 1; break;                                             // H_TP_BP: CF
        case 19: /*Problem! Need two TB[][]! */ break;                                                          // H_FULL_RC
        case 20: drawingProcces_0 = M_mat0; countModels = 1; break;                                             // M_FULL_RC: CV
        case 21: drawingProcces_0 = M_mat0; countModels = 1; break;                                             // M_FULL_RC: CF
    }


    uiMain->customPlot->clearGraphs();
    uiMain->customPlot->clearItems();

    uint countTrends = 0;

    // crutch...
    switch(choiceModel)
    {
        case 8:  countTrends = static_cast <uint> (spaceParametrTP + spaceParametrACU + spaceParametrACU - 6); sizeListWithTrends = spaceParametrTP + spaceParametrACU - 4; break;
        case 11: countTrends = static_cast <uint> (spaceParametrBP + spaceParametrEVAP - 4); sizeListWithTrends = spaceParametrBP + spaceParametrEVAP - 4; break;
        default: { countTrends = static_cast <uint> (sizeListWithTrends * countModels); }
    }

    /*-------------Rendering graphics------------------*/

    uiMain->customPlot->legend->setVisible(true);

    srand(static_cast <uint>(time(nullptr)));
    int randColorR, randColorG, randColorB;

    QPen pen;
    qreal widthPen = 2.2;
    int translucent = 35;

    for(uint i = 0, j = 0, k = 0, counter = 1; i < countTrends; ++i, ++counter)
    {
        uiMain->customPlot->addGraph();

        randColorR = rand() % 255;
        randColorG = rand() % 255;
        randColorB = rand() % 255;

        pen.setColor(QColor(randColorR, randColorG, randColorB));

        if(counter <= sizeListWithTrends)
        {
            /*-------------Customize of pen to drawing first model------------------*/

            pen.setStyle(Qt::SolidLine);
            uiMain->customPlot->graph(static_cast <int>(i))->setName(QString(tr("First phase_%1")).arg(i));
        }
        else
            if(counter <= sizeListWithTrends * (countModels-1))
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

    // crutch...
    switch(choiceModel)
    {
        case 8:  countTrends = static_cast <uint> (spaceParametrTP + spaceParametrACU + spaceParametrACU - 6); sizeListWithTrends = spaceParametrTP + spaceParametrACU - 4; break;
        case 11: countTrends = static_cast <uint> (spaceParametrBP + spaceParametrEVAP - 4); sizeListWithTrends = spaceParametrBP + spaceParametrEVAP - 4; break;
        default: { countTrends = static_cast <uint> (sizeListWithTrends * countModels); }
    }

    for(uint i = 0, j = 0, k = 0, counter = 1; i < countTrends; ++i, ++counter)
    {
        if(counter <= sizeListWithTrends)
        {
            uiMain->customPlot->graph(static_cast <int>(i))->setData(t, drawingProcces_0.at(static_cast <int>(i)));
        }
        else
            if(counter <= sizeListWithTrends * (countModels-1))
            {
                uiMain->customPlot->graph(static_cast <int>(i))->setData(t, drawingProcces_2.at(static_cast <int>(k)));
                ++k;
            }
            else
            {
                uiMain->customPlot->graph(static_cast <int>(i))->setData(t, drawingProcces_1.at(static_cast <int>(j)));
                ++j;
            }
    }

    uiMain->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    uiMain->customPlot->replot();

    clock_t timeDiff = clock() - timeMW_Cust;
    msec = static_cast <int>( (timeDiff * 1000) / static_cast <double>(CLOCKS_PER_SEC) );
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
        std::thread threadToFileTB_ACUMM(toFileMM, std::ref(TB_ACU), "TB_ACU");

        threadToFileTVMM.join();
        threadToFileTB_ACUMM.join();
    }

    if(uiMain->EVAP->isChecked())
    {
        std::thread threadToFileTFMM(toFileMM, std::ref(TF), "TF_EVAP");
        std::thread threadToFileTB_EVAPMM(toFileMM, std::ref(TB_EVAP), "TB_EVAP");
        std::thread threadToFileTFGMM(toFileMM, std::ref(TFG), "TFG_EVAP");

        threadToFileTFMM.join();
        threadToFileTB_EVAPMM.join();
        threadToFileTFGMM.join();
    }

    if(uiMain->TP_ACU->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, std::ref(TV), "TV_TP_ACU");
        std::thread threadToFileTFMM(toFileMM, std::ref(TF), "TF_TP_ACU");
        std::thread threadToFileCVMM(toFileMM, std::ref(CV), "CV_TP_ACU");
        std::thread threadToFileCFMM(toFileMM, std::ref(CF), "CF_TP_ACU");

        std::thread threadToFileTB_ACUMM(toFileMM, std::ref(TB_ACU), "TB_TP_ACU");

        threadToFileTVMM.join();

        threadToFileTFMM.join();
        threadToFileCVMM.join();
        threadToFileCFMM.join();

        threadToFileTB_ACUMM.join();
    }

    if(uiMain->TP_BP->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, std::ref(TV), "TV_TP_BP");
        std::thread threadToFileTFMM(toFileMM, std::ref(TF), "TF_TP_BP");
        std::thread threadToFileCVMM(toFileMM, std::ref(CV), "CV_TP_BP");
        std::thread threadToFileCFMM(toFileMM, std::ref(CF), "CF_TP_BP");

        threadToFileTVMM.join();        
        threadToFileTFMM.join();
        threadToFileCVMM.join();
        threadToFileCFMM.join();
    }

    if(uiMain->BP_EVAP->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, std::ref(TV), "TV_BP_EVAP");
        std::thread threadToFileTFMM(toFileMM, std::ref(TF), "TF_BP_EVAP");
        std::thread threadToFileCVMM(toFileMM, std::ref(CV), "CV_BP_EVAP");
        std::thread threadToFileCFMM(toFileMM, std::ref(CF), "CF_BP_EVAP");

        std::thread threadToFileTB_EVAPMM(toFileMM, std::ref(TB_EVAP), "TB_BP_EVAP");
        std::thread threadToFileTFGMM(toFileMM, std::ref(TFG), "TFG_BP_EVAP");

        threadToFileTVMM.join();
        threadToFileCVMM.join();
        threadToFileCFMM.join();

        threadToFileTFMM.join();

        threadToFileTB_EVAPMM.join();
        threadToFileTFGMM.join();
    }

    if(uiMain->FULL_RC->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, std::ref(TV), "TV_FULL_RC");
        std::thread threadToFileTFMM(toFileMM, std::ref(TF), "TF_FULL_RC");
        std::thread threadToFileCVMM(toFileMM, std::ref(CV), "CV_FULL_RC");
        std::thread threadToFileCFMM(toFileMM, std::ref(CF), "CF_FULL_RC");

        std::thread threadToFileTB_ACUMM(toFileMM, std::ref(TB_ACU), "TB_FULL_RC");
        std::thread threadToFileTB_EVAPMM(toFileMM, std::ref(TB_EVAP), "TB_FULL_RC");

        std::thread threadToFileTFGMM(toFileMM, std::ref(TFG), "TFG_FULL_RC");

        threadToFileTVMM.join();
        threadToFileCVMM.join();
        threadToFileCFMM.join();

        threadToFileTFMM.join();

        threadToFileTB_ACUMM.join();
        threadToFileTB_EVAPMM.join();

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

    uiMain->firstState->clear();
    uiMain->secondState->clear();
    uiMain->thirdState->clear();

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

    //------------------------------------------------------------------------

    //-----INTERCONNECTED MODEL(TOP PART) + AIR-COOLING UNIT MODEL(ACU)-------

    // Out to display steady-state value temperature
    QList <QString> listStatesFirst, listStatesSecond;

    QString choicedTrend;
    QList<double> choicedAxesOfTrend;

    QList<QString> listTrends;
    listTrends.append("TV; TF");
    listTrends.append("TB;");
    listTrends.append("CV; CF");

    choiceTrend choiceTrendWindow(listTrends, this);

    if (choiceTrendWindow.exec() == QDialog::Accepted)
    {
        choicedTrend = choiceTrendWindow.getTrend();
        choicedAxesOfTrend = choiceTrendWindow.getAxesOfTrend();

        cout << "Your choice! (" << choicedTrend.toStdString()
             << ") Axes: "
             << choicedAxesOfTrend.at(0) << "; "
             << choicedAxesOfTrend.at(1) << "; "
             << choicedAxesOfTrend.at(2) << "; "
             << endl;
    }

    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    if(choicedTrend == "TV; TF")
    {
        uiMain->inputRightX->insert(QString::number(choicedAxesOfTrend.at(0)));
        uiMain->inputLeftY->insert(QString::number(choicedAxesOfTrend.at(1)));
        uiMain->inputRightY->insert(QString::number(choicedAxesOfTrend.at(2)));

        getData();

        uiMain->customPlot->yAxis->setLabel("Tемпература, 'C");

        uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
        uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

        // make left and bottom axes always transfer their ranges to right and top axes:
        connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
        connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));


        if(!TOP_ACU_MM(TV, TF, CV, CF, TB_ACU))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            uiMain->statusBar->showMessage(QString("(!) Drawing TV; TF processes on the graph... (!)"));
            drawModel(8);

            // Out to display steady-state value temperature
            for(uint j = 0; j < (spaceParametrTP + spaceParametrACU - 2); ++j)
            {
                listStatesFirst.append(QString::number(TV.at(TV.size()-1).at(j)));
            }

            for(uint j = 0; j < spaceParametrTP; ++j)
            {
                listStatesSecond.append(QString::number(TF.at(TF.size()-1).at(j)));
            }
        }
    }
    else
    {
        if(choicedTrend == "TB;")
        {
            uiMain->inputRightX->insert(QString::number(choicedAxesOfTrend.at(0)));
            uiMain->inputLeftY->insert(QString::number(choicedAxesOfTrend.at(1)));
            uiMain->inputRightY->insert(QString::number(choicedAxesOfTrend.at(2)));

            getData();

            uiMain->customPlot->yAxis->setLabel("Tемпература, 'C");

            uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
            uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

            // make left and bottom axes always transfer their ranges to right and top axes:
            connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
            connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));

            if(!TOP_ACU_MM(TV, TF, CV, CF, TB_ACU))
            {
                QMessageBox msgBox;
                msgBox.setText("Sampling error! Change dt!");
                msgBox.exec();
            }
            else
            {
                uiMain->statusBar->showMessage(QString("(!) Drawing TB on the graph... (!)"));
                drawModel(9);

                // Out to display steady-state value temperature
                for(uint j = 0; j < spaceParametrACU; ++j)
                {
                    listStatesFirst.append(QString::number(TB_ACU.at(TB_ACU.size()-1).at(j)));
                }
            }
        }
        else
        {
            if(choicedTrend == "CV; CF")
            {
                uiMain->inputRightX->insert(QString::number(choicedAxesOfTrend.at(0)));
                uiMain->inputLeftY->insert(QString::number(choicedAxesOfTrend.at(1)));
                uiMain->inputRightY->insert(QString::number(choicedAxesOfTrend.at(2)));

                getData();

                uiMain->customPlot->yAxis->setLabel("Концентрация, %");

                uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
                uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

                // make left and bottom axes always transfer their ranges to right and top axes:
                connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
                connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));

                if(!TOP_ACU_MM(TV, TF, CV, CF, TB_ACU))
                {
                    QMessageBox msgBox;
                    msgBox.setText("Sampling error! Change dt!");
                    msgBox.exec();
                }
                else
                {
                    uiMain->statusBar->showMessage(QString("(!) Drawing CV; CF on the graph... (!)"));
                    drawModel(10);

                    // Out to display steady-state value concentration
                    for(uint j = 0; j < spaceParametrTP; ++j)
                    {
                        listStatesFirst.append(QString::number(CV.at(CV.size()-1).at(j)));
                    }

                    for(uint j = 0; j < spaceParametrTP; ++j)
                    {
                        listStatesSecond.append(QString::number(CF.at(CF.size()-1).at(j)));
                    }
                }
            }
        }
    }

    //------------------------------------------------------------------------

    QStringList firstStates(listStatesFirst);
    QStringList secondStates(listStatesSecond);

    uiMain->firstState->addItems(firstStates);
    uiMain->secondState->addItems(secondStates);

}

void MainWindow::on_TP_BP_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->firstState->clear();
    uiMain->secondState->clear();
    uiMain->thirdState->clear();

    uiMain->selectDRC->setText(QString::number(4.910)); // (RC_TOP)3.51 + (RC_BOT)1.4
    uiMain->spaceParametrTP->setValue(3);
    uiMain->spaceParametrBP->setValue(3);

    // RC_TOP:
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


    // RC_BOT:
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


    // Clearing other tabs (ACU, EVAP)
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


    //---INTERCONNECTED MODEL(TOP PART) + INTERCONNECTED MODEL(BOTTOM PART)---

    // Out to display steady-state value temperature
    QList <QString> listStatesFirst;

    QString choicedTrend;
    QList<double> choicedAxesOfTrend;

    QList<QString> listTrends;
    listTrends.append("TV;");
    listTrends.append("TF;");
    listTrends.append("CV;");
    listTrends.append("CF;");

    choiceTrend choiceTrendWindow(listTrends, this);

    if (choiceTrendWindow.exec() == QDialog::Accepted)
    {
        choicedTrend = choiceTrendWindow.getTrend();
        choicedAxesOfTrend = choiceTrendWindow.getAxesOfTrend();

        cout << "Your choice! (" << choicedTrend.toStdString()
             << ") Axes: "
             << choicedAxesOfTrend.at(0) << "; "
             << choicedAxesOfTrend.at(1) << "; "
             << choicedAxesOfTrend.at(2) << "; "
             << endl;
    }

    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    if(choicedTrend == "TV;")
    {
        uiMain->inputRightX->insert(QString::number(choicedAxesOfTrend.at(0)));
        uiMain->inputLeftY->insert(QString::number(choicedAxesOfTrend.at(1)));
        uiMain->inputRightY->insert(QString::number(choicedAxesOfTrend.at(2)));

        getData();

        uiMain->customPlot->yAxis->setLabel("Tемпература, 'C");

        uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
        uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

        // make left and bottom axes always transfer their ranges to right and top axes:
        connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
        connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));

        if(!TOP_BOT_MM(TV, TF, CV, CF))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            uiMain->statusBar->showMessage(QString("(!) Drawing TV; processes on the graph... (!)"));
            drawModel(15);

            // Out to display steady-state value temperature
            for(uint j = 0; j < spaceParametrTP; ++j)
            {
                listStatesFirst.append(QString::number(TV.at(TV.size()-1).at(j)));
            }
        }

    }
    else
    {
        if(choicedTrend == "TF;")
        {
            uiMain->inputRightX->insert(QString::number(choicedAxesOfTrend.at(0)));
            uiMain->inputLeftY->insert(QString::number(choicedAxesOfTrend.at(1)));
            uiMain->inputRightY->insert(QString::number(choicedAxesOfTrend.at(2)));

            getData();

            uiMain->customPlot->yAxis->setLabel("Tемпература, 'C");

            uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
            uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

            // make left and bottom axes always transfer their ranges to right and top axes:
            connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
            connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));
            //............................................]

            if(!TOP_BOT_MM(TV, TF, CV, CF))
            {
                QMessageBox msgBox;
                msgBox.setText("Sampling error! Change dt!");
                msgBox.exec();
            }
            else
            {
                uiMain->statusBar->showMessage(QString("(!) Drawing TF; processes on the graph... (!)"));
                drawModel(16);

                // Out to display steady-state value temperature
                for(uint j = 0; j < spaceParametrTP; ++j)
                {
                    listStatesFirst.append(QString::number(TF.at(TF.size()-1).at(j)));
                }
            }
        }
        else
        {
            if(choicedTrend == "CV;")
            {
                uiMain->inputRightX->insert(QString::number(choicedAxesOfTrend.at(0)));
                uiMain->inputLeftY->insert(QString::number(choicedAxesOfTrend.at(1)));
                uiMain->inputRightY->insert(QString::number(choicedAxesOfTrend.at(2)));

                getData();

                uiMain->customPlot->yAxis->setLabel("Концентрация, %");

                uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
                uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

                // make left and bottom axes always transfer their ranges to right and top axes:
                connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
                connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));


                if(!TOP_BOT_MM(TV, TF, CV, CF))
                {
                    QMessageBox msgBox;
                    msgBox.setText("Sampling error! Change dt!");
                    msgBox.exec();
                }
                else
                {
                    uiMain->statusBar->showMessage(QString("(!) Drawing CV; processes on the graph... (!)"));
                    drawModel(17);

                    // Out to display steady-state value concentration
                    for(uint j = 0; j < spaceParametrTP; ++j)
                    {
                        listStatesFirst.append(QString::number(CV.at(CV.size()-1).at(j)));
                    }
                }
            }
            else
            {
                if(choicedTrend == "CF;")
                {
                    uiMain->inputRightX->insert(QString::number(choicedAxesOfTrend.at(0)));
                    uiMain->inputLeftY->insert(QString::number(choicedAxesOfTrend.at(1)));
                    uiMain->inputRightY->insert(QString::number(choicedAxesOfTrend.at(2)));

                    getData();

                    uiMain->customPlot->yAxis->setLabel("Концентрация, %");

                    uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
                    uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

                    // make left and bottom axes always transfer their ranges to right and top axes:
                    connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
                    connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));


                    if(!TOP_BOT_MM(TV, TF, CV, CF))
                    {
                        QMessageBox msgBox;
                        msgBox.setText("Sampling error! Change dt!");
                        msgBox.exec();
                    }
                    else
                    {
                        uiMain->statusBar->showMessage(QString("(!) Drawing CV; processes on the graph... (!)"));
                        drawModel(18);

                        // Out to display steady-state value concentration
                        for(uint j = 0; j < spaceParametrTP; ++j)
                        {
                            listStatesFirst.append(QString::number(CF.at(CF.size()-1).at(j)));
                        }
                    }
                }
            }
        }
    }

    //------------------------------------------------------------------------

    QStringList firstStates(listStatesFirst);

    uiMain->firstState->addItems(firstStates);
}

void MainWindow::on_BP_EVAP_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->firstState->clear();
    uiMain->secondState->clear();
    uiMain->thirdState->clear();

    uiMain->selectDRC->setText(QString::number(6.90)); // (RC_BOT)1.4 + (EVAP)5.5
    uiMain->spaceParametrBP->setValue(3);
    uiMain->spaceParametrEVAP->setValue(4);

    // RC_BOT:
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

    // EVAP:
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


    // Clearing other tabs (ACU, TP)
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


    //------------------------------------------------------------------------

    //-------INTERCONNECTED MODEL(BOTTOM PART) + EVAPORATOR MODEL(EVAP)-------

    // Out to display steady-state value temperature
    QList <QString> listStatesFirst, listStatesSecond;

    QString choicedTrend;
    QList<double> choicedAxesOfTrend;

    QList<QString> listTrends;
    listTrends.append("TF;");

    listTrends.append("TV;");
    listTrends.append("CV; CF");

    listTrends.append("TB; TFG");

    choiceTrend choiceTrendWindow(listTrends, this);

    if (choiceTrendWindow.exec() == QDialog::Accepted)
    {
        choicedTrend = choiceTrendWindow.getTrend();
        choicedAxesOfTrend = choiceTrendWindow.getAxesOfTrend();

        cout << "Your choice! (" << choicedTrend.toStdString()
             << ") Axes: "
             << choicedAxesOfTrend.at(0) << "; "
             << choicedAxesOfTrend.at(1) << "; "
             << choicedAxesOfTrend.at(2) << "; "
             << endl;
    }

    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    if(choicedTrend == "TF;")
    {
        uiMain->inputRightX->insert(QString::number(choicedAxesOfTrend.at(0)));
        uiMain->inputLeftY->insert(QString::number(choicedAxesOfTrend.at(1)));
        uiMain->inputRightY->insert(QString::number(choicedAxesOfTrend.at(2)));

        getData();

        uiMain->customPlot->yAxis->setLabel("Tемпература, 'C");

        uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
        uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

        // make left and bottom axes always transfer their ranges to right and top axes:
        connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
        connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));

        if(!BOT_EVAP_MM(TV, TF, CV, CF, TB_EVAP, TFG))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            uiMain->statusBar->showMessage(QString("(!) Drawing TF; processes on the graph... (!)"));
            drawModel(11);

            // Out to display steady-state value temperature
            for(uint j = 0; j < (spaceParametrBP + spaceParametrEVAP - 4); ++j)
            {
                listStatesFirst.append(QString::number(TF.at(TF.size() - 1).at(j)));
            }
        }
    }
    else
    {
        if(choicedTrend == "TV;")
        {
            uiMain->inputRightX->insert(QString::number(choicedAxesOfTrend.at(0)));
            uiMain->inputLeftY->insert(QString::number(choicedAxesOfTrend.at(1)));
            uiMain->inputRightY->insert(QString::number(choicedAxesOfTrend.at(2)));

            getData();

            uiMain->customPlot->yAxis->setLabel("Tемпература, 'C");

            uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
            uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

            // make left and bottom axes always transfer their ranges to right and top axes:
            connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
            connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));

            if(!BOT_EVAP_MM(TV, TF, CV, CF, TB_EVAP, TFG))
            {
                QMessageBox msgBox;
                msgBox.setText("Sampling error! Change dt!");
                msgBox.exec();
            }
            else
            {
                uiMain->statusBar->showMessage(QString("(!) Drawing TV; processes on the graph... (!)"));
                drawModel(12);

                // Out to display steady-state value temperature
                for(uint j = 0; j < spaceParametrBP; ++j)
                {
                    listStatesFirst.append(QString::number(TV.at(TV.size()-1).at(j)));
                }
            }
        }
        else
        {
            if(choicedTrend == "CV; CF")
            {
                uiMain->inputRightX->insert(QString::number(choicedAxesOfTrend.at(0)));
                uiMain->inputLeftY->insert(QString::number(choicedAxesOfTrend.at(1)));
                uiMain->inputRightY->insert(QString::number(choicedAxesOfTrend.at(2)));

                getData();

                uiMain->customPlot->yAxis->setLabel("Концентрация, %");

                uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
                uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

                // make left and bottom axes always transfer their ranges to right and top axes:
                connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
                connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));

                if(!BOT_EVAP_MM(TV, TF, CV, CF, TB_EVAP, TFG))
                {
                    QMessageBox msgBox;
                    msgBox.setText("Sampling error! Change dt!");
                    msgBox.exec();
                }
                else
                {
                    uiMain->statusBar->showMessage(QString("(!) Drawing CV; CF processes on the graph... (!)"));
                    drawModel(13);

                    // Out to display steady-state value concentration
                    for(uint j = 0; j < spaceParametrBP; ++j)
                    {
                        listStatesFirst.append(QString::number(CV.at(CV.size()-1).at(j)));
                    }
                    for(uint j = 0; j < spaceParametrBP; ++j)
                    {
                        listStatesSecond.append(QString::number(CF.at(CF.size()-1).at(j)));
                    }
                }
            }
            else
            {
                if(choicedTrend == "TB; TFG")
                {
                    uiMain->inputRightX->insert(QString::number(choicedAxesOfTrend.at(0)));
                    uiMain->inputLeftY->insert(QString::number(choicedAxesOfTrend.at(1)));
                    uiMain->inputRightY->insert(QString::number(choicedAxesOfTrend.at(2)));

                    getData();

                    uiMain->customPlot->yAxis->setLabel("Концентрация, %");

                    uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
                    uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

                    // make left and bottom axes always transfer their ranges to right and top axes:
                    connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
                    connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));

                    if(!BOT_EVAP_MM(TV, TF, CV, CF, TB_EVAP, TFG))
                    {
                        QMessageBox msgBox;
                        msgBox.setText("Sampling error! Change dt!");
                        msgBox.exec();
                    }
                    else
                    {
                        uiMain->statusBar->showMessage(QString("(!) Drawing TB; TFG processes on the graph... (!)"));
                        drawModel(14);

                        // Out to display steady-state value concentration
                        for(uint j = 0; j < spaceParametrEVAP; ++j)
                        {
                            listStatesFirst.append(QString::number(TB_EVAP.at(TB_EVAP.size()-1).at(j)));
                        }
                        for(uint j = 0; j < spaceParametrEVAP; ++j)
                        {
                            listStatesSecond.append(QString::number(TFG.at(TFG.size()-1).at(j)));
                        }
                    }
                }
            }
        }
    }

    //------------------------------------------------------------------------

    QStringList firstStates(listStatesFirst);
    QStringList secondStates(listStatesSecond);

    uiMain->firstState->addItems(firstStates);
    uiMain->secondState->addItems(secondStates);

}

void MainWindow::on_FULL_RC_clicked()
{
    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    uiMain->firstState->clear();
    uiMain->secondState->clear();
    uiMain->thirdState->clear();

    uiMain->selectDRC->setText(QString::number(14.410)); // (ACU)4 + (RC_TOP)3.51 + (RC_BOT)1.4 + (EVAP)5.5

    uiMain->spaceParametrACU->setValue(3);
    uiMain->spaceParametrTP->setValue(3);
    uiMain->spaceParametrBP->setValue(3);
    uiMain->spaceParametrEVAP->setValue(4);


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



    // RC_BOT:
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

    // EVAP:
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


    //------------------------------------------------------------------------

    //--------------------------------FULL RC---------------------------------

    // Out to display steady-state value temperature
    QList <QString> listStatesFirst, listStatesSecond, listStatesThird;

    QString choicedTrend;
    QList<double> choicedAxesOfTrend;

    QList<QString> listTrends;

    listTrends.append("ACU_TB;");

    listTrends.append("BOT_TOP_ACU_TV;");

    listTrends.append("TOP_TF;");
    listTrends.append("BOT_EVAP_TF;"); /***********/

    listTrends.append("BOT_CV;");
    listTrends.append("BOT_CF;");
    listTrends.append("TOP_CV;");
    listTrends.append("TOP_CF;");

    listTrends.append("EVAP_TB;");
    listTrends.append("EVAP_TFG;");

    choiceTrend choiceTrendWindow(listTrends, this);

    if (choiceTrendWindow.exec() == QDialog::Accepted)
    {
        choicedTrend = choiceTrendWindow.getTrend();
        choicedAxesOfTrend = choiceTrendWindow.getAxesOfTrend();

        cout << "Your choice! (" << choicedTrend.toStdString()
             << ") Axes: "
             << choicedAxesOfTrend.at(0) << "; "
             << choicedAxesOfTrend.at(1) << "; "
             << choicedAxesOfTrend.at(2) << "; "
             << endl;
    }

    uiMain->inputLeftY->clear();
    uiMain->inputRightY->clear();
    uiMain->inputRightX->clear();

    if(choicedTrend == "ACU_TB;")
    {
        uiMain->inputRightX->insert(QString::number(choicedAxesOfTrend.at(0)));
        uiMain->inputLeftY->insert(QString::number(choicedAxesOfTrend.at(1)));
        uiMain->inputRightY->insert(QString::number(choicedAxesOfTrend.at(2)));

        getData();

        uiMain->customPlot->yAxis->setLabel("Tемпература, 'C");

        uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
        uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

        // make left and bottom axes always transfer their ranges to right and top axes:
        connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
        connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));

        if(!FULL_RC_MM(TV, TF,
                       CV, CF,
                       TB_ACU, TB_EVAP,
                       TFG))
        {
            QMessageBox msgBox;
            msgBox.setText("Sampling error! Change dt!");
            msgBox.exec();
        }
        else
        {
            uiMain->statusBar->showMessage(QString("(!) Drawing ACU_TB; processes on the graph... (!)"));
            drawModel(11);

            // Out to display steady-state value temperature
            for(uint j = 0; j < (spaceParametrBP + spaceParametrEVAP - 4); ++j)
            {
                listStatesFirst.append(QString::number(TF.at(TF.size() - 1).at(j)));
            }
        }
    }
    else
    {
        if(choicedTrend == "BOT_TOP_ACU_TV;")
        {
            uiMain->inputRightX->insert(QString::number(choicedAxesOfTrend.at(0)));
            uiMain->inputLeftY->insert(QString::number(choicedAxesOfTrend.at(1)));
            uiMain->inputRightY->insert(QString::number(choicedAxesOfTrend.at(2)));

            getData();

            uiMain->customPlot->yAxis->setLabel("Tемпература, 'C");

            uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
            uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

            // make left and bottom axes always transfer their ranges to right and top axes:
            connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
            connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));

            if(!FULL_RC_MM(TV, TF,
                           CV, CF,
                           TB_ACU, TB_EVAP,
                           TFG))
            {
                QMessageBox msgBox;
                msgBox.setText("Sampling error! Change dt!");
                msgBox.exec();
            }
            else
            {
                uiMain->statusBar->showMessage(QString("(!) Drawing BOT_TOP_ACU_TV; processes on the graph... (!)"));
                drawModel(12);

                // Out to display steady-state value temperature
                for(uint j = 0; j < spaceParametrBP; ++j)
                {
                    listStatesSecond.append(QString::number(TV.at(TV.size()-1).at(j)));
                }
            }
        }
        else
        {
            if(choicedTrend == "TOP_TF;")
            {
                uiMain->inputRightX->insert(QString::number(choicedAxesOfTrend.at(0)));
                uiMain->inputLeftY->insert(QString::number(choicedAxesOfTrend.at(1)));
                uiMain->inputRightY->insert(QString::number(choicedAxesOfTrend.at(2)));

                getData();

                uiMain->customPlot->yAxis->setLabel("Tемпература, 'C");

                uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
                uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

                // make left and bottom axes always transfer their ranges to right and top axes:
                connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
                connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));

                if(!FULL_RC_MM(TV, TF,
                               CV, CF,
                               TB_ACU, TB_EVAP,
                               TFG))
                {
                    QMessageBox msgBox;
                    msgBox.setText("Sampling error! Change dt!");
                    msgBox.exec();
                }
                else
                {
                    uiMain->statusBar->showMessage(QString("(!) Drawing TOP_TF; processes on the graph... (!)"));
                    drawModel(13);

                    // Out to display steady-state value concentration
                    for(uint j = 0; j < spaceParametrBP; ++j)
                    {
                        listStatesFirst.append(QString::number(CV.at(CV.size()-1).at(j)));
                    }
                    for(uint j = 0; j < spaceParametrBP; ++j)
                    {
                        listStatesSecond.append(QString::number(CF.at(CF.size()-1).at(j)));
                    }
                }
            }
            else
            {
                if(choicedTrend == "BOT_EVAP_TF;")
                {
                    uiMain->inputRightX->insert(QString::number(choicedAxesOfTrend.at(0)));
                    uiMain->inputLeftY->insert(QString::number(choicedAxesOfTrend.at(1)));
                    uiMain->inputRightY->insert(QString::number(choicedAxesOfTrend.at(2)));

                    getData();

                    uiMain->customPlot->yAxis->setLabel("Tемпература, 'C");

                    uiMain->customPlot->xAxis->setRange(0, (selectN / dt));
                    uiMain->customPlot->yAxis->setRange(uiMain->inputLeftY->text().toDouble(), uiMain->inputRightY->text().toDouble());

                    // make left and bottom axes always transfer their ranges to right and top axes:
                    connect(uiMain->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->xAxis2, SLOT(setRange(QCPRange)));
                    connect(uiMain->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), uiMain->customPlot->yAxis2, SLOT(setRange(QCPRange)));

                    if(!FULL_RC_MM(TV, TF,
                                   CV, CF,
                                   TB_ACU, TB_EVAP,
                                   TFG))
                    {
                        QMessageBox msgBox;
                        msgBox.setText("Sampling error! Change dt!");
                        msgBox.exec();
                    }
                    else
                    {
                        uiMain->statusBar->showMessage(QString("(!) Drawing BOT_EVAP_TF; processes on the graph... (!)"));
                        drawModel(14);

                        // Out to display steady-state value concentration
                        for(uint j = 0; j < spaceParametrEVAP; ++j)
                        {//??????????????????????????????????????????????????????????????????????????????????
                            listStatesFirst.append(QString::number(TB_ACU.at(TB_ACU.size()-1).at(j)));
                        }
                        for(uint j = 0; j < spaceParametrEVAP; ++j)
                        {
                            listStatesSecond.append(QString::number(TFG.at(TFG.size()-1).at(j)));
                        }
                    }
                }
            }
        }
    }

    //------------------------------------------------------------------------

    QStringList firstStates(listStatesFirst);
    QStringList secondStates(listStatesSecond);
    QStringList thirdStates(listStatesThird);

    uiMain->firstState->addItems(firstStates);
    uiMain->secondState->addItems(secondStates);
    uiMain->thirdState->addItems(thirdStates);
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

    if(uiMain->TP_BP->isChecked())
    {
        spaceParametrTP = static_cast <uint> (uiMain->spaceParametrTP->value());
        spaceParametrBP = static_cast <uint> (uiMain->spaceParametrBP->value());
        dh = dRC / ( static_cast <double> (spaceParametrTP + spaceParametrBP) );
    }

    if(uiMain->BP_EVAP->isChecked())
    {
        spaceParametrBP = static_cast <uint> (uiMain->spaceParametrBP->value());
        spaceParametrEVAP = static_cast <uint> (uiMain->spaceParametrEVAP->value());
        dh = dRC / ( static_cast <double> (spaceParametrBP + spaceParametrEVAP) );
    }

    if(uiMain->FULL_RC->isChecked())
    {
        spaceParametrACU = static_cast <uint> (uiMain->spaceParametrACU->value());
        spaceParametrTP = static_cast <uint> (uiMain->spaceParametrTP->value());
        spaceParametrBP = static_cast <uint> (uiMain->spaceParametrBP->value());
        spaceParametrEVAP = static_cast <uint> (uiMain->spaceParametrEVAP->value());
        dh = dRC / ( static_cast <double> (spaceParametrACU + spaceParametrTP
                                                            + spaceParametrBP
                                                            + spaceParametrEVAP) );
    }


    uiMain->selectStepH->setText(QString::number(dh));
}

void MainWindow::on_actionAbout_rectification_column_triggered()
{
    QString pathToFile = "C:/Devel/5part-newInterface/TZ.pdf";
    QFile file(pathToFile);

    QUrl url = QUrl::fromLocalFile(QFileInfo(file).canonicalFilePath());
    QDesktopServices::openUrl(url);
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
    if ( ( !(TV.empty()) ) || ( !(CV.empty()) ) || ( !(TB_ACU.empty() )) || ( !(TB_EVAP.empty() )) || ( !(TFG.empty()) ) )
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
