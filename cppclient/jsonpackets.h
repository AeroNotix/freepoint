/*

  jsonpackets.h defines some helper classes which provide simple methods
  of retrieving the commonly creating JSON querystrings by just instant-
  iating a class.

  The hierarchy is simple:

  All <XXX>Query classes inherit BaseQuery simply because they all will
  end up using the DATABASE/TABLE fields and thus should inherit them.

  As for the free functions, they allow us to create key:value pairs
  along with quoting single and double fields.
*/
#ifndef JSONPACKETS_H
#define JSONPACKETS_H

#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include <QtCore/QString>
#include <QStringList>
#include <QFile>
#include <QByteArray>

static const char* dq = "\"";

template <typename T>
static std::string quote(T toQuote) {
    std::stringstream s;
    s << dq << toQuote << dq;
    return s.str();
}

template <typename T>
static std::string quote(std::string key, T value) {
    std::stringstream s;
    s << dq << key << dq << ":" << dq << value << dq;
    return s.str();
}

class BaseQuery {

public:
    BaseQuery(QString database, QString table) :
        database(database.toStdString()), table(table.toStdString()) {};
    virtual ~BaseQuery() {};

    virtual QString QueryString() {
        std::stringstream s;
        s << quote("DATABASE", database) << "," 
          << quote("TABLE", table);
        return QString(s.str().c_str());
    }

protected:
    std::string database;
    std::string table;
};

class GetQuery :
    public BaseQuery {

public:
    GetQuery(QString database, QString table) :
        BaseQuery(database, table) {};

    QString QueryString() {
        std::stringstream s;
        s << "{" << BaseQuery::QueryString().toStdString() << "}";
        return QString(s.str().c_str());
    }
};

class UpdateQuery :
    public BaseQuery {

public:
    UpdateQuery(QString database, QString table, QString newdata, QString column, QString id) :
        BaseQuery(database, table),
        column(column.toStdString()),
        newdata(newdata.toStdString()),
        id(id.toStdString()) {};

    QString QueryString() {
        std::stringstream s;
        s << "{" << BaseQuery::QueryString().toStdString() << "," 
          << quote("COLUMN", column) << ","
          << quote("DATA", newdata) << ","
          << quote("ID", id)
          << "}";
        return QString(s.str().c_str());
    }

private:
    std::string column;
    std::string newdata;
    std::string id;
};

class InsertQuery :
    public BaseQuery {
public:
    InsertQuery(QString database, QString table, QStringList newrowdata)
        : BaseQuery(database, table), newrowdata(newrowdata) {};

    QString QueryString() {
        std::stringstream s;
        s << "{" << BaseQuery::QueryString().toStdString() << ","
          << quote("DATA") << ": [";
        for (int x = 0; x < newrowdata.size(); ++x) {
            s << quote(newrowdata[x].toStdString().c_str());
            if (x != (newrowdata.size() - 1))
                s << ",";
        }
        s << "]}";
        return QString(s.str().c_str());
    }
private:
    QStringList newrowdata;
};


class CreateQuery :
    public BaseQuery {
};

QVariantMap ReadJSONFromFile(QString filename);

class JSONError
	: public std::runtime_error {
public:
	JSONError(std::string s)
		: std::runtime_error(s) {};
};

class JSONParseError 
	: public JSONError {
public:
	JSONParseError(std::string s)
		: JSONError(s) {};
};

class JSONOpenError 
	: public JSONError {
public:
	JSONOpenError(std::string s)
		: JSONError(s) {};
};

#endif //JSONPACKETS_H
