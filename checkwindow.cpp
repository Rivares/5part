#include "ui_checkwindow.h"
#include "checkwindow.h"


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
    passError = uiCheck->passErrorSpinBox->value();

    passError = transfer(passError);

    uiCheck->~CheckWindow();
}

void CheckWindow::on_toCancelButton_clicked()
{
    uiCheck->~CheckWindow();                            // Return to MainWindow
}

double CheckWindow::transfer(double passError)
{
    passError = uiCheck->passErrorSpinBox->value();

    return passError;
}
