#ifndef CHECKWINDOW_H
#define CHECKWINDOW_H

#include <QDialog>

namespace Ui {
class CheckWindow;
}

class CheckWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CheckWindow(QWidget *parent = nullptr);
    ~CheckWindow();

    double transfer(double passError);

private slots:
    void on_toCheckButton_clicked();
    void on_toCancelButton_clicked();

private:
    Ui::CheckWindow *uiCheck;
    double passError;    // Default passing error = 5%; 5% / 100% = 0.05
};

#endif // CHECKWINDOW_H
