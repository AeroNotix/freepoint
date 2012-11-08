#ifndef CREATENEWDATABASE_H
#define CREATENEWDATABASE_H

#include <iostream>
#include <string>

#include <QtGui>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "ui_create_new_table.h"
#include "mainwindow.h"

class CreateNewDatabase :
    public QDialog {

    Q_OBJECT

public:
    explicit CreateNewDatabase(MainWindow *parent = 0);
    virtual ~CreateNewDatabase() {
        delete ui;
        delete currentNam;
    }
    virtual void accept(void);
    virtual void reject(void);

public slots:
//    void networkRequestFinished(QNetworkReply*);
//    void handleNetworkError(QNetworkReply::NetworkError);
    void changeFieldDescriptions(int i);
private:
    MainWindow *parent;
    Ui_CreateNewDatabase *ui;
    bool networkRequestPending;
    QNetworkAccessManager *currentNam;
};
#endif
