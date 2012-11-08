#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>

#include <QList>
#include <QtGui>
#include <QtCore/QString>
#include <QMap>
#include <QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "settings.h"

typedef QMap<QString, QVariant> QMetadata;

/*
  Class Database encapsulates the communication with the database.

  The idea behind this is that we could, and should be able to substi-
  tute another conforming class for this one.
*/
class Database :
    public QObject {

    Q_OBJECT

public:
    Database(QWidget *parent, QString user, QString passwd,
             QString using_db, QString table);
    Database(QWidget *parent) :
        parent(parent) {};
    ~Database() {
        delete currentNam;
    }

    void Connect();
    void Close();
    void Update();
    void Insert(QStringList newrowdata);
    void Query();
    void Delete(QList<QString> deleters);
    void ChangeTable(QString newdata, QString col, QString id);
	void CreateTable(QString jsondata);

    bool ParseMetadata(QMap<QString, QVariant>);
    QMetadata GetMetadata() {
        return metadata;
    };

    void SetUsername(const QString &Username) {
        User = Username;
    }
    const QString GetUsername(void) const {
        return User;
    }
    void SetPassword(const QString &Pass) {
        Password = Pass;
    }
    const QString GetPassword(void) const {
        return Password;
    }
    void SetTable(const QString &Table) {
        TableName = Table;
    }
    void SetDatabase(const QString &Database) {
        UsingDB = Database;
    }

public slots:
    void handleNetworkError(QNetworkReply::NetworkError);

private:
	int i;
    QWidget *parent;
    QString User;
    QString Password;
    QString UsingDB;
    QString TableName;
    bool Connected;
    QList<QList<QString> > queryset;
    QNetworkAccessManager *currentNam;
    QMetadata metadata;
};

#endif // DATABASE_H
