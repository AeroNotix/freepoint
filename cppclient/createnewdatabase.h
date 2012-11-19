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
    /*
     * @brief Simply accepts the dialog without creating a database or prompting
     * the user to see if they are sure about quitting.
     */
    virtual void accept(void);
    /*
     * @brief Simply rejectss the dialog without creating a database it will
     * prompt the user to confirm before closing.
     */
    virtual void reject(void);

public slots:
    /*
     * @brief changeFieldDescriptions is fired when the fieldButtonGroup is changed
     * we N = (~N-1) because the field ID numbers are negative indexes starting
     * from -2 for some bloody reason. Dafuq?
     * @param i this is the index of the fieldtype which was selected.
     */
    void changeFieldDescriptions(int i);
    /*
     * @brief acceptFieldAdd is fired on a StackedWidget page. It will determine
     * which page is currently active and create a row entry from the widgets
     * on that page.
     */
    void acceptFieldAdd();
    void DeleteSelectedRow();
    /*
     * @brief CreateTable creates the JSON string from the user-created rows and fires
     * off a request to the database.
     */
    void CreateTable();

private slots:
    void removeDatabaseNotifications();
    void removeTableNotifications();
    void removeMissingRowsNotifications();

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
    QList<QListWidgetItem*> list_items;
    QStringList rownames;
};
#endif
