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

    QByteArray data(generateQueryString());
    QUrl url(PARAMURL);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkReply *reply = currentNam->post(req, data);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));
}

const char* Database::generateQueryString() {
    std::stringstream s;
    std::string dq = "\"";
    s << "{" << dq << "DATABASE" << dq << ":" << dq << UsingDB.toStdString()
      << dq << "," << dq << "TABLE" << dq << ":" << dq << TableName.toStdString()
      << dq << "}";
    return s.str().c_str();
}

QList<QString> Database::GetHeadings() {
    return headings;
}

void Database::handleNetworkError(QNetworkReply::NetworkError error) {}
