#include <vector>
#include <sstream>
#include <iostream>

#include <QList>
#include <QtGui>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxy>

#include "database.h"
#include "settings.h"
#include "jsonpackets.h"

Database::Database
(QWidget *parent, QString user, QString passwd, QString using_db, QString table)
    : QObject(), parent(parent), User(user), Password(passwd), UsingDB(using_db),
    TableName(table), currentNam(new QNetworkAccessManager(this)) {};

void Database::Close() {
    return;
}

/*
  Insert creates a JSON string which is compliant with the API to
  insert a completely new row of data.
*/
void Database::Insert(QStringList newrowdata) {
    currentNam->disconnect();
    QObject::connect(currentNam.get(), SIGNAL(finished(QNetworkReply*)),
                     parent, SLOT(InsertedRow(QNetworkReply*)));

    InsertQuery iq = InsertQuery(UsingDB, TableName, newrowdata);
    QByteArray data;
    data.append(iq.QueryString());
    QUrl url(Settings::INSERTURL);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkReply *reply = currentNam->post(req, data);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));
}

/*
  Query creates a JSON string which is compliant with the API to
  retrieve the currently active table.
*/
void Database::Query() {
    currentNam->disconnect();
    QObject::connect(currentNam.get(), SIGNAL(finished(QNetworkReply*)),
                     parent, SLOT(InsertData(QNetworkReply*)));
    GetQuery gq = GetQuery(UsingDB, TableName);
    QByteArray data;
    data.append(gq.QueryString());
    QUrl url(Settings::PARAMURL);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkReply *reply = currentNam->post(req, data);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));
}

/*
  Delete creates a JSON string which is compliant with the API to
  delete either a row or rows of data.
*/
void Database::Delete(QList<QString> deleters) {
    currentNam->disconnect();
    QObject::connect(currentNam.get(), SIGNAL(finished(QNetworkReply*)),
                     parent, SLOT(DeletedData(QNetworkReply*)));

    DeleteQuery dq = DeleteQuery(UsingDB, TableName, deleters);
    QByteArray data;
    data.append(dq.QueryString());
    QUrl url(Settings::DELETEURL);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkReply *reply = currentNam->post(req, data);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));
}

/*
  Create creates a JSON string which is compliant with the API to
  create a new table in the database.
*/
void Database::Create(QString jsondata) {
    currentNam->disconnect();
    QByteArray data;
    data.append(jsondata);
    QUrl url(Settings::CREATEURL);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkReply *reply = currentNam->post(req, data);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));
}

/*
  ChangeTable creates a JSON string which is compliant with the API to
  update a cell in the database.
*/
void Database::ChangeTable(QString newdata, QString col, QString id) {
    currentNam->disconnect();
    QObject::connect(currentNam.get(), SIGNAL(finished(QNetworkReply*)),
                     parent, SLOT(UpdatedData(QNetworkReply*)));
    UpdateQuery uq = UpdateQuery(UsingDB, TableName, newdata, col, id);
    QByteArray data;
    data.append(uq.QueryString());
    QUrl url(Settings::UPDATEURL);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkReply *reply = currentNam->post(req, data);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));
}

/*
  ParseMetadata reads through the rawmeta data and assigns as much as
  it can to the key's true base-types.
*/
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

/*
  Not implemented.
*/
void Database::handleNetworkError(QNetworkReply::NetworkError error) {}
