#include "qcustomplot.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <vector>
#include <cmath>
#include <map>
#include <array>
#include <vector>
#include <thread>
#include <time.h>
#include <fstream>
#include <iostream>
#include <functional>   //ref()

using namespace std;

static bool okey = 0;
static bool notEmpty = false;
static double defaultH = 0.0;
static unsigned int selectZ = 0;
static unsigned long long selectN = 0;
double dt = 0.0;
static map <string, int> Sizes = {{ "windowSizeX", 540 },
                                     { "windowSizeY", 370 },
                                     { "axisX_x1", 0 },
                                     { "axisX_y1", 343 },
                                     { "axisX_x2", 540 },
                                     { "axisX_y2", 343 },
                                     { "axisY_x1", 28 },
                                     { "axisY_y1", 0 },
                                     { "axisY_x2", 28 },
                                     { "axisY_y2", 370 },
                                    };

static double dRC = 1.4;
const double initLayerTV_0 = 160, initLayerTV_1 = 147.99;
const double initLayerTF_0 = 120.37, initLayerTF_1 = 132.39;
const double initLayerCV_0 = 67.94, initLayerCV_1 = 72.04;
const double initLayerCF_0 = 6.5, initLayerCF_1 = 2.78;
static bool state = false;

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

void draw_Model(QPixmap *graph, int choiceModel);

void toFileMM(vector <vector <double> > MMM, string nameModel);

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
        clock_t timeMW_0 = clock();
        Q_UNUSED(notEmpty);
        int msec = 0;

        // To initial array of functions
        MM[0] = &bTV;
        MM[1] = &bTF;
        MM[2] = &bCV;
        MM[3] = &bCF;

        ui->setupUi(this);

        ui->inputLeftX->setReadOnly(true);

        selectZ = static_cast <unsigned int> (ui->CouiceZspinBox->value() + 2);
        selectN = ui->inputRightX->text().toULongLong(&okey, 10);
        defaultH = dRC/(selectZ-2);                                     // dRC(Height RC) = 1.4m

        state = false;

        ui->inputLeftY->clear();
        ui->inputRightY->clear();

        ui->inputLeftY->insert("100");
        ui->inputRightY->insert("180");

        leftY = ui->inputLeftY->text().toDouble();
        rightY = ui->inputRightY->text().toDouble();

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


