#include "tilitapahtumatwindow.h"
#include "ui_tilitapahtumatwindow.h"
#include <QDebug>
#include <QShowEvent>
#include <QMessageBox>


TilitapahtumatWindow::TilitapahtumatWindow(QNetworkAccessManager *manager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TilitapahtumatWindow)
{
    ui->setupUi(this);
    this->manager = manager;

    //Varmistetaan tässä kohtaa taulukon valmius
    ui->tableTapahtumat->setColumnCount(3);
    ui->tableTapahtumat->setHorizontalHeaderLabels({"Laji", "Summa (€)", "Aika"});
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

void TilitapahtumatWindow::on_btnKirjauduUlos_clicked()
{
qDebug() <<"Kirjaudu ulos" ;
    emit logoutValittu();
    close();
}

void TilitapahtumatWindow::on_btnPalaa_clicked()
{
qDebug() <<"Palaa takaisin" ;
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

void TilitapahtumatWindow::haeSaldo()
{
    QString url = Environment::base_url() + "tili/" + QString::number(tili_id) + "/debit";
qDebug() << "HAETAAN SALDO → URL:" << url;


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
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        QString saldoStr = obj.value("saldo_eur").toString();
        double saldo = saldoStr.toDouble();
        if (saldo >= 0) {
            ui->labelSaldo->setText(QString::number(saldo, 'f', 2));
        }
        else {
            ui->labelSaldo->setText("ei saatavilla");
        }
    }
    replySaldo->deleteLater();
    replySaldo = nullptr;
}

void TilitapahtumatWindow::haeTilitapahtumat()
{
    QString url = Environment::base_url() + "transaktio/tapahtumat/" + QString::number(tili_id);
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

    qDebug() << "Tapahtumat-vastaus:" << data;

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isArray()) {
        QJsonArray arr = doc.array();

        ui->tableTapahtumat->setRowCount(arr.size());

        int row = 0;
        for (const QJsonValue &v : arr) {
            QJsonObject o = v.toObject();

            QString laji = o.value("laji").toString();
            QString summaStr = o.value("summa_eur").toString();
            double summa = summaStr.toDouble();

            QString palvelinAika = o.value("tapahtuma_aika").toString();
            QDateTime dt = QDateTime::fromString(palvelinAika, Qt::ISODate);
            QString aika = dt.toString("yyyy-MM-dd HH:mm:ss");

            ui->tableTapahtumat->setItem(row, 0, new QTableWidgetItem(laji));
            ui->tableTapahtumat->setItem(row, 1, new QTableWidgetItem(QString::number(summa, 'f', 2)));
            ui->tableTapahtumat->setItem(row, 2, new QTableWidgetItem(aika));
            row++;
        }
    }
    else {
        qDebug() << "Ei array-vastausta tapahtumissa";
    }
    replyTapahtumat->deleteLater();
    replyTapahtumat = nullptr;
}

