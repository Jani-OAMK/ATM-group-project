#ifndef KORTINVALINTAWINDOW_H
#define KORTINVALINTAWINDOW_H

#include <QMainWindow>

namespace Ui {
class KortinValintaWindow;
}

class KortinValintaWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit KortinValintaWindow(QWidget *parent = nullptr);
    ~KortinValintaWindow();

signals:
    void debitValittu();

    void creditValittu();

    void logoutValittu();

private slots:
    void on_btnDebit_clicked();

    void on_btnCredit_clicked();

    void on_btnKirjauduUlos_clicked();

private:
    Ui::KortinValintaWindow *ui;
};

#endif // KORTINVALINTAWINDOW_H
