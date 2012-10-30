#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <QtCore/QString>
#include <QtGui>

#include "settings.h"

class Database {
public:
    Database(QWidget *parent, QString user, QString passwd,
             QString using_db, QString table);
    ~Database();

    void Connect();
    void Close();
    void Update();
    void Insert();

    QList<QList<QString> > Query();
    QList<QString> GetHeadings();

private:
    QWidget *Parent;
    QString User;
    QString Password;
    QString UsingDB;
    QString TableName;
    bool Connected;
    void ChangeTable(int x, int y);
    QList<QList<QString> > queryset;
    QList<QString> headings;
};

#endif // DATABASE_H
