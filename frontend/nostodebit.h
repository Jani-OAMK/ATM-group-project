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
    explicit nosto(QWidget *parent = nullptr, QNetworkAccessManager* manager = nullptr, int tili_id = 0, QByteArray webToken = "");
    ~nosto();
    //double pitää muutta vielä mikäli käytetään pienempää arvoa
    void settilinSaldo(double tilinSaldo);
    void setnostettu(double nostettu);
    void setnostettavissa(double nostettavissa);



signals:
    void on_btn20eValittu();
    void on_btn40eValittu();
    void on_btn50eValittu();
    void on_btn100eValittu();
    void on_btn200eValittu();
    void on_btnmuuSummaValittu();
    void on_btnKirjauduUlosValittu();


private slots:
    void on_btn20e_clicked();
    void on_btn40e_clicked();
    void on_btn50e_clicked();
    void on_btn100e_clicked();
    void on_btn200e_clicked();
    void on_btnmuuSumma_clicked();
    void saldoVastaus(QNetworkReply*reply);


private:
    Ui::nosto *ui;
     QNetworkAccessManager* manager = nullptr;
    int tili_id;
    int kortti_id;
    QByteArray webToken;

};

#endif // NOSTO_H
