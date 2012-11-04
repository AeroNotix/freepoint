#include <vector>
#include <sstream>
#include <iostream>

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
    : parent(parent), User(user), Password(passwd), UsingDB(using_db),
      TableName(table), currentNam(nullptr) {};

void Database::Close() {
    return;
}

void Database::Query() {
    currentNam = new QNetworkAccessManager(this);
    QObject::connect(currentNam, SIGNAL(finished(QNetworkReply*)),
                     parent, SLOT(InsertData(QNetworkReply*)));

    GetQuery gq = GetQuery(UsingDB, TableName);
    QByteArray data(gq.QueryString().toStdString().c_str());
    QUrl url(PARAMURL);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkReply *reply = currentNam->post(req, data);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));
}

void Database::ChangeTable(QString newdata, QString col, QString id) {
    currentNam = new QNetworkAccessManager(this);
    QObject::connect(currentNam, SIGNAL(finished(QNetworkReply*)),
                     parent, SLOT(InsertData(QNetworkReply*)));

    UpdateQuery uq = UpdateQuery(UsingDB, TableName, newdata, col, id);
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

void Database::handleNetworkError(QNetworkReply::NetworkError error) {}
