#ifndef NOSTO_H
#define NOSTO_H

#include <QWidget>
#include <QtDebug>
#include <qnetworkaccessmanager.h>
#include "ui_nostodebit.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace Ui {
class nosto;
}

class nosto : public QWidget
{
    Q_OBJECT

public:
    explicit nosto(QWidget *parent, QNetworkAccessManager* manager, int tili_id, int kortti_id, QByteArray webToken);
    ~nosto();
    //double pitää muutta vielä mikäli käytetään pienempää arvoa
    void settilinSaldo(double tilinSaldo);
    void setnostettu(double nostettu);

signals:
    void kirjauduUlosValittu();
    void logoutValittu();

private slots:
    /*void on_btn20e_clicked();
    void on_btn40e_clicked();
    void on_btn50e_clicked();
    void on_btn100e_clicked();
    void on_btn200e_clicked();*/
    void on_btnmuuSumma_clicked();
    void saldoVastaus(QNetworkReply*reply);
    void on_btnKirjauduUlos_clicked();
    void on_btnPalaa_clicked();

private:
    Ui::nosto *ui;
     QNetworkAccessManager* manager = nullptr;
    int tili_id;
    int kortti_id;
    QByteArray webToken;
    void haeKayttosaldo();
};

#endif // NOSTO_H
