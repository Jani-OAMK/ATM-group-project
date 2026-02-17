#include "nostodebit.h"
#include "ui_nostodebit.h"
#include "idlemanager.h"
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
    ui -> muuSummaWidget ->hide();

    IdleManager::instance()->stop();
    IdleManager::instance()->start(10000);
    connect(IdleManager::instance(), &IdleManager::idleTimeout, this, &nosto::onIdleTimeout);

    haeKayttosaldo();

}

void nosto::haeKayttosaldo()
{
    if(manager && (tili_id != 0 || kortti_id != 0)) {
        QString url = Environment:: base_url() + "transaktio/kayttosaldo/";
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

    qDebug() << "nostoVastaus RAW:" << response;

    if(reply->error() != QNetworkReply::NoError){
        qDebug() << "Error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QJsonObject obj = QJsonDocument::fromJson(response).object();
    qDebug() << "nostoVastaus JSON:" << obj;
    QJsonObject resultObj = obj["result"].toObject();


    double saldo = obj["result"].toObject()["newSaldo"].toDouble();
    qDebug() << "New saldo:" << saldo;

    setsaldoVastaus(saldo);
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
    hetkellinenSaldo = saldo;
    qDebug() << "saldoVastaus: saldo =" << saldo;
    settilinSaldo(saldo);

}


void nosto::on_btn20e_clicked()
{
    qDebug() << "Nostettu 20€";
    double summa = 20;
    if(summa > hetkellinenSaldo){
        ui -> varoitusLabel ->setText("Tilillä ei ole riittävästi katetta");
        return;
    }
    lahetaNosto(summa);
    setnostettu(summa);
}

void nosto::on_btn40e_clicked()
{
    qDebug() << "Nostettu 40€";
    double summa = 40;
    if(summa > hetkellinenSaldo){
        ui -> varoitusLabel ->setText("Tilillä ei ole riittävästi katetta");
        return;
    }
    lahetaNosto(summa);
    setnostettu(summa);
}

void nosto::on_btn50e_clicked()
{
    qDebug() << "Nostettu 50€";
    double summa = 50;
    if(summa > hetkellinenSaldo){
        ui -> varoitusLabel ->setText("Tilillä ei ole riittävästi katetta");
        return;
    }
    lahetaNosto(summa);
    setnostettu(summa);
}

void nosto::on_btn100e_clicked()
{
    qDebug() << "Nostettu 100€";
    double summa = 100;
    if(summa > hetkellinenSaldo){
        ui -> varoitusLabel ->setText("Tilillä ei ole riittävästi katetta");
            return;
    }
    lahetaNosto(summa);
    setnostettu(summa);
}


void nosto::on_btnOk_clicked()
{
    qDebug() << "btn_ok_clicked";
    QString text = ui-> muuSummaEdit->text();
    if(text.isEmpty())
        return (ui->varoitusLabel->setText("Summan pitää olla suurempi kuin 0"));

    double muuSumma = text.toDouble();

    // pitää vielä testata tätä
    if(muuSumma <= 0)
        return (ui->varoitusLabel->setText("Summan pitää olla suurempi kuin 0"));

    if(muuSumma >= 401)
        return (ui->varoitusLabel->setText("Nostorajoitus MAX 400€/vuorokausi"));

    if(!(muuSumma == 20 || (muuSumma >= 40 && ((int)muuSumma % 10 == 0))))
    {
        return ui->varoitusLabel->setText("Nostettavan summan tulee koostua vähintään 20€ ja 50€ seteleistä");
    }
    if(muuSumma > hetkellinenSaldo){
        ui -> varoitusLabel ->setText("Tilillä ei ole riittävästi katetta");
        return;
    }

    ui->nostettu->clear();

    lahetaNosto(muuSumma);
    setnostettu(muuSumma);
    ui -> muuSummaWidget ->close ();

}

void nosto::on_btnPeruuta_clicked()
{
    qDebug() << "btn_peruuta_clicked";
    ui->muuSummaWidget->close();
    ui->varoitusLabel->clear();
}

void nosto::on_btnmuuSumma_clicked()
{
    emit muuSummaWidget();
    ui ->muuSummaWidget->show();
    haeKayttosaldo();
    ui->muuSummaEdit->clear();
    ui -> saldoVastaus -> clear();
    ui -> nostettu -> clear();
    ui->varoitusLabel -> clear();


}

void nosto::setsaldoVastaus(double saldo)
{
    qDebug() << "setsaldoVastaus called with:" << saldo;
    ui->saldoVastaus-> setText(QString::number(saldo, 'f', 2) + " €");
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

void nosto::onIdleTimeout()
{
    emit takaisin();
    this->close();
}
