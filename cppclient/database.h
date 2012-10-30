#ifndef DATABASE_H
#define DATABASE_H

#include <vector>

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

public slots:
    void handleNetworkError(QNetworkReply::NetworkError);

private:
    const char* generateQueryString(void);
    QWidget *parent;
    QString User;
    QString Password;
    QString UsingDB;
    QString TableName;
    bool Connected;
    void ChangeTable(int x, int y);
    QList<QList<QString> > queryset;
    QList<QString> headings;
    QNetworkAccessManager *currentNam;
};

#endif // DATABASE_H
