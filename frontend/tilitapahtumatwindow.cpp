#include "tilitapahtumatwindow.h"
#include "ui_tilitapahtumatwindow.h"
//#include <QDebug>
//#include <QShowEvent>
//#include <QMessageBox>


TilitapahtumatWindow::TilitapahtumatWindow(QNetworkAccessManager *manager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TilitapahtumatWindow)
{
    ui->setupUi(this);
    this->manager = manager;

    //Varmistetaan tässä kohtaa taulukon valmius
    ui->tableTapahtumat->setColumnCount(4);
    ui->tableTapahtumat->setHorizontalHeaderLabels({"", "Laji", "Summa (€)", "Aika"});
    ui->tableTapahtumat->horizontalHeader()->setStretchLastSection(true);
}

TilitapahtumatWindow::~TilitapahtumatWindow()
{
    if (replySaldo) replySaldo->deleteLater();
    if (replyTapahtumat) replyTapahtumat->deleteLater();
    delete ui;
}

void TilitapahtumatWindow::setToken(const QByteArray &t)
{
    token = t;
}

void TilitapahtumatWindow::setTiliId(int id)
{
    tili_id = id;
}

void TilitapahtumatWindow::setKorttiId(int id)
{
    kortti_id = id;
}

void TilitapahtumatWindow::setRooli(const QString &r)
{
    rooli =r;
}

void TilitapahtumatWindow::on_btnKirjauduUlos_clicked()
{
qDebug() <<"Kirjaudu ulos" ;
    emit logoutValittu();
    close();
}

void TilitapahtumatWindow::on_btnPalaa_clicked()
{
qDebug() <<"Palaa takaisin" ;
    emit takaisin();
    this->close();
}

void TilitapahtumatWindow::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    // Varmistetaan että tarvittavat tiedot ovat olemassa ennen hakuja
    if (token.isEmpty() || tili_id <= 0) {
qDebug() << "token tyhjä tai tili_id virheellinen";
qDebug() << "tili_id:" << tili_id;
        return;
    }
    haeSaldo();
    haeTilitapahtumat();
}

void TilitapahtumatWindow::on_btnSeuraava_clicked()
{
    if(rivitJaljella >0){
    sivu++;
    haeTilitapahtumat();
    }
}

void TilitapahtumatWindow::on_btnEdellinen_clicked()
{
    if (sivu > 0 ) {
        sivu--;
        haeTilitapahtumat();
    }
}

void TilitapahtumatWindow::haeSaldo()
{
    QString url;

    if (rooli == "CREDIT"){
        url = Environment::base_url() + "tili/" + QString::number(tili_id) + "/credit";
qDebug() << "HAETAAN SALDO → URL:" << url;
    } else{
        url = Environment::base_url() + "tili/" + QString::number(tili_id) + "/debit";
qDebug() << "HAETAAN SALDO → URL:" << url;
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token);

    replySaldo = manager->get(request);
    connect(replySaldo, &QNetworkReply::finished, this, &TilitapahtumatWindow::saldoSlot);
}

void TilitapahtumatWindow::saldoSlot()
{
    QByteArray data = replySaldo->readAll();

 qDebug() << "Saldo-vastaus:" << data;

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    if (rooli == "CREDIT") {
 qDebug() << "Credit JSON keys:" << obj.keys();
        QString key = "(credit_limit + saldo_eur)";
        double kayttosaldo = obj.value(key).toString().toDouble();
        ui->labelSaldo->setText(QString::number(kayttosaldo, 'f', 2));
    } else {
        double saldo = obj.value("saldo_eur").toString().toDouble();
        ui->labelSaldo->setText(QString::number(saldo, 'f', 2));
    }
    replySaldo->deleteLater();
    replySaldo = nullptr;
}

void TilitapahtumatWindow::haeTilitapahtumat()
{
    QString url = Environment::base_url() + "transaktio/tapahtumat/" + QString::number(tili_id) + "?page=" + QString::number(sivu);
    qDebug() << "Tapahtuma-URL:" << url;

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token);

    replyTapahtumat = manager->get(request);
    connect(replyTapahtumat, &QNetworkReply::finished, this, &TilitapahtumatWindow::tapahtumatSlot);
}


void TilitapahtumatWindow::tapahtumatSlot()
{
    QByteArray data = replyTapahtumat->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    (doc.isArray());
    QJsonArray arr = doc.array();

    ui->tableTapahtumat->setRowCount(arr.size());
    ui->tableTapahtumat->setEditTriggers(QAbstractItemView::NoEditTriggers);

    rivitJaljella = arr.size();
    int rivi = 0;
    int alkuRivi = sivu * 10;

    for (const QJsonValue &v : arr) {
        QJsonObject o = v.toObject();

        QString laji = o.value("laji").toString();
        QString summaStr = o.value("summa_eur").toString();
        double summa = summaStr.toDouble();

        QString palvelinAika = o.value("tapahtuma_aika").toString();
        QDateTime dt = QDateTime::fromString(palvelinAika, Qt::ISODate);
        QString aika = dt.toString("yyyy-MM-dd HH:mm:ss");

        ui->tableTapahtumat->setItem(rivi, 0, new QTableWidgetItem(QString::number(alkuRivi + rivi + 1)));
        ui->tableTapahtumat->setItem(rivi, 1, new QTableWidgetItem(laji));
        ui->tableTapahtumat->setItem(rivi, 2, new QTableWidgetItem(QString::number(summa, 'f', 2)));
        ui->tableTapahtumat->setItem(rivi, 3, new QTableWidgetItem(aika));
        ui->tableTapahtumat->verticalHeader()->hide();
        rivi++;
    }


    replyTapahtumat->deleteLater();
    replyTapahtumat = nullptr;
}
