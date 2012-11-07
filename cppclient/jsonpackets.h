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
#include <stdexcept>
#include <iostream>

#include <QTextStream>
#include <QtCore/QString>
#include <QStringList>
#include <QFile>
#include <QByteArray>

static const char* dq = "\"";

/*
  T should be QString or char*

  Returns "T"
*/
template <typename T>
static QString quote(T toQuote) {
    QString ss;
    QTextStream s(&ss);
    s << dq << toQuote << dq;
    return *s.string();
}

/*
  T should be QString or char*

  Returns "key":"value"
*/
template <typename T>
static QString quote(QString key, T value) {
    QString ss;
    QTextStream s(&ss);
    s << dq << key << dq << ":" << dq << value << dq;
    return *s.string();
}

/*
  BaseQuery is used as the base class for all Queries because all Queries
  have the DATABASE/TABLE fields.
*/
class BaseQuery {

public:
    BaseQuery(QString database, QString table) :
        database(database), table(table) {};
    virtual ~BaseQuery() {};

	/*
	  Returns "DATABASE": <database>, "TABLE": <table>"
	*/
    virtual QString QueryString() {
        QString ss;
        QTextStream s(&ss);
        s << quote("DATABASE", database) << ","
          << quote("TABLE", table);
        return *s.string();
    }

protected:
    QString database;
    QString table;
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
        QString ss;
        QTextStream s(&ss);
        s << "{" << BaseQuery::QueryString() << "}";
        return *s.string();
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
        column(column),
        newdata(newdata),
        id(id) {};

	/*
	  Returns:
	  {
	      "COLUMN": <column>,
		  "DATA": <data>,
		  "ID": <id>
	  }
	*/
    QString QueryString() {
        QString ss;
        QTextStream s(&ss);
        s << "{" << BaseQuery::QueryString() << ","
          << quote("COLUMN", column) << ","
          << quote("DATA", newdata) << ","
          << quote("ID", id)
          << "}";
        return *s.string();
    }

private:
    QString column;
    QString newdata;
    QString id;
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
        QString ss;
        QTextStream s(&ss);
        s << "{" << BaseQuery::QueryString() << ","
          << quote("DATA") << ": [";
        for (int x = 0; x < newrowdata.size(); ++x) {
            s << quote(newrowdata[x]);
            if (x != (newrowdata.size() - 1))
                s << ",";
        }
        s << "]}";
        return *s.string();
    }
private:
    QStringList newrowdata;
};

// not implemented yet.
class DeleteQuery :
    public BaseQuery {
public:
    DeleteQuery(QString database, QString table, QList<QString> deleters)
        : BaseQuery(database, table), deleters(deleters) {};

    QString QueryString() {
        QString ss;
        QTextStream s(&ss);
        s << "{" << BaseQuery::QueryString() << ","
          << quote("DATA") << ":" << "[";

        for (int x = 0; x < deleters.size(); ++x) {
            s << quote(deleters[x]);
            if (x + 1 != deleters.size())
                s << ",";
        }
        s << "]}";
        return *s.string();
   }
private:
    QList<QString> deleters;
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

QString cxnstring(int num);

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
