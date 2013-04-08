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
    for (int x = 0; x < listwidgets.size(); ++x)
        delete listwidgets[x];
  }
  virtual void accept(void);
  virtual void reject(void);
  
private slots:
    void fillList();
    void populateFields(QListWidgetItem *item);
    void deleteRow();

private:
    void WriteConfig();

    MainWindow *parent;
    Ui_ConnectionsDialog *ui;
    QList<QListWidgetItem*> listwidgets;
};

/*
  This was created because QListWidgetItem does not inherit from QObject
  and thus doesn't have the setObjectName function. We can use this on
  other classes to have an internal name or piece of data associated
  with a widget.
*/
class CustomListWidget :
    public QListWidgetItem {

public:
    explicit CustomListWidget(QString s)
        : QListWidgetItem(s) {}
    void setObjectName(QString s) {
        o = s;
    }
    const QString objectName() const {
        return o;
    }
private:
    QString o;
};
#endif
