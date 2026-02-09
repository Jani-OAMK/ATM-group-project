#include "nostodebit.h"
#include "ui_nostodebit.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QObject>
#include "environment.h"

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

void nosto::lahetaNosto(double summa)
{
    QString url = Environment::postNosto();
    QNetworkRequest request((QUrl(url)));

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + webToken);

    QJsonObject json;
    json["tili_id"] = tili_id;
    json["kortti_id"] = kortti_id;
    json["summa_eur"] = summa;

    QByteArray data = QJsonDocument(json).toJson();

    qDebug() << "noston url:" << url;
    qDebug() << "noston json:" << data;

    QNetworkReply* reply = manager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        nostoVastaus(reply);
    });
}


void nosto::nostoVastaus(QNetworkReply* reply)
{
    QByteArray response = reply->readAll();
    qDebug() << "nostoVastaus:" << response;

    if(reply->error() != QNetworkReply::NoError){
        qDebug() << "Nosto virhe:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QJsonObject obj = QJsonDocument::fromJson(response).object();
    double saldo = obj["saldo_eur"].toString().toDouble();



    settilinSaldo(saldo);
    reply->deleteLater();
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


void nosto::on_btn20e_clicked()
{
    qDebug() << "Nostettu 20€";
    double summa = 20;
    lahetaNosto(summa);
    setnostettu(summa);
}

void nosto::on_btn40e_clicked()
{
    qDebug() << "Nostettu 40€";
    double summa = 40;
    lahetaNosto(summa);
    setnostettu(summa);
}

void nosto::on_btn50e_clicked()
{
    qDebug() << "Nostettu 50€";
    double summa = 50;
    lahetaNosto(summa);
    setnostettu(summa);
}

void nosto::on_btn100e_clicked()
{
    qDebug() << "Nostettu 100€";
    double summa = 100;
    lahetaNosto(summa);
    setnostettu(summa);
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
    emit takaisin();
    this->close();
}
