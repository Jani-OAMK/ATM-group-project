#ifndef DEBITWINDOW_H
#define DEBITWINDOW_H

#include "tilitapahtumatwindow.h"
#include <QMainWindow>

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

private:
    Ui::DebitWindow *ui;
    QByteArray token;
    int tili_id;
    int kortti_id;

    QNetworkAccessManager *manager;
};

#endif // DEBITWINDOW_H
