#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "debitwindow.h"
#include "kortinvalintawindow.h"
#include <QUrl>
#include <QDebug>
#include "creditwindow.h"
#include <QPixmap>
#include "environment.h"
#include "idlemanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->textUsername, &QLineEdit::returnPressed, ui->BtnLogin, &QPushButton::click);
    connect(ui->textUserpassword, &QLineEdit::returnPressed, ui->BtnLogin, &QPushButton::click);
    connect(ui->BtnLogin, &QPushButton::clicked, this, &MainWindow::btnLoginSlot);
    manager = new QNetworkAccessManager(this);

    // Yhdistä IdleManager timeout tähän ikkunaan
    connect(IdleManager::instance(), &IdleManager::idleTimeout, 
            this, &MainWindow::handleIdleTimeout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    IdleManager::instance()->start(10000);
    qDebug() << "MainWindow shown - IdleManager started with 10s timeout";
}

void MainWindow::btnLoginSlot()
{
    const QString url = Environment::base_url() + "auth/verify-pin";
    QNetworkRequest request((QUrl(url)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject jObject;
    jObject.insert("kortti_numero", ui->textUsername->text());
    jObject.insert("pin", ui->textUserpassword->text());

    const QJsonDocument jsonDoc(jObject);

    reply = manager->post(request, jsonDoc.toJson());
    connect(reply, &QNetworkReply::finished, this, &MainWindow::loginAction);
}

void MainWindow::loginAction()
{
    const QByteArray responseData = reply->readAll();
qDebug() << "Full login response JSON:" << responseData;


    if (reply->error() != QNetworkReply::NoError) {
       //debug pitää poistaa myöhemmin
qDebug() << "verify-pin failed:" << reply->errorString();
qDebug() << "server response:" << responseData;
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    QJsonParseError parseError{};
    const QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        //debug pitää poistaa myöhemmin
qDebug() << "verify-pin response   " << parseError.errorString();
qDebug() << "server response:" << responseData;
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    const QJsonObject obj = doc.object();


    const QString tokenString = obj.value("token").toString();
    const int serverKorttiId  = obj.value("kortti_id").toInt(0);
    const QString cardType    = obj.value("cardType").toString().toUpper();
    const QJsonArray tilit    = obj.value("tilit").toArray();

    if (tokenString.isEmpty() || serverKorttiId <= 0) {
qDebug() << "verify-pin OK mutta token puuttuu:" << doc;
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    webToken = tokenString.toUtf8();
    kortti_id = serverKorttiId;

qDebug() << "Token jemmassa" << webToken.size();
qDebug() << "kortti_id:" << kortti_id;
qDebug() << "cardType:" << cardType;

 // KÄYNNISTÄ IdleManager 30 sekunnin timeoutilla
    IdleManager::instance()->start(30000);


    int debitTiliId  = 0;
    int creditTiliId = 0;

    for (const QJsonValue &val : tilit) {
        QJsonObject tiliObj = val.toObject();
        QString rooli   = tiliObj.value("rooli").toString().toUpper();
        int     tiliId  = tiliObj.value("tili_id").toInt(0);

        if (rooli == "DEBIT"  && tiliId > 0) debitTiliId  = tiliId;
        if (rooli == "CREDIT" && tiliId > 0) creditTiliId = tiliId;
    }
qDebug() << "Löydettiin debit tili_id:" << debitTiliId;
qDebug() << "Löydettiin credit tili_id:" << creditTiliId;

    bool useSelectionWindow = (cardType == "COMBO") || (debitTiliId > 0 && creditTiliId > 0);

if (!useSelectionWindow) {
    // Yksinkertainen kortti → suoraan oikeaan ikkunaan

    if (cardType == "DEBIT" && debitTiliId > 0) {
qDebug() << "DEBIT-kortti → suoraan DebitWindow";
        auto *d = new DebitWindow(webToken, debitTiliId, kortti_id, manager);
        d->setAttribute(Qt::WA_DeleteOnClose);
        connect(d, &DebitWindow::logoutValittu, this, [this]() {
            IdleManager::instance()->stop();
            resetLogin();
            this->show();
        });
        d->show();
        this->hide();
    }
    else if (cardType == "CREDIT" && creditTiliId > 0) {
qDebug() << "CREDIT-kortti → suoraan CreditWindow";
        auto *c = new CreditWindow(webToken, creditTiliId, kortti_id, manager);
        // ↑↑↑ TÄRKEÄÄ: CreditWindow pitää tukea samaa konstruktoria kuin DebitWindow
        c->setAttribute(Qt::WA_DeleteOnClose);
        connect(c, &CreditWindow::logoutValittu, this, [this]() {
            IdleManager::instance()->stop();
            resetLogin();
            this->show();
        });
        c->show();
        this->hide();
    }
    else {
qDebug() << "Ei debit- eikä credit-tiliä → virhe";

    }
}
else {

qDebug() << "COMBO tai useita tilejä → näytetään KortinValintaWindow";

    auto *w = new KortinValintaWindow();
    w->setAttribute(Qt::WA_DeleteOnClose);

    connect(w, &KortinValintaWindow::debitValittu, this, [this, debitTiliId]() {
qDebug() << "Debit valittu, käytetään tili_id:" << debitTiliId;
        if (debitTiliId > 0) {
            auto *d = new DebitWindow(webToken, debitTiliId, kortti_id, manager);
            d->setAttribute(Qt::WA_DeleteOnClose);
            connect(d, &DebitWindow::logoutValittu, this, [this]() {
                IdleManager::instance()->stop();
                resetLogin();
                this->show();
            });
            d->show();
            this->hide();
        } else {
qDebug() << "Virhe: debit-tiliä ei löytynyt vaikka DEBIT valittiin";
        }
    });

    connect(w, &KortinValintaWindow::creditValittu, this, [this, creditTiliId]() {
qDebug() << "Credit valittu, käytetään tili_id:" << creditTiliId;
        if (creditTiliId > 0) {
            auto *c = new CreditWindow(webToken, creditTiliId, kortti_id, manager);
            c->setAttribute(Qt::WA_DeleteOnClose);
            connect(c, &CreditWindow::logoutValittu, this, [this]() {
                IdleManager::instance()->stop();
                resetLogin();
                this->show();
            });
            c->show();
            this->hide();
        } else {
qDebug() << "Virhe: credit-tiliä ei löytynyt vaikka CREDIT valittiin";
        }
    });

    connect(w, &KortinValintaWindow::logoutValittu, this, [this]() {
        IdleManager::instance()->stop();
        resetLogin();
        this->show();
    });

    w->show();
    this->hide();
}

reply->deleteLater();
reply = nullptr;
}

void MainWindow::resetLogin()
{
    ui->textUsername->clear();
    ui->textUserpassword->clear();
    ui->textUsername->setFocus();
    webToken = "";
    kortti_id =  0;   
}

void MainWindow::handleIdleTimeout()
{
    qDebug() << "MainWindow::handleIdleTimeout() CALLED";
    qDebug() << "Idle timeout -> automaattinen logout";

   

    // Sulje kaikki muut ikkunat paitsi MainWindow
    for (QWidget *w : QApplication::topLevelWidgets()) {
        if (w != this && w->isVisible()) {
            w->close();
        }
    }

    resetLogin();
    this->show();

     IdleManager::instance()->start(10000);
}



