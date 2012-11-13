#ifndef MANAGEDIALOG_H
#define MANAGEDIALOG_H

#include <iostream>
#include <string>

#include <QtGui>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "mainwindow.h"
#include "ui_connections.h"


class ManageConnectionDialog :
public QDialog {

  Q_OBJECT

 public:
  explicit ManageConnectionDialog(MainWindow *parent = 0);
  virtual ~ManageConnectionDialog() {
    delete ui;
  }
  virtual void accept(void);
  virtual void reject(void);

 private:
    MainWindow *parent;
    Ui_ConnectionsDialog *ui;
};
#endif
