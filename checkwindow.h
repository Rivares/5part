#ifndef CHECKWINDOW_H
#define CHECKWINDOW_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class CheckWindow;
}

class CheckWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CheckWindow(QWidget *parent = nullptr);
    ~CheckWindow();

    void setParentToCheck(MainWindow *parentWindow);

private slots:
    void on_toCheckButton_clicked();
    void on_toCancelButton_clicked();

private:
    Ui::CheckWindow *uiCheck;

    QComboBox *firstState;
    QComboBox *secondState;
    QComboBox *thirdState;

    QRadioButton *LVM_BP;
    QRadioButton *NLVM_BP;
    QRadioButton *EVM_BP;
    QRadioButton *EFM_BP;
    QRadioButton *EVM_TP;
    QRadioButton *EFM_TP;
    QRadioButton *EVAP;
    QRadioButton *ACU;
    QRadioButton *TP_ACU;
    QRadioButton *BP_EVAP;
    QRadioButton *TP_BP;

    double passError;    // Default passing error = 5%; 5% / 100% = 0.05
};

#endif // CHECKWINDOW_H
