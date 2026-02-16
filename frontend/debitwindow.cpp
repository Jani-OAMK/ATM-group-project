#include "debitwindow.h"
#include "ui_debitwindow.h"
#include "tilitapahtumatwindow.h"
#include "nostodebit.h"
#include <QDebug>
//#include <ui_nostodebit.h>

DebitWindow::DebitWindow(const QByteArray &token, int tili_id, int kortti_id, QNetworkAccessManager *manager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DebitWindow)
{
    ui->setupUi(this);
    this->webToken = token;
    this->tili_id = tili_id;
    this->kortti_id = kortti_id;
    this->manager = manager;
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

    auto *anosto = new nosto(nullptr, manager, tili_id, kortti_id, webToken);
    anosto->setAttribute(Qt::WA_DeleteOnClose);

    connect(anosto, &nosto::takaisin,this,[this](){
        this -> show();
    });

    connect(anosto, &nosto::logoutValittu,this,[this]() {});
    this -> hide();
    anosto->show();
}


void DebitWindow::on_btnTilitapahtumat_clicked()
{
    //TilitapahtumatWindow *objTilitapahtumat = new TilitapahtumatWindow(this);
    qDebug() << "DebitWindow: Tilitapahtumat valittu";
    qDebug() << "DebitWindow: Tilitapahtumat valittu";
    qDebug() << "Token:" << webToken.left(20) << "...";
    qDebug() << "Tili ID:" << tili_id;
    qDebug() << "Kortti ID:" << kortti_id;

    auto *t = new TilitapahtumatWindow(manager, this);
    t->setAttribute(Qt::WA_DeleteOnClose);

    t->setToken(webToken);
    t->setTiliId(tili_id);
    t->setKorttiId(kortti_id);


    connect(t, &TilitapahtumatWindow::takaisin, this, [this](){
        this->show();
    });

    connect(t, &TilitapahtumatWindow::logoutValittu, this, [this](){
        emit logoutValittu();           //Välitetään kirjauduUlos-painikesignaali mainiin
        this->close();                  //Suljetaan debitWindow
    });
}


