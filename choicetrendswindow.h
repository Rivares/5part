#ifndef CHOICETRENDSWINDOW_H
#define CHOICETRENDSWINDOW_H

#include "mainwindow.h"

#include <QDialog>

namespace Ui {
class choiceTrendsWindow;
}

class choiceTrendsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit choiceTrendsWindow(QWidget *parent = nullptr);
    ~choiceTrendsWindow();

    void setParent(MainWindow *parentWindow);
    string getTrend();

private slots:
    void on_cancelButton_clicked();

    void on_TP_ACU_itemSelectionChanged();

    void on_BP_EVAP_itemSelectionChanged();

    void on_TP_BP_itemSelectionChanged();

    void on_FULL_RC_itemSelectionChanged();

    void on_doneButton_clicked();

private:
    Ui::choiceTrendsWindow *uiTrend;
    QRadioButton *LVM_BP;

    string currentTrend;
};

#endif // CHOICETRENDSWINDOW_H
