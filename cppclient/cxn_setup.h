#ifndef CXN_SETUP_H
#define CXN_SETUP_H

#include <iostream>
#include <string>

#include <QtGui>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "mainwindow.h"
#include "ui_cxn_setup.h"

class CXNSetup :
    public QDialog {

    Q_OBJECT

public:
    explicit CXNSetup(MainWindow *parent = 0);
    virtual ~CXNSetup() {
        delete ui;
    }

private:
    MainWindow *parent;
    Ui_CXNSetup *ui;
};
#endif