void MainWindow::recountPixels()
{
    onePixelX = (Sizes["windowSizeX"] - Sizes["axisY_x1"]) / (rightX-leftX);
    onePixelY = Sizes["axisX_y1"]  /(rightY - leftY);
    Ox = fabs(leftX + Sizes["axisY_x1"]); Oy = fabs(rightY);                // Nothing!!!
    ui->selectStep->setText(QString::fromStdString(std::to_string(dt)));
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

void MainWindow::drawGraph(bool notEmpty)
{
    clock_t timeMW_1 = clock();
    int msec = 0;

    /**********************************************************************/

    // configure right and top axis to show ticks but no labels:
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->yAxis2->setTickLabels(false);

    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("t, sec");
    ui->customPlot->yAxis->setLabel("T, C");

    ui->customPlot->xAxis->setRange(0, selectN);
    ui->customPlot->yAxis->setRange(ui->inputLeftY->text().toDouble(), ui->inputRightY->text().toDouble());

    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    /**********************************************************************/

    if(!notEmpty)
    {
        notEmpty = true;
        //ui->outputGraph->setPixmap(graph);
        return;
    }

    ui->progressBar_calculating->setRange(0, 1);

    if (ui->LHM->isChecked())
    {
        calculateMM(TV, TF);
        ui->progressBar_calculating->setValue(1);   // Fake
        draw_Model(0);
    }

    if (ui->NHM->isChecked())
    {
        calculateMM(TV, TF, 1);                     // OVERLOADING CALCULATE OF FUNCTION
        ui->progressBar_calculating->setValue(1);   // Fake
        draw_Model(0);
    }

    if((ui->Hexch->isChecked()) || (ui->Mexch->isChecked()))
    {
        calculateMM(TV, TF, CV, CF);
        ui->progressBar_calculating->setValue(1);   // Fake
        if((ui->Hexch->isChecked()))
            draw_Model(0);
        else
            draw_Model(2);
    }

    //----------------------------------------------------------------------

    // Out to display steady-state value temperature
    if((ui->Hexch->isChecked()) || (ui->LHM->isChecked())) {
        QString str = QString::number(TV[static_cast <size_t> (selectN-1)][1]);
        ui->valV1->setText(str);

        str = QString::number(TV[static_cast <size_t> (selectN-1)][(selectZ-1) / 2]);
        ui->valV2->setText(str);

        str = QString::number(TV[static_cast <size_t> (selectN-1)][selectZ-2]);
        ui->valV3->setText(str);
    //----------------------------------------------------------------------
        str = QString::number(TF[static_cast <size_t> (selectN-1)][1]);
        ui->valF1->setText(str);

        str = QString::number(TF[static_cast <size_t> (selectN-1)][(selectZ-1) / 2]);
        ui->valF2->setText(str);

        str = QString::number(TF[static_cast <size_t> (selectN-1)][selectZ-2]);
        ui->valF3->setText(str);
    }

    // Out to display steady-state concentration of absorbent
    if(ui->Mexch->isChecked()) {
        QString str = QString::number(CV[static_cast <size_t> (selectN-1)][1]);
        ui->valV1->setText(str);

        str = QString::number(CV[static_cast <size_t> (selectN-1)][(selectZ-1) / 2]);
        ui->valV2->setText(str);

        str = QString::number(CV[static_cast <size_t> (selectN-1)][selectZ-2]);
        ui->valV3->setText(str);
    //----------------------------------------------------------------------
        str = QString::number(CF[static_cast <size_t> (selectN-1)][1]);
        ui->valF1->setText(str);

        str = QString::number(CF[static_cast <size_t> (selectN-1)][(selectZ-1) / 2]);
        ui->valF2->setText(str);

        str = QString::number(CF[static_cast <size_t> (selectN-1)][selectZ-2]);
        ui->valF3->setText(str);
    }

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
    recountPixels();
    drawGraph();
}

void MainWindow::on_draw_clicked()
{
    getData();
    recountPixels();
    drawGraph(1);
}

void MainWindow::draw_Model(int choiceModel)
{
    //ui->progressBar_drawing->setRange(0, (selectN / dt));
    QVector <double> t((selectN / dt));
    QVector <QVector <double>> TV_mat, TF_mat, CV_mat, CF_mat;

    // Convert from std::vector <std::vector <double>> to QVector <QVector <double>>
    for(uint i = 0; i < selectZ; ++i)
    {
        QVector <double> tmpVectorTV, tmpVectorTF, tmpVectorCV, tmpVectorCF;

        for(uint j = 0; j < (selectN / dt); ++j)
        {
            //ui->progressBar_drawing->setValue(j+1);
            ui->statusBar->showMessage(QString("(!) Calculate procceses... (!)"));

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

    /******************************/

    srand(time(NULL));
    qreal widthPen = 2.2;
    int translucent = 35;

    for(uint j = 1; j < selectZ-1; ++j)
    {
        ui->customPlot->addGraph();
        int randColorR = rand() % 255,
            randColorG = rand() % 255,
            randColorB = rand() % 255;

        ui->customPlot->graph(j-1)->setPen(QPen(QColor  (randColorR,
                                                        randColorG,
                                                        randColorB), widthPen));
        ui->customPlot->graph(j-1)->setBrush(QBrush(QColor (randColorR,
                                                           randColorG,
                                                           randColorB, translucent) ));

        ui->customPlot->graph(j-1)->setData(t, TV_mat[j]);

        ui->customPlot->removeGraph(j-1);

        ui->customPlot->addGraph();
        randColorR = rand() % 255;
        randColorG = rand() % 255;
        randColorB = rand() % 255;

        ui->customPlot->graph(j-1)->setPen(QPen(QColor  (randColorR,
                                                        randColorG,
                                                        randColorB), widthPen));
        ui->customPlot->graph(j-1)->setBrush(QBrush(QColor (randColorR,
                                                           randColorG,
                                                           randColorB, translucent) ));

        ui->customPlot->graph(j-1)->setData(t, TF_mat[j]);
    }


    /******************************/

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->customPlot->replot();

    ui->statusBar->showMessage(QString("Ready"));
}

void MainWindow::on_save_clicked()
{
    if(ui->Hexch->isChecked() || ui->Mexch->isChecked())
    {
        toFileMM(TV, "TV");
        toFileMM(TF, "TF");
        toFileMM(CV, "CV");
        toFileMM(CF, "CF");
    }

    if(ui->LHM->isChecked())
    {
        toFileMM(TV, "TV");
        toFileMM(TF, "TF");
    }

    if(ui->NHM->isChecked())
    {
        toFileMM(TV, "NTV");
        toFileMM(TF, "NTF");
    }

   QTime time = QTime::currentTime();
   QDate date = QDate::currentDate();
   QString name;
   if(date.day()<10)
        name += "0";
    name += QString::number(date.day())+".";
    if(date.month()<10)
        name += "0";
    name += QString::number(date.month())+".";
    name += QString::number(date.year())+"_";
    if(time.hour()<10)
        name += "0";
    name += QString::number(time.hour())+"-";
    if(time.minute()<10)
        name += "0";
    name += QString::number(time.minute())+"-";
    if(time.second()<10)
        name += "0";
    name += QString::number(time.second());
    QFile file(name+".png");
    qDebug() << name;
    file.open(QIODevice::WriteOnly);

   QMessageBox msgBox;
   msgBox.setStandardButtons(QMessageBox::Ok);
/*
   if(ui->outputGraph->pixmap()->save(&file,"PNG"))
   {
        msgBox.setText("Saved to program folder with name: "+name+".png");
        msgBox.setWindowTitle("Saved!");
    }
    else
   {
        msgBox.setText("Error saving.");
        msgBox.setWindowTitle("Error!");
    }
    */
    msgBox.exec();

}

void MainWindow::on_LHM_clicked()
{
    ui->lineEdit_3->hide();
    ui->lineEdit_4->hide();

    ui->inputLeftY->clear();
    ui->inputRightY->clear();

    ui->inputLeftY->insert("100");
    ui->inputRightY->insert("170");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();

    ui->progressBar_calculating->setValue(0);   // Fake
    ui->progressBar_drawing->setValue(0);
}

void MainWindow::on_NHM_clicked()
{
    ui->lineEdit_3->hide();
    ui->lineEdit_4->hide();

    ui->inputLeftY->clear();
    ui->inputRightY->clear();

    ui->inputLeftY->insert("100");
    ui->inputRightY->insert("170");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();

    ui->progressBar_calculating->setValue(0);   // Fake
    ui->progressBar_drawing->setValue(0);
}

void MainWindow::on_Hexch_clicked()
{
    ui->inputLeftY->clear();
    ui->inputRightY->clear();

    ui->lineEdit_3->show();
    ui->lineEdit_4->show();

    ui->inputLeftY->insert("100");
    ui->inputRightY->insert("170");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();

    ui->progressBar_calculating->setValue(0);   // Fake
    ui->progressBar_drawing->setValue(0);
}

void MainWindow::on_Mexch_clicked()
{
    ui->inputLeftY->clear();
    ui->inputRightY->clear();

    ui->lineEdit_3->show();
    ui->lineEdit_4->show();

    ui->inputLeftY->insert("0");
    ui->inputRightY->insert("90");

    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();

    ui->progressBar_calculating->setValue(0);   // Fake
    ui->progressBar_drawing->setValue(0);
}

//--------------------------LINER MODEL!!!-----------------------------------
void calculateMM(vector <vector <double> > &TV, vector <vector <double> > &TF)
{
    double  RvT = 0.0191806, RfT = 0.0000777, PTV = (0.05654433 * dt) / defaultH,
            PTF = (0.0002291314 * dt) / defaultH;

    vector <double> bmp;
    bmp.assign(selectZ, 0);

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
           TV[i][j] = TV[i-1][j] * (-PTV + 1 - (dt * RvT)) + (PTV * TV[i-1][j-1]) + (dt * RvT * TF[i-1][(selectZ-1)-j]) + P_TV;
           TF[i][j] = TF[i-1][j] * (-PTF + 1 - (dt * RfT)) + (PTF * TF[i-1][j-1]) + (dt * RfT * TV[i-1][(selectZ-1)-j]) + P_TF;
       }
    }
}

//--------------------------NON-LINER MODEL!!!-----------------------------------
void calculateMM(vector <vector <double> > &TV, vector <vector <double> > &TF, bool turnOn) // turnOn is nonsense
{
    turnOn = (!turnOn);
    //double a0 = 0.0001152735759;  // a0-?????????True or False????

    // -----Model's heat exchenger parameters------
    double RvT = 0.0191806, RfT = 0.0000777, a0 = 0.00016966,
            PTV_L = (a0 * 273.15 * dt) / defaultH,
            PTF = (0.0002291314 * dt) / defaultH,
            PTV_2 = 0;

    vector <double> bmp;
    bmp.assign(selectZ, 0);

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
           PTV_2 = (a0*TV[i-1][j+1] * dt)/defaultH;
           TV[i][j] = (-TV[i-1][j]) * (PTV_L - 1 - PTV_2 + (dt * RvT)) + (PTV_L * TV[i-1][j-1]) - (PTV_2 * TV[i-1][j+1]) + (dt * RvT * TF[i-1][(selectZ-1)-j]) + P_TV;
           TF[i][j] = TF[i-1][j] * ((-PTF * dt) + 1 - (dt * RfT)) + ((PTF * dt) * TF[i-1][j-1]) + (dt * RfT * TV[i-1][(selectZ-1)-j]) + P_TF;
        }
    }

}

