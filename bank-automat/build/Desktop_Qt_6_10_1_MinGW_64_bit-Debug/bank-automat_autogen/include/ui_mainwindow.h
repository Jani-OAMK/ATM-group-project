/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *labelSalasana;
    QLineEdit *textUsername;
    QLineEdit *textUserpassword;
    QPushButton *BtnLogin;
    QLabel *label_3;
    QLabel *labelTunnus;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1082, 841);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        labelSalasana = new QLabel(centralwidget);
        labelSalasana->setObjectName("labelSalasana");
        labelSalasana->setGeometry(QRect(40, 340, 143, 38));
        QFont font;
        font.setPointSize(22);
        labelSalasana->setFont(font);
        textUsername = new QLineEdit(centralwidget);
        textUsername->setObjectName("textUsername");
        textUsername->setGeometry(QRect(230, 260, 291, 41));
        textUserpassword = new QLineEdit(centralwidget);
        textUserpassword->setObjectName("textUserpassword");
        textUserpassword->setGeometry(QRect(230, 340, 291, 41));
        BtnLogin = new QPushButton(centralwidget);
        BtnLogin->setObjectName("BtnLogin");
        BtnLogin->setGeometry(QRect(280, 440, 191, 71));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(40, 130, 351, 51));
        label_3->setFont(font);
        labelTunnus = new QLabel(centralwidget);
        labelTunnus->setObjectName("labelTunnus");
        labelTunnus->setGeometry(QRect(40, 260, 114, 38));
        labelTunnus->setFont(font);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1082, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        labelSalasana->setText(QCoreApplication::translate("MainWindow", "SALASANA", nullptr));
        BtnLogin->setText(QCoreApplication::translate("MainWindow", "KIRJADU", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "GROUP 4 ATM SOVELLUS", nullptr));
        labelTunnus->setText(QCoreApplication::translate("MainWindow", "TUNNUS", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
