#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings {
public:
	Settings(QString filename);
	void OpenSettings(QString filename);
	static QString SERVERURL;
	static QString LOGINURL;
	static QString PARAMURL;
	static QString UPDATEURL;
	static QString INSERTURL;
};

#endif // SETTINGS_H

