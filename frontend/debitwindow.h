#ifndef DEBITWINDOW_H
#define DEBITWINDOW_H

#include "nostodebit.h"
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace Ui {
class DebitWindow;
}

class DebitWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DebitWindow(QWidget *parent = nullptr);
    ~DebitWindow();

signals:
    void logoutValittu();

 private slots:
    void on_btnKirjauduUlos_clicked();
    void on_btnNosto_clicked();
    void on_btnTilitapahtumat_clicked();

private:
    Ui::DebitWindow *ui;
    QNetworkAccessManager* manager = nullptr;  //käyttää MainWindowin manageria joka syötetään nostodebit uille
    int tili_id;
    int kortti_id;
    nosto* nostoWindow = nullptr;
};

#endif // DEBITWINDOW_H
