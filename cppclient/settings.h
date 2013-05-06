#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings {
public:
    Settings(QString filename);
    void OpenSettings(QString filename);
    static QString SERVERURL; /*!< The base URL for which the server should be accessed */
    static QString LOGINURL;  /*!< The URL where the login endpoint resides */
    static QString PARAMURL;  /*!< The URL where the database parameters reside */
    static QString UPDATEURL; /*!< The URL where updates to the database are requested */
    static QString INSERTURL; /*!< The URL where inserts to the database are requested */
    static QString DELETEURL; /*!< The URL where deletes to the database are requested */
    static QString CREATEURL; /*!< The URL where creates to the database are requested */
	static QString USERURL;   /*!< The URL where new users are created via */
};

bool InitializeSettings(QDir filename);

#endif // SETTINGS_H
