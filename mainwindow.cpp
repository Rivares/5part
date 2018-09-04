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

static bool okey = 0;
static double defaultH = 0.0000;
static unsigned int selectZ = 0;
static unsigned long long selectN = 0;
double dt = 0.0000;

static double dRC = 1.4;
const double initLayerTV_0 = 160.0000, initLayerTV_1 = 147.9999;
const double initLayerTF_0 = 120.3777, initLayerTF_1 = 132.3999;
const double initLayerCV_0 = 67.9444, initLayerCV_1 = 72.0444;
const double initLayerCF_0 = 6.5555, initLayerCF_1 = 2.7888;

//----------Petrtubation----------//    Temperature(min:???; maxHeat:???), gas flow or pressure differential?
static double P_TV = 0;
static double P_TF = 0;
static double P_CV = 0;
static double P_CF = 0;

void calculateMM(vector <vector <double> > &TV, vector <vector <double> > &TF);
void calculateMM(vector <vector <double> > &TV, vector <vector <double> > &TF,
                 bool turnOn);
void calculateMM(vector <vector <double> > &TV, vector <vector <double> > &TF,
                 vector <vector <double> > &CV, vector <vector <double> > &CF);

void initialLayerTV(vector <vector <double> > &TV);         // Would be relize as template
void initialLayerTF(vector <vector <double> > &TF);
void initialLayerCV(vector <vector <double> > &CV);
void initialLayerCF(vector <vector <double> > &CF);

void toFileMM(vector <vector <double> > MMM, string nameModel);

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
        clock_t timeMW_0 = clock();
        int msec = 0;

        // To initial array of functions
        MM[0] = &bTV;
        MM[1] = &bTF;
        MM[2] = &bCV;
        MM[3] = &bCF;

        ui->setupUi(this);

        ui->inputLeftX->setReadOnly(true);

        selectZ = static_cast <unsigned int> (ui->spaceParametr->value() + 2);
        selectN = ui->inputRightX->text().toULongLong(&okey, 10);
        defaultH = dRC/(selectZ-2);                                     // dRC(Height RC) = 1.4m

        ui->inputLeftY->clear();
        ui->inputRightY->clear();

        ui->inputLeftY->insert("100");
        ui->inputRightY->insert("180");

        leftY = ui->inputLeftY->text().toDouble();
        rightY = ui->inputRightY->text().toDouble();

        getData();
        drawGraph();

        clock_t timeDiff = clock() - timeMW_0;
        msec = timeDiff * 1000 / CLOCKS_PER_SEC;
        cout << "General time of program taken " << msec/1000 << " seconds, and " << msec%1000 <<" milliseconds!" << endl;
}


MainWindow::~MainWindow()
{
    delete ui;
}

double MainWindow::bTV(unsigned long long i, uint j)
{
    return TV[static_cast <size_t> (i)][j];
}

double MainWindow::bTF(unsigned long long i, uint j)
{
    return TF[static_cast <size_t> (i)][j];
}

double MainWindow::bCV(unsigned long long i, uint j)
{
    return CV[static_cast <size_t> (i)][j];
}

double MainWindow::bCF(unsigned long long i, uint j)
{
    return CF[static_cast <size_t> (i)][j];
}

