#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect (ui->BtnLogin, &QPushButton::clicked, this, &MainWindow::btnLoginSlot);
    manager=new QNetworkAccessManager;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::btnLoginSlot()
{

    QString url= Enviroment::base_url()+"login";
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    qDebug()<<"login buttionia painettu";
}

void MainWindow::loginAction()
{

}
