#ifndef LOGIN_H
#define LOGIN_H

#include <iostream>

#include <QtGui>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "mainwindow.h"
#include "ui_loginbox.h"


namespace login {
    class Login :
        public QDialog {

        Q_OBJECT

    public:
        explicit Login(MainWindow *parent = 0);
        virtual ~Login() {
            delete ui;
            delete currentNam;
        }
        virtual void accept(void);
        virtual void reject(void);

    public slots:
        void networkRequestFinished(QNetworkReply*);
        void handleNetworkError(QNetworkReply::NetworkError);

    private:
        void errorCleanup();
        void login();
        const char* generateLoginString();
        MainWindow *parent;
        Ui_frm_login *ui;
        QString storedUser;
        QString storedPass;
        bool networkRequestPending;
        QNetworkAccessManager *currentNam;
    };
};
#endif
