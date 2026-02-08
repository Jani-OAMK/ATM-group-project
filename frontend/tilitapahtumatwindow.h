#ifndef TILITAPAHTUMATWINDOW_H
#define TILITAPAHTUMATWINDOW_H


#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include "environment.h"

namespace Ui {
class TilitapahtumatWindow;
}

class TilitapahtumatWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TilitapahtumatWindow(QNetworkAccessManager *manager, QWidget *parent = nullptr);
    ~TilitapahtumatWindow();
    void setToken(const QByteArray &t);
    void setTiliId(int id);
    void setKorttiId(int id);
protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::TilitapahtumatWindow *ui;
    QByteArray token;
    int tili_id;
    int kortti_id;
    QNetworkAccessManager *manager = nullptr;
    QNetworkReply *replySaldo = nullptr;
    QNetworkReply *replyTapahtumat = nullptr;
private slots:
    void on_btnKirjauduUlos_clicked();
    void on_btnPalaa_clicked();
    void haeSaldo();
    void haeTilitapahtumat();
    void tapahtumatSlot();
    void saldoSlot();
signals:
    void logoutValittu();

};

#endif // TILITAPAHTUMATWINDOW_H
