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
    void setsaldoVastaus(double saldo );
    void setmuuSumma(int muuSummax);

signals:
    void kirjauduUlosValittu();
    void logoutValittu();
    void takaisin();
    void muuSummaWidget();

private slots:
    void on_btnOk_clicked();
    void on_btnPeruuta_clicked();
    void on_btn20e_clicked();
    void on_btn40e_clicked();
    void on_btn50e_clicked();
    void on_btn100e_clicked();
    void on_btnmuuSumma_clicked();
    void on_btnKirjauduUlos_clicked();
    void on_btnPalaa_clicked();
    void saldoVastaus(QNetworkReply*reply);
    void onIdleTimeout();

private:
    Ui::nosto *ui;
     QNetworkAccessManager* manager = nullptr;
    QByteArray webToken;
    int tili_id;
    int kortti_id;


    void haeKayttosaldo();
    void lahetaNosto(double summa);
    void nostoVastaus(QNetworkReply*reply);
    void muuSumma(double);
    void varoitusLabel();
    double hetkellinenSaldo=0;

};

#endif // NOSTO_H
