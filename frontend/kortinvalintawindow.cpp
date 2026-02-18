#include "kortinvalintawindow.h"
#include "ui_kortinvalintawindow.h"
#include "idlemanager.h"
#include <QDebug>
#include <QShowEvent>

KortinValintaWindow::KortinValintaWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::KortinValintaWindow)
{
    ui->setupUi(this);
    
    // Connectaa idle timeout - jos 30s tulee täyteen, logout
    connect(IdleManager::instance(), &IdleManager::idleTimeout, this, &KortinValintaWindow::onIdleTimeout);
}

KortinValintaWindow::~KortinValintaWindow()
{
    delete ui;
}

void KortinValintaWindow::on_btnDebit_clicked()
{
    qDebug() << "Debit valittu";
    emit debitValittu();
    close();
}

void KortinValintaWindow::on_btnCredit_clicked()
{
    qDebug() << "Credit valittu";
    emit creditValittu();
    close();
}

void KortinValintaWindow::on_btnKirjauduUlos_clicked()
{
    qDebug() << "Kirjaudu ulos";
    emit logoutValittu();
    close();
}

void KortinValintaWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    IdleManager::instance()->stop();
    IdleManager::instance()->start(30000);
}

void KortinValintaWindow::onIdleTimeout()
{
    qDebug() << "KortinValintaWindow idle timeout - logout";
    emit logoutValittu();
    this->close();
}
