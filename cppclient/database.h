#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>

#include <QtGui>
#include <QtCore/QString>
#include <QMap>
#include <QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "settings.h"

typedef QMap<QString, QVariant> QMetadata;

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
    void ChangeTable(QString newdata, QString col, QString id);
	bool ParseMetadata(QMap<QString, QVariant>);
	QMetadata GetMetadata() {
		return metadata;
	};

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
    QNetworkAccessManager *currentNam;
	QMetadata metadata;
};

#endif // DATABASE_H
