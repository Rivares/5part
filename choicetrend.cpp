#include "ui_choicetrend.h"
#include "choicetrend.h"

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

static double rightXOfTrend, leftYOfTrend, rightYOfTrend;

choiceTrend::choiceTrend(QList<QString> allTrends, QWidget *parent) :
    QDialog(parent),
    gottenTrends(allTrends),
    uiTrend(new Ui::choiceTrend)
{
    uiTrend->setupUi(this);

    cout << "Gotten trends!" << endl;
    for(int i = 0; i < gottenTrends.size(); ++i)
    {
        uiTrend->widgetListTrends->addItem(gottenTrends.at(i));
    }

    rightXOfTrend = 1000.0;
    leftYOfTrend = 0.0;
    rightYOfTrend = 180.0;
}

choiceTrend::~choiceTrend()
{
    delete uiTrend;
}

void choiceTrend::on_widgetListTrends_itemSelectionChanged()
{
    QString bufList(uiTrend->widgetListTrends->currentItem()->text());

    cout << "Me choice! (" << bufList.toStdString() << ")" << endl;

    currentTrend = bufList;
}

QString choiceTrend::getTrend()
{
    return currentTrend;
}

QList<double> choiceTrend::getAxesOfTrend()
{
    axesOfTrend.append(rightXOfTrend);
    axesOfTrend.append(leftYOfTrend);
    axesOfTrend.append(rightYOfTrend);

    return axesOfTrend;
}

// Buttons
void choiceTrend::on_doneButton_clicked()
{
    choiceTrend::accept();  // Return to MainWindow
}

void choiceTrend::on_cancelButton_clicked()
{
    currentTrend = "0";
    choiceTrend::reject();  // Return to MainWindow
}


void choiceTrend::on_inputRightXOfTrend_textChanged(const QString &value)
{
    rightXOfTrend = value.toDouble();
}

void choiceTrend::on_inputLeftYOfTrend_textChanged(const QString &value)
{
    leftYOfTrend = value.toDouble();
}

void choiceTrend::on_inputRightYOfTrend_textChanged(const QString &value)
{
    rightYOfTrend = value.toDouble();
}
