#ifndef CREDITWINDOW_H
#define CREDITWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>

namespace Ui {
class CreditWindow;
}

class CreditWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CreditWindow(const QByteArray &token, int tili_id, int kortti_id, QNetworkAccessManager *manager, QWidget *parent = nullptr);
    ~CreditWindow();

signals:
    void logoutValittu();
    void nostoValittu();
    void tilitapahtumatValittu();

private slots:
    void on_btnKirjauduUlos_clicked();
    void on_btnNosto_clicked();
    void on_btnTilitapahtumat_clicked();

private:
    Ui::CreditWindow *ui;
    QByteArray token;
    int tili_id;
    int kortti_id;
    QNetworkAccessManager *manager;
};

#endif // CREDITWINDOW_H
