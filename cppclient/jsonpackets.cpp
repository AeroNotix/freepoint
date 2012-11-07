#include <iostream>
#include <sstream>

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

bool WriteJSONConfigFile(QStringList connection_names, QVariantMap connection_map,
                         QString filename, QString database, QString table)
{
    std::stringstream s;
    s << "{\n\t";

    // if the connection_names has a size, then it must be a pre-existing
    // config file, pre-existing files should all have version strings.
    if (connection_names.size())
        s << quote("version", connection_map["version"].toString().toStdString())
          << ",\n\t";
    else
        s << quote("version", "0.1");

    // we need x outside the loop, so we declare it here.
    int x;
    for (x = 0; x < connection_names.size(); ++x) {
        // we use the map twice so we'll just store it.
        QVariantMap xmap = connection_map[connection_names[x]].toMap();

        // we're using newlines and tabs because the config file should be
        // human readable.
        s << quote(cxnstring(x)) << ": {\n\t\t";
        s << quote("database", xmap["database"].toString().toStdString())
          << ",\n\t\t";
        s << quote("table", xmap["table"].toString().toStdString())
          << "\n\t}";
        // if we're on the last one, don't insert a comma.
        if (x + 1 != connection_names.size())
            s << ",\n\t";
    }

    // we're simply re-writing the config file if we weren't supplied with
    // additional database/table strings.
    //
    // Otherwise we've got an additional database/table combo to store.
    QString empty = QString("");
    if (database != empty) {
        s << ",\n\t";
        s << quote(cxnstring(x)) << ": {\n\t\t";
        s << quote("database", database.toStdString()) << ",\n\t\t";
        s << quote("table", table.toStdString()) << "\n\t";
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

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << QString(s.str().c_str());
    out.flush();
    return true;
}

bool WriteJSONServerFile(
    QString base, QString login,
    QString param, QString update,
    QString insert, QString del,
    QString create, QString filename)
{
    std::stringstream s;
    std::string le = ",\n\t\t";
    std::string dq = "\"";
    s << "{\n";
    s << "\t" << quote("CONNECTION_DETAILS") << ":{\n\t\t";
    s << quote("SERVERURL") << ":" << dq << base.toStdString() << dq << ",\n\t\t";
    s << quote("LOGINURL") << ":" << dq << base.toStdString() << login.toStdString() << "/" << dq << le;
    s << quote("PARAMURL") << ":" << dq << base.toStdString() << param.toStdString() << "/" << dq << le;
    s << quote("UPDATEURL") << ":" << dq << base.toStdString() << update.toStdString() << "/" << dq << le;
    s << quote("INSERTURL") << ":" << dq << base.toStdString() << insert.toStdString() << "/" << dq << le;
    s << quote("DELETEURL") << ":" << dq << base.toStdString() << del.toStdString() << "/" << dq << le;
    s << quote("CREATEURL") << ":" << dq << base.toStdString() << create.toStdString() << "/" << dq << "\n\t}\n}\n";
    s.flush();

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << QString(s.str().c_str());
    out.flush();
    return true;
}

std::string cxnstring(int num) {
    std::stringstream s;
    s << "connection-" << num;
    return s.str();
}
