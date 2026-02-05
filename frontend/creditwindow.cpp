#include "creditwindow.h"
#include "ui_creditwindow.h"
#include <QDebug>

CreditWindow::CreditWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CreditWindow)
{
    ui->setupUi(this);
}

CreditWindow::~CreditWindow()
{
    delete ui;
}

void CreditWindow::on_btnKirjauduUlos_clicked()
{
    qDebug() << "Credit: kirjaudu ulos";
    emit logoutValittu();
    close();
}

void CreditWindow::on_btnNosto_clicked()
{
    qDebug() << "Credit: nosta rahaa";
    emit nostoValittu();
}

void CreditWindow::on_btnTilitapahtumat_clicked()
{
    qDebug() << "Credit: tilitapahtumat";
    emit tilitapahtumatValittu();
}
