#include "creditwindow.h"
#include "ui_creditwindow.h"
#include "tilitapahtumatwindow.h"
#include "nostodebit.h"
#include "idlemanager.h"
#include <QWidget>

CreditWindow::CreditWindow(const QByteArray &token, int tili_id, int kortti_id, QNetworkAccessManager *manager, QString kuva, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CreditWindow)
{
    ui->setupUi(this);
    this->webToken  = token;
    this->tili_id   = tili_id;
    this->kortti_id = kortti_id;
    this->manager   = manager;

    qDebug() << "CreditWindow avattu → tili_id:" << tili_id << "kortti_id:" << kortti_id;
    
    // Connectaa idle timeout - jos 30s tulee täyteen, logout
    connect(IdleManager::instance(), &IdleManager::idleTimeout, this, &CreditWindow::onIdleTimeout);

    const QString kuvaUrl =
        "https://ankkalinnanpankki.rocks/asiakasImages/" + kuva;
    qDebug() << "Manager pointer:" << manager;
    QNetworkReply *rep = manager->get( QNetworkRequest(QUrl(kuvaUrl)) );

    connect(rep, &QNetworkReply::finished, this, [this, rep]() {
        if (rep->error() == QNetworkReply::NoError) {
            QByteArray data = rep->readAll();
            QPixmap px;
            if (px.loadFromData(data)) {
                ui->labelKuva->setPixmap(px.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } else {
                qDebug() << "Kuvan lataus epäonnistui:" << rep->errorString();
            }
            rep->deleteLater();
        }
    });
}

CreditWindow::~CreditWindow()
{
    delete ui;
}

void CreditWindow::onIdleTimeout()
{
    emit logoutValittu();
    this->close();
}

void CreditWindow::on_btnKirjauduUlos_clicked()
{
    qDebug() << "Credit: kirjaudu ulos";
    emit logoutValittu();
    close();
}

void CreditWindow::on_btnNosto_clicked()
{
    qDebug() << "CreditWindow: Nosta rahaa painettu";

    auto *anosto = new nosto(nullptr, manager, tili_id, kortti_id, webToken);
    anosto->setAttribute(Qt::WA_DeleteOnClose);

    connect(anosto, &nosto::takaisin, this, [this](){
        IdleManager::instance()->stop();
        IdleManager::instance()->start(30000);
        // Reconnectaa credit window idleTimeout
        connect(IdleManager::instance(), &IdleManager::idleTimeout, this, &CreditWindow::onIdleTimeout);
        this->show();
    });

    connect(anosto, &nosto::logoutValittu, this, [this](){
        emit logoutValittu();
        this->close();
    });

    // Disconnectaa credit window idleTimeout ennen nosto ikkunan avaus
    disconnect(IdleManager::instance(), &IdleManager::idleTimeout, this, &CreditWindow::onIdleTimeout);
    
    this->hide();
    anosto->show();
}


void CreditWindow::on_btnTilitapahtumat_clicked()
{
    //TilitapahtumatWindow *objTilitapahtumat = new TilitapahtumatWindow(this);
    qDebug() << "DebitWindow: Tilitapahtumat valittu";
    qDebug() << "DebitWindow: Tilitapahtumat valittu";
    //qDebug() << "Token:" << webToken.left(20) << "...";
    qDebug() << "Tili ID:" << tili_id;
    qDebug() << "Kortti ID:" << kortti_id;

    auto *t = new TilitapahtumatWindow(manager, this);
    t->setAttribute(Qt::WA_DeleteOnClose);

    t->setToken(webToken);
    t->setTiliId(tili_id);
    t->setKorttiId(kortti_id);
    t->setRooli("CREDIT");

    connect(t, &TilitapahtumatWindow::takaisin, this, [this](){
        IdleManager::instance()->stop();
        IdleManager::instance()->start(30000);
        // Reconnectaa credit window idleTimeout
        connect(IdleManager::instance(), &IdleManager::idleTimeout, this, &CreditWindow::onIdleTimeout);
        this->show();
    });

    connect(t, &TilitapahtumatWindow::logoutValittu, this, [this](){
        emit logoutValittu();           //Välitetään kirjauduUlos-painikesignaali mainiin
        this->close();                  //Suljetaan creditWindow
    });

    // Disconnectaa credit window idleTimeout ennen tilitapahtumat ikkunan avaus
    disconnect(IdleManager::instance(), &IdleManager::idleTimeout, this, &CreditWindow::onIdleTimeout);
    
    this->hide();
    t->show();
}


