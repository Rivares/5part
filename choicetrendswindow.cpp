#include "ui_choicetrendswindow.h"
#include "choicetrendswindow.h"


choiceTrendsWindow::choiceTrendsWindow(QWidget *parent) :
    QDialog(parent),
    uiTrend(new Ui::choiceTrendsWindow)
{
    uiTrend->setupUi(this);

    uiTrend->TP_ACU->addItem("TV; TF");
    uiTrend->TP_ACU->addItem("TB;");
    uiTrend->TP_ACU->addItem("CV; CF");
    uiTrend->BP_EVAP->addItem("TF;");
    uiTrend->BP_EVAP->addItem("TB; TFG");
    uiTrend->TP_BP->addItem("TV;");
    uiTrend->TP_BP->addItem("TF;");
    uiTrend->TP_BP->addItem("CV;");
    uiTrend->TP_BP->addItem("CF;");
    uiTrend->FULL_RC->addItem("TV;");
    uiTrend->FULL_RC->addItem("TF;");
    uiTrend->FULL_RC->addItem("CV;");
    uiTrend->FULL_RC->addItem("CF;");
    uiTrend->FULL_RC->addItem("TB1;");
    uiTrend->FULL_RC->addItem("TB2;");
    uiTrend->FULL_RC->addItem("TFG;");

}

choiceTrendsWindow::~choiceTrendsWindow()
{
    delete uiTrend;
}

void choiceTrendsWindow::setParent(MainWindow *parentWindow)
{
    LVM_BP = parentWindow->findChild<QRadioButton *>("LVM_BP");
}

void choiceTrendsWindow::on_cancelButton_clicked()
{
    choiceTrendsWindow::reject();  // Return to MainWindow
}

void choiceTrendsWindow::on_TP_ACU_itemSelectionChanged()
{
    currentTrend = "TP_ACU";
}

void choiceTrendsWindow::on_BP_EVAP_itemSelectionChanged()
{
    currentTrend = "TB_EVAP";
}

void choiceTrendsWindow::on_TP_BP_itemSelectionChanged()
{
    currentTrend = "TP_BP";
}

void choiceTrendsWindow::on_FULL_RC_itemSelectionChanged()
{
    currentTrend = "FULL_RC";
}

string choiceTrendsWindow::getTrend()
{
    return currentTrend;
}

void choiceTrendsWindow::on_doneButton_clicked()
{

}
