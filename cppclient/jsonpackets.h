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

/*
  T should be std::string or char*

  Returns "T"
*/
template <typename T>
static std::string quote(T toQuote) {
    std::stringstream s;
    s << dq << toQuote << dq;
    return s.str();
}

/*
  T should be std::string or char*

  Returns "key":"value"
*/
template <typename T>
static std::string quote(std::string key, T value) {
    std::stringstream s;
    s << dq << key << dq << ":" << dq << value << dq;
    return s.str();
}

/*
  BaseQuery is used as the base class for all Queries because all Queries
  have the DATABASE/TABLE fields.
*/
class BaseQuery {

public:
    BaseQuery(QString database, QString table) :
        database(database.toStdString()), table(table.toStdString()) {};
    virtual ~BaseQuery() {};

	/*
	  Returns "DATABASE": <database>, "TABLE": <table>"
	*/
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

/*
  GetQuery is a little twist on the BaseQuery, since this is used by
  client code we need to wrap the QueryString method in { and }.
*/
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

/*
  UpdateQuery is used when we need to send a message to the server
  about a row which needs to be updated.
*/
class UpdateQuery :
    public BaseQuery {

public:
    UpdateQuery(QString database, QString table, QString newdata, QString column, QString id) :
        BaseQuery(database, table),
        column(column.toStdString()),
        newdata(newdata.toStdString()),
        id(id.toStdString()) {};

	/*
	  Returns:
	  {
	      "COLUMN": <column>,
		  "DATA": <data>,
		  "ID": <id>
	  }
	*/
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

/*
  InsertQuery is used when we need to create a whole row.
*/
class InsertQuery :
    public BaseQuery {
public:
    InsertQuery(QString database, QString table, QStringList newrowdata)
        : BaseQuery(database, table), newrowdata(newrowdata) {};

	/*
	  Returns:

	  {
	      "DATABASE": <database>,
		  "TABLE": <table>,
		  "DATA": [array, of, strings],
      }
	*/
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

// not implemented yet.
class CreateQuery :
    public BaseQuery {
};

QVariantMap ReadJSONFromFile(QString filename);
bool WriteJSONConfigFile(QStringList connection_names, QVariantMap connection_map,
                         QString filename, QString database = 0, QString table = 0);
bool WriteJSONServerFile(QString base, QString login, QString param, QString update,
                         QString insert, QString del, QString create, QString filename);

std::string cxnstring(int num);

/*

  A few Exception classes to throw when various fail states happen in
  using the classes/functions in this module

*/

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
