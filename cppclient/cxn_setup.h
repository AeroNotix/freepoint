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

    enum WindowMode {
        StandAlone,
        Dialog
    };

    explicit CXNSetup(CXNSetup::WindowMode mode, MainWindow *parent = 0);
    virtual ~CXNSetup() {
        delete ui;
    }

    void accept();
    void reject();

private:
    MainWindow *parent;
    Ui_CXNSetup *ui;
    CXNSetup::WindowMode mode;
};
#endif
