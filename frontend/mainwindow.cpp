#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "debitwindow.h"
#include "kortinvalintawindow.h"
#include <QUrl>
#include <QDebug>
#include "creditwindow.h"
#include <QPixmap>
#include "enviroment.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->textUsername, &QLineEdit::returnPressed,
            ui->BtnLogin, &QPushButton::click);

    connect(ui->textUserpassword, &QLineEdit::returnPressed,
            ui->BtnLogin, &QPushButton::click);

    connect(ui->BtnLogin, &QPushButton::clicked,
            this, &MainWindow::btnLoginSlot);

    manager = new QNetworkAccessManager(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::btnLoginSlot()
{
    const QString url = Enviroment::base_url() + "verify-pin";
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
    if (tokenString.isEmpty()) {
        qDebug() << "verify-pin OK mutta token puuttuu:" << doc;
    } else {
        webToken = tokenString.toUtf8();
        qDebug() << "Token jemmassa" << webToken.size();
        auto *w = new KortinValintaWindow();
        w->setAttribute(Qt::WA_DeleteOnClose);

        connect(w, &KortinValintaWindow::debitValittu, this, [this]() {
            qDebug() << "MainWindow: Debit valittu";

            auto *d = new DebitWindow();
            d->setAttribute(Qt::WA_DeleteOnClose);

            connect(d, &DebitWindow::logoutValittu, this, [this]() {
                resetLogin();
                this->show();   // palaa login-ikkunaan
            });

            d->show();
        });


        connect(w, &KortinValintaWindow::creditValittu, this, [this]() {
            qDebug() << "MainWindow: Credit valittu";

            auto *c = new CreditWindow();
            c->setAttribute(Qt::WA_DeleteOnClose);

            connect(c, &CreditWindow::logoutValittu, this, [this]() {
            resetLogin();
            this->show();
        });

        c->show();
    });


        connect(w, &KortinValintaWindow::logoutValittu, this, [this]() {
            resetLogin();
            this->show(); // palaa login-ikkunaan
        });
        w->show();

        this->hide();

    }

    reply->deleteLater();
    reply = nullptr;
    return;

}

void MainWindow::resetLogin()
{
    ui->textUsername->clear();
    ui->textUserpassword->clear();
    ui->textUsername->setFocus();
}


