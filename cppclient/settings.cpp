#include <string>
#include <iostream>

#include <QDir>
#include <QMap>
#include <QString>
#include <QVariant>

#include "settings.h"
#include "jsonpackets.h"
#include "table_tools.h"

QString Settings::SERVERURL;
QString Settings::LOGINURL;
QString Settings::PARAMURL;
QString Settings::UPDATEURL;
QString Settings::INSERTURL;

Settings::Settings(QString filename) {

	QVariantMap result = ReadJSONFromFile(filename);
	QMap<QString, QVariant> settings = result["CONNECTION_DETAILS"].toMap();

	SERVERURL = settings["SERVERURL"].toString();
	LOGINURL = settings["LOGINURL"].toString();
	PARAMURL = settings["PARAMURL"].toString();
	UPDATEURL = settings["UPDATEURL"].toString();
	INSERTURL = settings["INSERTURL"].toString();
}

bool InitializeSettings(QDir filename) {
	try {
		Settings settings(filename.path());
		return true;
	} catch (JSONError jerror) {
		return false;
	}
}
