#include "creditwindow.h"
#include "ui_creditwindow.h"
#include "tilitapahtumatwindow.h"

#include <QWidget>

CreditWindow::CreditWindow(const QByteArray &token, int tili_id, int kortti_id, QNetworkAccessManager *manager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CreditWindow)
{
    ui->setupUi(this);
    this->webToken  = token;
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
    //TilitapahtumatWindow *objTilitapahtumat = new TilitapahtumatWindow(this);
    qDebug() << "DebitWindow: Tilitapahtumat valittu";
    qDebug() << "DebitWindow: Tilitapahtumat valittu";
    //qDebug() << "Token:" << webToken.left(20) << "...";
    qDebug() << "Tili ID:" << tili_id;
    qDebug() << "Kortti ID:" << kortti_id;

    auto *t = new TilitapahtumatWindow(manager, this);
    t->setAttribute(Qt::WA_DeleteOnClose);

    t->setToken(webToken);
    t->setTiliId(tili_id);
    t->setKorttiId(kortti_id);
    t->setRooli("CREDIT");


    connect(t, &TilitapahtumatWindow::logoutValittu, this, [this](){
        emit logoutValittu();           //Välitetään kirjauduUlos-painikesignaali mainiin
        this->close();                  //Suljetaan TapahtumatWindow
    });

    t->show();
}


