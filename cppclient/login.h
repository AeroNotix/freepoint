#ifndef LOGIN_H
#define LOGIN_H

#include <iostream>
#include <string>

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
        /**
         * @brief This method should not be called directly.
         * This method is asynchronously called via the QNetworkAccessManager
         * class.
         */
        void networkRequestFinished(QNetworkReply*);
        /**
         * @brief We attach a signal to this slot so we can asynchronously
         * cleanup members that won't be used because the NetworkRequests fail.
         */
        void handleNetworkError(QNetworkReply::NetworkError);

    private:
        void errorCleanup();
        void login();
        QString generateLoginString();
        MainWindow *parent;
        Ui_frm_login *ui;
        QString storedUser;
        QString storedPass;
        bool networkRequestPending;
        QNetworkAccessManager *currentNam;
    };
};
#endif
