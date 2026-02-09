#include "nostodebit.h"
#include "ui_nostodebit.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QObject>
#include "environment.h"
#include "mainwindow.h"

nosto::nosto(QWidget *parent, QNetworkAccessManager* manager, int tili_id, int kortti_id, QByteArray webToken)
    : QWidget(parent), ui(new Ui::nosto),
    manager(manager), tili_id(tili_id),
    kortti_id(kortti_id), webToken(webToken)
{
    ui->setupUi(this);

    haeKayttosaldo();

}

void nosto::haeKayttosaldo()
{
    if(manager && tili_id != 0 || kortti_id != 0) {
        QString url = Environment::base_url() + "transaktio/kayttosaldo";
        QNetworkRequest request((QUrl(url)));

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization", "Bearer " + webToken);
        qDebug() << "tarkistetaan saldoa tällä tokenilla:" << webToken.left(10) << "...";
        qDebug() << "haeKayttosaldo: Lähetetään POST ->" << url;
        qDebug() << "haeKayttosaldo: tili_id =" << tili_id << ", kortti_id =" << kortti_id;
        qDebug() << "haeKayttosaldo: token =" << webToken.left(10) << "...";

        QJsonObject json;
        json["tili_id"] = tili_id;
        json["kortti_id"] = kortti_id;

        QNetworkReply* reply = manager->post(request, QJsonDocument(json).toJson());
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            saldoVastaus(reply);
        });
    }
}

nosto::~nosto()
{
    delete ui;
}

void nosto::saldoVastaus(QNetworkReply* reply)
{
    QByteArray data = reply->readAll();
    reply->deleteLater();
qDebug() << "saldoVastaus: raw response =" << data;
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "saldoVastaus: VIRHE:" << reply->errorString();
        return;
    }
    QJsonObject obj = QJsonDocument::fromJson(data).object();
qDebug() << "saldoVastaus: JSON =" << obj;
double saldo = obj["saldo_eur"].toString().toDouble();
    qDebug() << "saldoVastaus: saldo =" << saldo;
    settilinSaldo(saldo);
}


void nosto::on_btnmuuSumma_clicked()
{

}
void nosto::settilinSaldo(double tilinSaldo)
{
    ui->tilinSaldo-> setText(QString::number(tilinSaldo, 'f', 2) + " €");
}
void nosto::setnostettu(double nostettu)
{
    ui->nostettu-> setText(QString::number(nostettu, 'f', 2) + " €");
}

void nosto::on_btnKirjauduUlos_clicked()
{
    qDebug() <<"Kirjaudu ulos" ;
    emit logoutValittu();
    close();
}

void nosto::on_btnPalaa_clicked()
{
    qDebug() <<"Palaa takaisin" ;
    this->close();
}
