#include "ui_checkwindow.h"
#include "checkwindow.h"
#include "logger.h"

#include <iostream>

using std::cout;
using std::endl;


CheckWindow::CheckWindow(QWidget *parent) :
    QDialog(parent),
    uiCheck(new Ui::CheckWindow)
{
    uiCheck->setupUi(this);

    passError = 0.05;                                   // Default passing error = 5%; 5% / 100% = 0.05
    uiCheck->passErrorSpinBox->setValue(passError);
    uiCheck->passErrorSpinBox->setSingleStep(0.01);
    uiCheck->passErrorSpinBox->setRange(0.0, 1.0);      // Set the minimum and maximum values
}

CheckWindow::~CheckWindow()
{
    delete uiCheck;
}

void CheckWindow::on_toCheckButton_clicked()
{
    cout << endl << "Checking steady state..." << endl;

    passError = uiCheck->passErrorSpinBox->value();  

    // Get measurement values
    std::vector<double> listFirstMeasurementState, listSecondMeasurementState, listThirdMeasurementState;

    for(uint i = 0; i < uint(firstState->count()); ++i)
    {
        listFirstMeasurementState.push_back(firstState->itemText(int(i)).toDouble());
        listSecondMeasurementState.push_back(secondState->itemText(int(i)).toDouble());
        listThirdMeasurementState.push_back(thirdState->itemText(int(i)).toDouble());
    }

    // Get true values
    std::vector<double> listFirstTrueState, listSecondTrueState, listThirdTrueState;

    if (( LVM_BP->isChecked() ) || ( NLVM_BP->isChecked() ) || ( EVM_BP->isChecked() ))
    {
        listFirstTrueState = {157.000, 153.900, 150.600};
        listSecondTrueState = {124.800, 129.000, 133.000};
    }

    if ( EFM_BP->isChecked() )
    {
        listFirstTrueState = {77.000, 74.000, 71.140};
        listSecondTrueState = {4.497, 2.524, 0.626};
    }

    if ( EVM_TP->isChecked() )
    {
        listFirstTrueState = {122, 97.4, 69.59};
        listSecondTrueState = {58.28, 86.23, 111.8};
    }

    if ( EFM_TP->isChecked() )
    {
        listFirstTrueState = {0.6646, 0.8024, 0.9853};
        listSecondTrueState = {0.8271, 0.6863, 0.5696};
    }

    if ( EVAP->isChecked() )
    {
        listFirstTrueState = {160.161, 154.220, 148.736, 143.673};
        listSecondTrueState = {164.482, 158.209, 152.418, 147.072};
        listThirdTrueState = {283.306, 267.896, 253.671, 240.540};
    }

    if ( ACU->isChecked() )
    {
        listFirstTrueState = {53.364, 39.552, 30.001};
        listSecondTrueState = {26.846, 21.992, 18.635};
    }

    // Checking
    std::vector<double> listFirstErrorState, listSecondErrorState, listThirdErrorState;

    for(uint i = 0; i < uint(firstState->count()); ++i)
    {
        listFirstErrorState.push_back(listFirstTrueState[i] * passError);
        listSecondErrorState.push_back(listSecondTrueState[i] * passError);


        if(listFirstErrorState[i] < abs(listFirstTrueState[i] - listFirstMeasurementState[i]))
        {
            QString message(QString(tr("Attention! Error has very big value - listFirstErrorState[%1] = %2\n")).arg(i).arg(listFirstErrorState[i]));
            cout << message.toStdString() << endl;
            Logger::log(message, LOG_LEVEL_INFO);
        }

        if(listSecondErrorState[i] < abs(listSecondTrueState[i] - listSecondMeasurementState[i]))
        {
            QString message(QString(tr("Attention! Error has very big value - listSecondErrorState[%1] = %2\n")).arg(i).arg(listSecondErrorState[i]));
            cout << message.toStdString() << endl;
            Logger::log(message, LOG_LEVEL_INFO);
        }

        if(thirdState->count() > 0)
        {
            listThirdErrorState.push_back(listThirdTrueState[i] * passError);

            if(listThirdErrorState[i] < abs(listThirdTrueState[i] - listThirdMeasurementState[i]))
            {
                QString message(QString(tr("Attention! Error has very big value - listThirdErrorState[%1] = %2\n")).arg(i).arg(listThirdErrorState[i]));
                cout << message.toStdString() << endl;
                Logger::log(message, LOG_LEVEL_INFO);
            }
        }

    }

}

void CheckWindow::on_toCancelButton_clicked()
{
    CheckWindow::reject();                          // Return to MainWindow
}

void CheckWindow::setParentToCheck(MainWindow *parentWindow)
{
    firstState = parentWindow->findChild<QComboBox *>("firstState");
    secondState = parentWindow->findChild<QComboBox *>("secondState");
    thirdState = parentWindow->findChild<QComboBox *>("thirdState");

    LVM_BP = parentWindow->findChild<QRadioButton *>("LVM_BP");
    NLVM_BP = parentWindow->findChild<QRadioButton *>("NLVM_BP");
    EVM_BP = parentWindow->findChild<QRadioButton *>("EVM_BP");
    EFM_BP = parentWindow->findChild<QRadioButton *>("EFM_BP");
    EVM_TP = parentWindow->findChild<QRadioButton *>("EVM_TP");
    EFM_TP = parentWindow->findChild<QRadioButton *>("EFM_TP");
    EVAP = parentWindow->findChild<QRadioButton *>("EVAP");
    ACU = parentWindow->findChild<QRadioButton *>("ACU");
}
