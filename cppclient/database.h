#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>

#include <QtCore/QString>
#include <QtGui>
#include <QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "settings.h"

class Database :
    public QObject {
public:
    Database(QWidget *parent, QString user, QString passwd,
             QString using_db, QString table);
    ~Database() {
        delete currentNam;
    }

    void Connect();
    void Close();
    void Update();
    void Insert();
    void Query();
    QList<QString> GetHeadings();
    void ChangeTable(QTableWidgetItem*);

public slots:
    void handleNetworkError(QNetworkReply::NetworkError);

private:
    QWidget *parent;
    QString User;
    QString Password;
    QString UsingDB;
    QString TableName;
    bool Connected;
    QList<QList<QString> > queryset;
    QList<QString> headings;
    QNetworkAccessManager *currentNam;
};

#endif // DATABASE_H
