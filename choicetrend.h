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
    QList<double> getAxesOfTrend();

private slots:
    void on_widgetListTrends_itemSelectionChanged();

    void on_cancelButton_clicked();
    void on_doneButton_clicked();

    void on_inputRightXOfTrend_textChanged(const QString &value);
    void on_inputLeftYOfTrend_textChanged(const QString &value);
    void on_inputRightYOfTrend_textChanged(const QString &value);

private:
    QList<QString> gottenTrends;
    QString currentTrend;
    QList<double> axesOfTrend; /*rightXOfTrend, leftYOfTrend, rightYOfTrend;*/

    Ui::choiceTrend *uiTrend;
};

#endif // CHOICETREND_H