void MainWindow::getData()
{
    leftX = ui->inputLeftX->text().toDouble();
    rightX = ui->inputRightX->text().toDouble();
    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();
    selectN = ui->inputRightX->text().toULongLong(&okey, 10);

    dt = (ui->selectStep->text().toDouble() <= 0.0)? 0.01 : abs(ui->selectStep->text().toDouble());
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

    if(ui->LVM->isChecked() || ui->NLVM->isChecked() || ui->EVM->isChecked())
        ui->customPlot->yAxis->setLabel("Tемпература, 'C");

    if(ui->EFM->isChecked())
        ui->customPlot->yAxis->setLabel("Концентрация, %");

    ui->customPlot->xAxis->setRange(0, (selectN / dt));
    ui->customPlot->yAxis->setRange(ui->inputLeftY->text().toDouble(), ui->inputRightY->text().toDouble());

    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    /*----------------------------------------------------------------------*/

    ui->statusBar->showMessage(QString("(!) Calculating the mathematical model... (!)"));

    if (ui->LVM->isChecked())
    {      
        calculateMM(TV, TF);
        ui->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));
        drawModel(0);
    }

    if (ui->NLVM->isChecked())
    {
        calculateMM(TV, TF, 1);                     // OVERLOADING CALCULATE OF FUNCTION
        ui->statusBar->showMessage(QString("(!) Drawing physical processes on the graph...... (!)"));
        drawModel(0);
    }

    if((ui->EVM->isChecked()) || (ui->EFM->isChecked()))
    {
        calculateMM(TV, TF, CV, CF);
        ui->statusBar->showMessage(QString("(!) Drawing physical processes on the graph... (!)"));

        if((ui->EVM->isChecked()))
            drawModel(0);
        else
            drawModel(2);
    }

    ui->statusBar->showMessage(QString("Ready!"));

    /*----------------------------------------------------------------------*/

    // Out to display steady-state value temperature
    QList <QString> listStatesVapor, listStatesFluid;
    if((ui->LVM->isChecked()) || (ui->NLVM->isChecked()) || (ui->EVM->isChecked()))
    {
        for(uint j = 1; j < selectZ-1; ++j)
        {
            listStatesVapor.append(QString::number(TV[ static_cast <size_t> ((selectN-1) / dt) ][j]));
            listStatesFluid.append(QString::number(TF[ static_cast <size_t> ((selectN-1) / dt) ][j]));
        }
    }

    // Out to display steady-state concentration of absorbent
    if(ui->EFM->isChecked())
    {
        for(uint j = 1; j < selectZ-1; ++j)
        {
            listStatesVapor.append(QString::number(CV[ static_cast <size_t> ((selectN-1) / dt) ][j]));
            listStatesFluid.append(QString::number(CF[ static_cast <size_t> ((selectN-1) / dt) ][j]));
        }
    }
    QStringList vaporStates(listStatesVapor);
    QStringList fluidStates(listStatesFluid);

    ui->vaporState->addItems(vaporStates);
    ui->fluidState->addItems(fluidStates);

    clock_t timeDiff = clock() - timeMW_1;
    msec = timeDiff * 1000 / CLOCKS_PER_SEC;
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
    QVector <QVector <double>> TV_mat, TF_mat, CV_mat, CF_mat;

    // Convert from std::vector <std::vector <double>> to QVector <QVector <double>>
    for(uint i = 0; i < selectZ; ++i)
    {
        QVector <double> tmpVectorTV, tmpVectorTF, tmpVectorCV, tmpVectorCF;

        for(uint j = 0; j < (selectN / dt); ++j)
        {
            tmpVectorTV.push_back(TV[j][i]);
            tmpVectorTF.push_back(TF[j][i]);

            if(choiceModel == 2)
            {
                tmpVectorCV.push_back(CV[j][i]);
                tmpVectorCF.push_back(CF[j][i]);
            }

            t[j] = j;
        }

        TV_mat.push_back(tmpVectorTV);
        TF_mat.push_back(tmpVectorTF);

        if(choiceModel == 2)
        {
            CV_mat.push_back(tmpVectorCV);
            CF_mat.push_back(tmpVectorCF);
        }
    }

    /*-------------Choice drawing processes------------------*/

    QVector <QVector <double>> drawingProcces_0, drawingProcces_1;

    drawingProcces_0 = TV_mat;
    drawingProcces_1 = TF_mat;

    if(choiceModel == 2)
    {
        drawingProcces_0 = CV_mat;
        drawingProcces_1 = CF_mat;
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
    for(uint j = 0; j < (selectZ-2) * 2; ++j)
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
    msec = timeDiff * 1000 / CLOCKS_PER_SEC;
    cout << endl <<"(QCustomPlot) Drawing time of program taken " << msec/1000 << " seconds, and " << msec%1000 <<" milliseconds!" << endl;
}

