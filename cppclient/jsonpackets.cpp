#include <iostream>

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
	QByteArray rawstring;
	
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QString s = "Failure to open: ";
		s.append(filename);
		throw std::runtime_error(s.toStdString());	
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
		throw std::runtime_error(s.toStdString());
	}

	return results;
}
