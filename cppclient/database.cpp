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


#include "database.h"
#include "settings.h"

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

    QByteArray data(generateQueryString().c_str());
    QUrl url(PARAMURL);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkReply *reply = currentNam->post(req, data);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));
}

std::string Database::generateQueryString() {
    std::stringstream s;
    std::string dq = "\"";
    s << "{" << dq << "DATABASE" << dq << ":" << dq << UsingDB.toStdString()
      << dq << "," << dq << "TABLE" << dq << ":" << dq << TableName.toStdString()
      << dq << "}";
    return s.str();
}

QList<QString> Database::GetHeadings() {
    return headings;
}

void Database::ChangeTable(QTableWidgetItem *item) {
    if (item)
        std::cout << item->text().toStdString() << std::endl;
}

void Database::handleNetworkError(QNetworkReply::NetworkError error) {}
