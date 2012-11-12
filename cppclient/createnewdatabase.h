#ifndef CREATENEWDATABASE_H
#define CREATENEWDATABASE_H

#include <iostream>
#include <string>

#include <QtGui>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QMap>

#include "ui_create_new_table.h"
#include "mainwindow.h"

class CreateNewDatabase :
    public QDialog {

    Q_OBJECT

public:

    enum {
        text,
        choice,
        date,
        time,
        group,
        currency
    };

    explicit CreateNewDatabase(MainWindow *parent = 0);
    virtual ~CreateNewDatabase() {
        for (int x = 0; x < list_items.size(); ++x)
            delete list_items[x];
    }
    virtual void accept(void);
    virtual void reject(void);

public slots:
    void changeFieldDescriptions(int i);
    void acceptFieldAdd();
    void ShowRowAttributes();
    void DeleteSelectedRow();

private:
    QString generateTextData();
    QString generateChoiceData();
    QString generateDateData();
    QString generateCurrData();
    QString GetCurrentRowname();
    void GenericCleanup();
    void AddToList(QString);
    QString genericAddData();
    QString toStrBool(bool);
    bool CheckOverwrite();
    bool doPreChecks();

    MainWindow *parent;
    std::unique_ptr<Ui_CreateNewDatabase> ui;
    std::unique_ptr<QMap<QString, QString>> rowmap;
    int column_number;
    QList<QListWidgetItem*> list_items;
};
#endif
