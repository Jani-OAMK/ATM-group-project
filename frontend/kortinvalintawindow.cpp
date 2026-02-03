#include "kortinvalintawindow.h"
#include "ui_kortinvalintawindow.h"

KortinValintaWindow::KortinValintaWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::KortinValintaWindow)
{
    ui->setupUi(this);
}

KortinValintaWindow::~KortinValintaWindow()
{
    delete ui;
}

void KortinValintaWindow::on_btnDebit_clicked()
{
    qDebug() << "Debit valittu";
}


void KortinValintaWindow::on_btnCredit_clicked()
{
    qDebug() << "Credit valittu";
}


void KortinValintaWindow::on_btnKirjauduUlos_clicked()
{
    close(); // tai paluu login-ikkunaan
}