void MainWindow::on_save_clicked()
{
    if(ui->EVM->isChecked() || ui->EFM->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, ref(TV), "TV");
        std::thread threadToFileTFMM(toFileMM, ref(TF), "TF");
        std::thread threadToFileCVMM(toFileMM, ref(CV), "CV");
        std::thread threadToFileCFMM(toFileMM, ref(CF), "CF");

        threadToFileTVMM.join();
        threadToFileTFMM.join();
        threadToFileCVMM.join();
        threadToFileCFMM.join();
    }

    if(ui->LVM->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, ref(TV), "TV");
        std::thread threadToFileTFMM(toFileMM, ref(TF), "TF");

        threadToFileTVMM.join();
        threadToFileTFMM.join();
    }

    if(ui->NLVM->isChecked())
    {
        std::thread threadToFileTVMM(toFileMM, ref(TV), "NTV");
        std::thread threadToFileTFMM(toFileMM, ref(TF), "NTF");

        threadToFileTVMM.join();
        threadToFileTFMM.join();
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

void MainWindow::on_LVM_clicked()
{
    ui->valuePetrubationCVM->hide();
    ui->valuePetrubationCFM->hide();

    ui->inputLeftY->clear();
    ui->inputRightY->clear();

    ui->inputLeftY->insert("100");
    ui->inputRightY->insert("170");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();
}

void MainWindow::on_NLVM_clicked()
{
    ui->valuePetrubationCVM->hide();
    ui->valuePetrubationCFM->hide();

    ui->inputLeftY->clear();
    ui->inputRightY->clear();

    ui->inputLeftY->insert("100");
    ui->inputRightY->insert("170");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();
}

void MainWindow::on_EVM_clicked()
{
    ui->inputLeftY->clear();
    ui->inputRightY->clear();

    ui->valuePetrubationCVM->show();
    ui->valuePetrubationCFM->show();

    ui->inputLeftY->insert("100");
    ui->inputRightY->insert("170");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();
}

void MainWindow::on_EFM_clicked()
{
    ui->inputLeftY->clear();
    ui->inputRightY->clear();

    ui->valuePetrubationCVM->show();
    ui->valuePetrubationCFM->show();

    ui->inputLeftY->insert("0");
    ui->inputRightY->insert("90");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();
}

//--------------------------LINER MODEL!!!-----------------------------------
void calculateMM(vector <vector <double> > &TV, vector <vector <double> > &TF)
{
    double  RvT = 0.0191806, RfT = 0.0000777, PTV = (0.05654433 * dt) / defaultH,
            PTF = (0.0002291314 * dt) / defaultH;

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

    // Calculate model
    for(size_t i = 1; i < size_t(selectN / dt); ++i)   // n: t
    {
       for(uint j = 1; j < (selectZ-1); ++j)  // i: z
       {
           TV[i][j] = TV[i-1][j] * (-PTV + 1.0 - (dt * RvT)) + (PTV * TV[i-1][j-1]) + (dt * RvT * TF[i-1][(selectZ-1)-j]) + P_TV;
           TF[i][j] = TF[i-1][j] * (-PTF + 1.0 - (dt * RfT)) + (PTF * TF[i-1][j-1]) + (dt * RfT * TV[i-1][(selectZ-1)-j]) + P_TF;
       }
    }
    for(uint j = 1; j < (selectZ-1); ++j)  // i: z
    {
        cout << TV[size_t((selectN-1) / dt)][j] << " | ";
     }

    for(uint j = 1; j < (selectZ-1); ++j)  // i: z
    {
        cout << TF[size_t((selectN-1) / dt)][j] << " | ";
     }
}

//--------------------------NON-LINER MODEL!!!-----------------------------------
void calculateMM(vector <vector <double> > &TV, vector <vector <double> > &TF, bool turnOn) // turnOn is nonsense
{
    turnOn = (!turnOn);
    //double a0 = 0.0001152735759 or 0.00016966, What is TRUE?????????

    // -----Model's heat exchenger parameters------
    double RvT = 0.0191806, RfT = 0.0000777, a0 = 1.305007E-4,
            PTV_L = (a0 * 273.15 * dt) / defaultH,
            PTF = (0.0002291314 * dt) / defaultH,
            PTV_N = 0.0;

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

    // Calculate model
    for(size_t i = 1; i < size_t(selectN / dt); ++i)   // n: t
    {
       for(uint j = 1; j < (selectZ-1); ++j)  // i: z
       {
           PTV_N = (a0*TV[i-1][j] * dt) / defaultH;
           TV[i][j] = -TV[i-1][j] * (PTV_L - 1.0 - PTV_N + dt*RvT) + (PTV_L * TV[i-1][j-1]) - (PTV_N * TV[i-1][j+1])
                                                                 + (dt * RvT * TF[i-1][(selectZ-1)-j]) + P_TV;  // + perturbation of the temperature, the gas flow, the pressure differential
           TF[i][j] = -TF[i-1][j] * (PTF - 1.0 + dt*RfT) + (PTF * TF[i-1][j-1]) + (dt * RfT * TV[i-1][(selectZ-1)-j]) + P_TF;
        }
    }


   for(uint j = 1; j < (selectZ-1); ++j)  // i: z
   {
       cout << TV[size_t((selectN-1) / dt)][j] << " | ";
    }

   for(uint j = 1; j < (selectZ-1); ++j)  // i: z
   {
       cout << TF[size_t((selectN-1) / dt)][j] << " | ";
    }

}

//---------------------------INTERCONNECTED MODEL!!!------------------------------
void calculateMM(vector <vector <double> > &TV, vector <vector <double> > &TF,
                 vector <vector <double> > &CV, vector <vector <double> > &CF)
{
    // -----Model's heat exchenger parameters------
    double RvT = 0.0191806, RfT = 0.0000777, a0 = 1.305007E-4,
            PTV_L = (a0 * 273.15 * dt) / defaultH, PTV_N = 0.0, PTF = (0.0002291314 * dt) / defaultH;

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
    for(size_t j = 0; j < selectZ; ++j)
    {
        cout << TV[0][j] << " | ";
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
           PTV_N = (a0*TV[i-1][j] * dt) / defaultH;
           TV[i][j] = -TV[i-1][j] * (PTV_L - 1.0 - PTV_N + dt*RvT) + (PTV_L * TV[i-1][j-1]) - (PTV_N * TV[i-1][j+1])
                                                                 + (dt * RvT * TF[i-1][(selectZ-1)-j]) + P_TV;  // + perturbation of the temperature, the gas flow, the pressure differential
           TF[i][j] = -TF[i-1][j] * (PTF - 1.0 + dt*RfT) + (PTF * TF[i-1][j-1]) + (dt * RfT * TV[i-1][(selectZ-1)-j]) + P_TF;

           // -----Calculate layer mass exchenger model------
           // New schema: CV(i,i-1); CV(i,i-1). Error was increase..., but dynamic of process good
           CV[i][j] = -CV[i-1][j] * (PTV_L - 1.0 + PTV_N - dt*RvM) + (PTV_L + PTV_N) * CV[i-1][j-1] - (dt * RvM * E * CF[i-1][(selectZ-1)-j]) + P_CV;

           /*
            * Old schema: CV(i,i-1); CV(i+1,i)
            * CV[i][j] = -CV[i-1][j] * (PTV_L - 1 - PTV_N - dt*RvM) + (PTV_L * CV[i-1][j-1]) - (PTV_N * CV[i-1][j+1])
                                                                            - (dt * RvM * E * CF[i-1][(selectZ-1)-j]) + P_CV;
           */

           CF[i][j] = -CF[i-1][j] * (PTF - 1.0 - (dt*RfM*E)) + (PTF * CF[i-1][j-1]) - (dt * RfM * CV[i-1][(selectZ-1)-j]) + P_CF;
        }
    }

    for(uint j = 1; j < (selectZ-1); ++j)  // i: z
    {
        cout << TV[size_t((selectN-1) / dt)][j] << " | ";
     }

    for(uint j = 1; j < (selectZ-1); ++j)  // i: z
    {
        cout << TF[size_t((selectN-1) / dt)][j] << " | ";
     }

    for(uint j = 1; j < (selectZ-1); ++j)  // i: z
    {
        cout << CV[size_t((selectN-1) / dt)][j] << " | ";
     }

    for(uint j = 1; j < (selectZ-1); ++j)  // i: z
    {
        cout << CF[size_t((selectN-1) / dt)][j] << " | ";
     }
}

