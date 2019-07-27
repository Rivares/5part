#ifndef CHOICETREND_H
#define CHOICETREND_H

#include "mainwindow.h"

#include <QDialog>

namespace Ui {
class choiceTrend;
}

class choiceTrend : public QDialog
{
    Q_OBJECT

public:
    explicit choiceTrend(QList<QString> allTrends, QWidget *parent = nullptr);
    ~choiceTrend();

    QString getTrend();

private slots:
    void on_widgetListTrends_itemSelectionChanged();

    void on_cancelButton_clicked();
    void on_doneButton_clicked();

private:
    QList<QString> gottenTrends;
    QString currentTrend;

    Ui::choiceTrend *uiTrend;
};

#endif // CHOICETREND_H
