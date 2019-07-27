#include "ui_choicetrend.h"
#include "choicetrend.h"

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;


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

// Buttons
void choiceTrend::on_doneButton_clicked()
{
    choiceTrend::accept();  // Return to MainWindow
}

void choiceTrend::on_cancelButton_clicked()
{
    currentTrend.append("0");
    currentTrend.append("0");
    choiceTrend::reject();  // Return to MainWindow
}



