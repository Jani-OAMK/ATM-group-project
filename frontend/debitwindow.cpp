#include "debitwindow.h"
#include "ui_debitwindow.h"
#include <QDebug>

DebitWindow::DebitWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DebitWindow)
{
    ui->setupUi(this);
}

DebitWindow::~DebitWindow()
{
    delete ui;
}

void DebitWindow::on_btnKirjauduUlos_clicked()
{
    qDebug() << "DebitWindow: Kirjaudu ulos";
    emit logoutValittu();
    close();
}

void DebitWindow::on_btnNosto_clicked()
{
    qDebug() << "DebitWindow: Nosta rahaa painettu";
}

void DebitWindow::on_btnTilitapahtumat_clicked()
{
    qDebug() << "DebitWindow: Tilitapahtumat painettu";
}
