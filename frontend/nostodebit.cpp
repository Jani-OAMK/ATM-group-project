#include "nostodebit.h"
#include "ui_nostodebit.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include "environment.h"
#include "mainwindow.h"

nosto::nosto(QWidget *parent, QNetworkAccessManager* manager, int tili_id, QByteArray webToken)
    : QWidget(parent), ui(new Ui::nosto), manager(manager), tili_id(tili_id), webToken(webToken)
{
    ui->setupUi(this);

    if(manager && tili_id != 0) {
        QString url = Environment::base_url() + "tili/" + QString::number(tili_id);
        QNetworkRequest request((QUrl(url)));

        request.setRawHeader("Authorization", "Bearer " + webToken);
        qDebug() << "tarkistetaan saldoa tällä tokenilla:" << webToken.left(10) << "...";

        QNetworkReply* reply = manager->get(request);
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

    reply->deleteLater();
}



void nosto::on_btn20e_clicked()
{
    qDebug() << "Nostettu 20€";
    emit on_btn20eValittu();
}

void nosto::on_btn40e_clicked()
{
    qDebug() << "Nostettu 40€";
    emit on_btn40eValittu();
}

void nosto::on_btn50e_clicked()
{
    qDebug() << "Nostettu 50€";
    emit on_btn50eValittu();
}

void nosto::on_btn100e_clicked()
{
    qDebug() << "Nostettu 100€";
    emit on_btn100eValittu();
}

void nosto::on_btn200e_clicked()
{
    qDebug() << "Nostettu 200€";
    emit on_btn200eValittu();
}

void nosto::on_btnmuuSumma_clicked()
{
    qDebug() << "Painettu muu summa";
    emit on_btnmuuSummaValittu();
}

void nosto::setnostettavissa(double nostettavissa)
{
    ui->nostettavissa-> setText(QString::number(nostettavissa, 'f', 2) + " €");
}
void nosto::settilinSaldo(double tilinSaldo)
{
    ui->tilinSaldo-> setText(QString::number(tilinSaldo, 'f', 2) + " €");
}
void nosto::setnostettu(double nostettu)
{
    ui->nostettu-> setText(QString::number(nostettu, 'f', 2) + " €");
}