//---------------------------INTERCONNECTED MODEL!!!------------------------------
void calculateMM(vector <vector <double> > &TV, vector <vector <double> > &TF,
                 vector <vector <double> > &CV, vector <vector <double> > &CF)
{
    // -----Model's heat exchenger parameters------
    double RvT = 0.0191806, RfT = 0.0000777, a0 = 0.00016966,
            PTV_L = (a0 * 273.15 * dt) / defaultH, PTV_N = 0, PTF = (0.0002291314 * dt) / defaultH;

    // -----Model's mass exchenger parameters------
    double E = 0.000000001, //RvM = 0.004302, RfM = 0.00001222;
                            RfM = 0.000010734, RvM = 0.0216487318;

    vector <double> bmp;
    bmp.assign(selectZ, 0);

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
    for(size_t i = 1; i < size_t(selectN / dt); ++i)   // n: t
    {
       for(size_t j = 1; j < (selectZ-1); ++j)  // i: z
       {
           // -----Calculate layer heat exchenger model------
           PTV_N = (a0*TV[i-1][j+1] * dt) / defaultH;
           TV[i][j] = -TV[i-1][j] * (PTV_L - 1 - PTV_N + dt*RvT) + (PTV_L * TV[i-1][j-1]) - (PTV_N * TV[i-1][j+1])
                                                                 + (dt * RvT * TF[i-1][(selectZ-1)-j]) + P_TV;  // + perturbation of the temperature, the gas flow, the pressure differential
           TF[i][j] = -TF[i-1][j] * (PTF - 1 + dt*RfT) + (PTF * TF[i-1][j-1]) + (dt * RfT * TV[i-1][(selectZ-1)-j]) + P_TF;

           // -----Calculate layer mass exchenger model------
           CV[i][j] = -CV[i-1][j] * (PTV_L - 1 - PTV_N - dt*RvM) + (PTV_L * CV[i-1][j-1]) - (PTV_N * CV[i-1][j+1])
                                                                 - (dt * RvM * E * CF[i-1][(selectZ-1)-j]) + P_CV;
           CF[i][j] = -CF[i-1][j] * (PTF - 1 - (dt*RfM*E)) + (PTF * CF[i-1][j-1]) - (dt * RfM * CV[i-1][(selectZ-1)-j]) + P_CF;
        }
    }

    /******************************************************************/

    cout << "Set values:" << endl;
    for(uint j = 0; j < selectZ; j++)
    {
        cout << TV[size_t((selectN-1) / dt)][j] << " | ";
    }
    cout << endl;

    for(uint j = 0; j < selectZ; j++)
    {
        cout << TF[size_t((selectN-1) / dt)][j] << " | ";
    }
    cout << endl;

    for(uint j = 0; j < selectZ; j++)
    {
        cout << CV[size_t((selectN-1) / dt)][j] << " | ";
    }
    cout << endl;

    for(uint j = 0; j < selectZ; j++)
    {
        cout << CF[size_t((selectN-1) / dt)][j] << " | ";
    }
    cout << endl;

    /******************************************************************/

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

void MainWindow::toFileMM(vector <vector <double> > MMM, string nameModel)
{
    nameModel = "MM_" + nameModel + ".txt";

    ofstream foutMM(nameModel, ios_base::out | ios_base::trunc);

    for(size_t i = 0; i < (selectN / dt); i++)
    {
        for(uint j = 0; j < selectZ; j++)
        {
            foutMM << MMM[i][j] << " | ";
        }
        foutMM << endl;
    }
    foutMM.close();
}

void MainWindow::on_CouiceZspinBox_valueChanged(int valueChanged)
{
    selectZ = static_cast <unsigned int> (valueChanged + 2);
}

void MainWindow::on_lineEdit_editingFinished()
{
    P_TV = ui->lineEdit->text().toDouble();
}

void MainWindow::on_lineEdit_2_editingFinished()
{
    P_TF = ui->lineEdit_2->text().toDouble();
}

void MainWindow::on_lineEdit_3_editingFinished()
{
   P_CV = ui->lineEdit_3->text().toDouble();
}

void MainWindow::on_lineEdit_4_editingFinished()
{
    P_CF = ui->lineEdit_4->text().toDouble();
}
