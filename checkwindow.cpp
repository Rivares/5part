#include "ui_checkwindow.h"
#include "checkwindow.h"
#include "logger.h"

#include <iostream>
#include <QString>

using std::cout;
using std::endl;

void toCheck(uint count, double passError, std::vector<double> listMeasurementState, std::vector<double> listTrueState, uint8_t id);

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

        toCheck(static_cast <uint> (firstState->count()), passError, listFirstMeasurementState, listFirstTrueState, 0);
        toCheck(static_cast <uint> (secondState->count()), passError, listSecondMeasurementState, listSecondTrueState, 1);
    }

    if ( EFM_BP->isChecked() )
    {
        listFirstTrueState = {77.000, 74.000, 71.140};
        listSecondTrueState = {4.497, 2.524, 0.626};

        toCheck(static_cast <uint> (firstState->count()), passError, listFirstMeasurementState, listFirstTrueState, 0);
        toCheck(static_cast <uint> (secondState->count()), passError, listSecondMeasurementState, listSecondTrueState, 1);
    }

    if ( EVM_TP->isChecked() )
    {
        listFirstTrueState = {122, 97.4, 69.59};
        listSecondTrueState = {58.28, 86.23, 111.8};

        toCheck(static_cast <uint> (firstState->count()), passError, listFirstMeasurementState, listFirstTrueState, 0);
        toCheck(static_cast <uint> (secondState->count()), passError, listSecondMeasurementState, listSecondTrueState, 1);
    }

    if ( EFM_TP->isChecked() )
    {
        listFirstTrueState = {0.6646, 0.8024, 0.9853};
        listSecondTrueState = {0.8271, 0.6863, 0.5696};

        toCheck(static_cast <uint> (firstState->count()), passError, listFirstMeasurementState, listFirstTrueState, 0);
        toCheck(static_cast <uint> (secondState->count()), passError, listSecondMeasurementState, listSecondTrueState, 1);
    }

    if ( EVAP->isChecked() )
    {
        listFirstTrueState = {160.161, 154.220, 148.736, 143.673};
        listSecondTrueState = {164.482, 158.209, 152.418, 147.072};
        listThirdTrueState = {283.306, 267.896, 253.671, 240.540};

        toCheck(static_cast <uint> (firstState->count()), passError, listFirstMeasurementState, listFirstTrueState, 0);
        toCheck(static_cast <uint> (secondState->count()), passError, listSecondMeasurementState, listSecondTrueState, 1);
        toCheck(static_cast <uint> (thirdState->count()), passError, listThirdMeasurementState, listThirdTrueState, 2);
    }

    if ( ACU->isChecked() )
    {
        listFirstTrueState = {53.364, 39.552, 30.001};
        listSecondTrueState = {26.846, 21.992, 18.635};

        toCheck(static_cast <uint> (firstState->count()), passError, listFirstMeasurementState, listFirstTrueState, 0);
        toCheck(static_cast <uint> (secondState->count()), passError, listSecondMeasurementState, listSecondTrueState, 1);
    }

    if ( TP_ACU->isChecked() )
    {
        listFirstTrueState = {122, 97, 69, 67.000, 66.000, 52, 39, 30};
        listSecondTrueState = {58, 86, 112};
        listThirdTrueState = {27, 23, 20};

        toCheck(static_cast <uint> (firstState->count()), passError, listFirstMeasurementState, listFirstTrueState, 0);
        toCheck(static_cast <uint> (secondState->count()), passError, listSecondMeasurementState, listSecondTrueState, 1);
        toCheck(static_cast <uint> (thirdState->count()), passError, listThirdMeasurementState, listThirdTrueState, 2);
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
    TP_ACU = parentWindow->findChild<QRadioButton *>("TP_ACU");
    BP_EVAP = parentWindow->findChild<QRadioButton *>("BP_EVAP");
    TP_BP = parentWindow->findChild<QRadioButton *>("TP_BP");
}

void toCheck(uint count, double passError, std::vector<double> listMeasurementState, std::vector<double> listTrueState, uint8_t id)
{
     std::vector<double> listErrorState;

    for(uint i = 0; i < count; ++i)
    {
        listErrorState.push_back(listTrueState[i] * passError);

        if(listErrorState[i] < abs(listTrueState[i] - listMeasurementState[i]))
        {
            QString message(QString("Attention! Error has very big value - list(%1)_[%2] = %3\n").arg(id).arg(i).arg(listErrorState[i]));
            cout << message.toStdString() << endl;
            Logger::log(message, LOG_LEVEL_INFO);
        }
    }
}
