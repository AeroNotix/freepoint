#include <iostream>

#include <QTextStream>
#include <QFile>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QByteArray>

#ifdef _WIN32
    #include "QJson/Parser"
#elif defined __unix__
    #include "qjson/parser.h"
#endif

#include "jsonpackets.h"

static const char* dq = "\"";

/*
  Quotes a QString as well as a T to:

  "key" : "T"
*/
template <typename T>
QString quote(QString key, T value) {
    QString ss;
    QTextStream s(&ss);
    s << quote(key) << ":" << quote(value);
    return *s.string();
}

// Total template specification because int wants to be trate a little
// differently.
template <>
QString quote(QString key, int value) {
    QString ss;
    QTextStream s(&ss);
    s << quote(key) << ":" << quote(value);
    return *s.string();
}

// Quotes a single T in double quotes.
template <typename T>
QString quote(T toQuote) {
    QString ss;
    QTextStream s(&ss);
    s << dq << toQuote << dq;
    return *s.string();
}

/*
  Reads a JSON file and returns the parsed version of it as a QVariantMap
*/
QVariantMap ReadJSONFromFile(QString filename) {
    QFile file(filename);
    if (!file.exists()) {
        QString s = "File does not exist: ";
        s.append(filename);
        throw JSONOpenError(s.toStdString());
    }

    QByteArray rawstring;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString s = "Failure to open: ";
        s.append(filename);
        throw JSONOpenError(s.toStdString());
    }

    while (!file.atEnd()) {
        rawstring.append(file.readLine());
    }

    QJson::Parser parser;
    bool ok;
    QVariantMap results = parser.parse(rawstring, &ok).toMap();
    if (!ok) {
        QString s = "Failure to parse: ";
        s.append(filename);
        throw JSONParseError(s.toStdString());
    }

    return results;
}

/*
  Writes a JSON config file back to disk. Uses a list of connections
  and the map from which to write back. As well as an additional new pair
  of QStrings. If either of those are missing it will just write the old
  data in to the file.
*/
bool WriteJSONConfigFile(QStringList connection_names, QVariantMap connection_map,
                         QString filename, QString database, QString table)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream s(&file);

    s << "{\n\t";

    // if the connection_names has a size, then it must be a pre-existing
    // config file, pre-existing files should all have version strings.
    if (connection_names.size())
        s << quote("version", connection_map["version"].toString());
    else
        s << quote("version", "0.1");
    s  << ",\n\t";

    // we need x outside the loop, so we declare it here.
    int x;
    for (x = 0; x < connection_names.size(); ++x) {
        // we use the map twice so we'll just store it.
        QVariantMap xmap = connection_map[connection_names[x]].toMap();

        // we're using newlines and tabs because the config file should be
        // human readable.
        s << quote(cxnstring(x)) << ": {\n\t\t";
        s << quote("database", xmap["database"].toString())
          << ",\n\t\t";
        s << quote("table", xmap["table"].toString())
          << "\n\t}";
        s << ",\n\t";
    }

    // we're simply re-writing the config file if we weren't supplied with
    // additional database/table strings.
    //
    // Otherwise we've got an additional database/table combo to store.
    QString empty = QString("");
    if (database != empty) {
        s << quote(cxnstring(x)) << ": {\n\t\t";
        s << quote("database", database) << ",\n\t\t";
        s << quote("table", table) << "\n\t";
        s << "},";
        // we increment x because we need to write out the names of the
        // connections, if we don't increment x it will write out just the
        // names of the pre-existing connections.
        x++;
    }

    s << "\n\t";
    s << quote("connections") << ": [";
    for (int z = 0; z < x; ++z) {
        s << quote(cxnstring(z));
        if (z + 1 != x)
            s << ",";
    }
    s << "]\n}\n";
    s.flush();
    return true;
}

/*
  Writes the server JSON config file back to disk
*/
bool WriteJSONServerFile(
    QString base, QString login,
    QString param, QString update,
    QString insert, QString del,
    QString create, QString user,
	QString filename)
{

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream s(&file);
    QString le = "/\",\n\t\t";
    QString dq = "\"";
    QString qtdbase = ":\"";
    qtdbase.append(base);

    s << "{\n"
      << "\t" << quote("CONNECTION_DETAILS") << ":{\n\t\t"
      << quote("SERVERURL") << qtdbase << dq << ",\n\t\t"
      << quote("LOGINURL") << qtdbase << login << le
      << quote("PARAMURL") << qtdbase << param << le
      << quote("UPDATEURL") << qtdbase << update << le
      << quote("INSERTURL") << qtdbase << insert << le
      << quote("DELETEURL") << qtdbase << del << le
	  << quote("USERURL") << qtdbase << user << le
      << quote("CREATEURL") << qtdbase << create << "/" << dq << "\n\t}\n}\n";
    s.flush();
    return true;
}

QString cxnstring(int num) {
    QTextStream s(new QString(""));
    s << "connection-" << num;
    return *s.string();
}