void initialLayerTV(vector <vector <double> > &TV)
{
    double *initLayerTV = new double [selectZ];
    double stepInit = 0.0;
    size_t i = 0;

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
}

void initialLayerTF(vector <vector <double> > &TF)
{
    double *initLayerTF = new double [selectZ];
    double stepInit = 0.0;
    size_t i = 0;

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
}

void initialLayerCV(vector <vector <double> > &CV)
{
    double *initLayerCV = new double [selectZ];
    double stepInit = 0.0;
    size_t i = 0;

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
}

void initialLayerCF(vector <vector <double> > &CF)
{
    double *initLayerCF = new double [selectZ];
    double stepInit = 0.0;
    size_t i = 0;

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

void MainWindow::on_spaceParametr_valueChanged(int countSpacePoints)
{
    selectZ = static_cast <unsigned int> (countSpacePoints + 2);
}

void MainWindow::on_valuePetrubationTVM_editingFinished()
{
    P_TV = ui->valuePetrubationTVM->text().toDouble();
}

void MainWindow::on_valuePetrubationTFM_editingFinished()
{
    P_TF = ui->valuePetrubationTFM->text().toDouble();
}

void MainWindow::on_valuePetrubationCVM_editingFinished()
{
   P_CV = ui->valuePetrubationCVM->text().toDouble();
}

void MainWindow::on_valuePetrubationCFM_editingFinished()
{
    P_CF = ui->valuePetrubationCFM->text().toDouble();
}
