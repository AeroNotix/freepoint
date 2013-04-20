#ifndef CREATEUSER_H
#define CREATEUSER_H

#include <iostream>
#include <string>

#include <QtGui>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "ui_create_user.h"

class MainWindow;
class CreateUser :
public QDialog {

    Q_OBJECT
    
public:
    explicit CreateUser(MainWindow *parent = 0);
    virtual ~CreateUser() {
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
private slots:
    void togglePassword(int);
private:
    void Create();
    QString generateCreateUserString();
    MainWindow *parent;
    Ui_CreateUser *ui;
    QString storedUser;
    QString storedPass;
    bool networkRequestPending;
    QNetworkAccessManager *currentNam;
};
#endif
