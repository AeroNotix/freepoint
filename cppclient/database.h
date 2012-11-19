/*
 *
 * This module is a reference implementation of talking with the API.
 *
 * The design of this means that the database can be swapped out on
 * GUI relatively easy, since a lot of the operations are simply VOID.
 * and are supposed to have slots assigned to the finshed() signal of
 * the QNetworkAccessManager.
 *
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>
#include <memory>

#include <QList>
#include <QtGui>
#include <QtCore/QString>
#include <QMap>
#include <QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "settings.h"

typedef QMap<QString, QVariant> QMetadata;

/*
  Class Database encapsulates the communication with the database.

  The idea behind this is that we could, and should be able to substi-
  tute another conforming class for this one.
*/
class Database :
    public QObject {

    Q_OBJECT

public:
    Database(QWidget *parent, QString user, QString passwd,
             QString using_db, QString table);
    ~Database() {};

    void Close();
    void Update();
    /**
     * Insert creates a JSON string which is compliant with the API to
     * insert a completely new row of data.
     */
    void Insert(QStringList newrowdata);
    /**
     * @brief Query creates a JSON string which is compliant with the API to
     * retrieve the currently active table.
     */
    void Query();
    /**
     * @brief Delete creates a JSON string which is compliant with the API to
     * delete either a row or rows of data.
     */
    void Delete(QList<QString> deleters);
    /**
     * @brief Create creates a JSON string which is compliant with the API to
     * create a new table in the database.
     */
	void Create(QString jsondata);
    /**
     * @brief ChangeTable creates a JSON string which is compliant with the API to
     * update a cell in the database.
     */
    void ChangeTable(QString newdata, QString col, QString id);
    /**
     * @brief ParseMetadata reads through the rawmeta data and assigns as much as
     * it can to the key's true base-types.
     */
    bool ParseMetadata(QMap<QString, QVariant>);
    QMetadata GetMetadata() {
        return metadata;
    };

    void SetUsername(const QString &Username) {
        User = Username;
    }
    const QString GetUsername(void) const {
        return User;
    }
    void SetPassword(const QString &Pass) {
        Password = Pass;
    }
    const QString GetPassword(void) const {
        return Password;
    }
    void SetTable(const QString &Table) {
        TableName = Table;
    }
    void SetDatabase(const QString &Database) {
        UsingDB = Database;
    }

public slots:
    void handleNetworkError(QNetworkReply::NetworkError);

private:
    QWidget *parent;
    QString User;
    QString Password;
    QString UsingDB;
    QString TableName;
    bool Connected;
    QList<QList<QString> > queryset;
    std::unique_ptr<QNetworkAccessManager> currentNam;
    QMetadata metadata;
};

#endif // DATABASE_H
