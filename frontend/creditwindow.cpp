#include "creditwindow.h"
#include "ui_creditwindow.h"
#include <QDebug>
#include <QWidget>

CreditWindow::CreditWindow(const QByteArray &token, int tili_id, int kortti_id, QNetworkAccessManager *manager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CreditWindow)
{
    ui->setupUi(this);

    this->token     = token;
    this->tili_id   = tili_id;
    this->kortti_id = kortti_id;
    this->manager   = manager;

qDebug() << "CreditWindow avattu → tili_id:" << tili_id << "kortti_id:" << kortti_id;
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
