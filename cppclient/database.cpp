#include <vector>
#include <sstream>
#include <iostream>

#include <QList>
#include <QtGui>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QNetworkCookie>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxy>

#include "database.h"
#include "settings.h"
#include "jsonpackets.h"
#include "mainwindow.h"
#include "requests.h"

Database::Database
(MainWindow *parent, QString user, QString passwd, QString using_db, QString table)
    : QObject(), parent(parent), User(user), Password(passwd), UsingDB(using_db),
	  TableName(table), currentNam(new QNetworkAccessManager(this))
{
	currentNam->setCookieJar(new QNetworkCookieJar(this));
	currentNam->cookieJar()->setCookiesFromUrl(parent->GetCookies(), QUrl(Settings::SERVERURL));
};

void Database::Close() {
    return;
}

void Database::Insert(QStringList newrowdata) {
    disconnect();
    QObject::connect(currentNam.get(), SIGNAL(finished(QNetworkReply*)),
                     parent, SLOT(InsertedRow(QNetworkReply*)));

    InsertQuery iq = InsertQuery(UsingDB, TableName, newrowdata);
    QByteArray data;
    data.append(iq.QueryString());
    QUrl url(Settings::INSERTURL);
	QNetworkRequest req(url);
    SetHeaders(req);
    QNetworkReply *reply = currentNam->post(req, data);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));
}

void Database::Query() {
    disconnect();
    QObject::connect(currentNam.get(), SIGNAL(finished(QNetworkReply*)),
                     parent, SLOT(InsertData(QNetworkReply*)));
    GetQuery gq = GetQuery(UsingDB, TableName);
    QByteArray data;
    data.append(gq.QueryString());
    QUrl url(Settings::PARAMURL);
	QNetworkRequest req(url);
    SetHeaders(req);
    QNetworkReply *reply = currentNam->post(req, data);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));
}

void Database::Delete(QList<QString> deleters) {
    disconnect();
    QObject::connect(currentNam.get(), SIGNAL(finished(QNetworkReply*)),
                     parent, SLOT(DeletedData(QNetworkReply*)));

    DeleteQuery dq = DeleteQuery(UsingDB, TableName, deleters);
    QByteArray data;
    data.append(dq.QueryString());
    QUrl url(Settings::DELETEURL);
    QNetworkRequest req(url);
    SetHeaders(req);
    QNetworkReply *reply = currentNam->post(req, data);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));
}

void Database::Create(QString jsondata) {
    disconnect();
    QByteArray data;
    data.append(jsondata);
    QUrl url(Settings::CREATEURL);
    QNetworkRequest req(url);
    SetHeaders(req);
    QNetworkReply *reply = currentNam->post(req, data);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));
}

void Database::ChangeTable(QString newdata, QString col, QString id) {
    disconnect();
    QObject::connect(currentNam.get(), SIGNAL(finished(QNetworkReply*)),
                     parent, SLOT(UpdatedData(QNetworkReply*)));
    UpdateQuery uq = UpdateQuery(UsingDB, TableName, newdata, col, id);
    QByteArray data;
    data.append(uq.QueryString());
    QUrl url(Settings::UPDATEURL);
    QNetworkRequest req(url);
    SetHeaders(req);
    QNetworkReply *reply = currentNam->post(req, data);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));
}

bool Database::ParseMetadata(QMap<QString, QVariant> rawmeta) {
    QMap<QString, QVariant> rawmetadata = rawmeta["Metadata"].toMap();
    QMap<QString, QVariant> headings = rawmetadata["HEADINGS"].toMap();
    QMetadata newmetadata;
    QList<QString> headerlist = headings.keys();
    for (int x = 0; x < headerlist.size(); ++x) {
        newmetadata.insert(headerlist[x], headings[headerlist[x]].toMap());
    }

    metadata = newmetadata;
    return true;
}

void Database::disconnect() {
	if (currentNam)
		currentNam->disconnect();
}

/*
  Not implemented.
*/
void Database::handleNetworkError(QNetworkReply::NetworkError) {}
