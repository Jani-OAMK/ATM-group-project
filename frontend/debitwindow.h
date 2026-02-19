#ifndef DEBITWINDOW_H
#define DEBITWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "tilitapahtumatwindow.h"
#include "nostodebit.h"

namespace Ui {
class DebitWindow;
}

class DebitWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DebitWindow(const QByteArray &token, int tili_id, int kortti_id, QNetworkAccessManager *manager, QWidget *parent = nullptr);
    ~DebitWindow();

signals:
    void logoutValittu();

 private slots:
    void on_btnKirjauduUlos_clicked();
    void on_btnNosto_clicked();
    void on_btnTilitapahtumat_clicked();
    void onIdleTimeout();


private:
    Ui::DebitWindow *ui;
    QByteArray webToken;
    QNetworkAccessManager* manager = nullptr;  //käyttää MainWindowin manageria joka syötetään nostodebit uille
    int tili_id;
    int kortti_id;
    nosto* nostoWindow = nullptr;
};

#endif // DEBITWINDOW_H
