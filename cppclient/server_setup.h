#ifndef SETUP_SERVER_H
#define SETUP_SERVER_H

#include "ui_server_setup.h"
#include "mainwindow.h"

class ServerSetup
    : public QDialog {

    Q_OBJECT

public:
    explicit ServerSetup(MainWindow *parent = 0);
    virtual ~ServerSetup() {
        delete ui;
    }
    virtual void accept(void);
    virtual void reject(void);

private:
    MainWindow *parent;
    Ui_CXNServer *ui;
};

#endif
