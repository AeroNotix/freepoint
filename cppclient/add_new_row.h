#ifndef ADD_NEW_ROW_H
#define ADD_NEW_ROW_H

#include <iostream>
#include <string>

#include <QtGui>

#include "ui_add_new_row.h"

class MainWindow;
class QNetworkAccessManager;

class AddNewRow :
    public QDialog {

    Q_OBJECT

public:
    explicit AddNewRow(QStringList headers, QList<QItemDelegate*> delegates, MainWindow *parent = 0);
    virtual ~AddNewRow() {
    }
    virtual void accept(void);

private:
    void errorCleanup();
    std::string generateInsertString();
    MainWindow *parent;
    Ui_AddNewRow *ui;
    QString storedUser;
    QString storedPass;
    bool networkRequestPending;
    QNetworkAccessManager *currentNam;
};
#endif // ADD_NEW_ROW_H
