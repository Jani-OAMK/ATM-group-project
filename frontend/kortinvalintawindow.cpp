#include "kortinvalintawindow.h"
#include "ui_kortinvalintawindow.h"
#include <QDebug>

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
    emit debitValittu();
}

void KortinValintaWindow::on_btnCredit_clicked()
{
    qDebug() << "Credit valittu";
    emit creditValittu();
}

void KortinValintaWindow::on_btnKirjauduUlos_clicked()
{
    qDebug() << "Kirjaudu ulos";
    emit logoutValittu();
    close();
}
