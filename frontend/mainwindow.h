#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
   explicit  MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void resetLogin();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager = nullptr;
    QNetworkReply *reply = nullptr;
    QByteArray webToken;

private slots:
    void btnLoginSlot();
    void loginAction();
};
#endif // MAINWINDOW_H
