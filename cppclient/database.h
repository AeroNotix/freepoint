#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <QtCore/QString>
#include <QtGui>

class Database {
public:
    Database(QWidget *parent, QString user, QString passwd,
	     QString using_db, QString table, QString URL);
    ~Database();
    void Connect();
    void Close();
    void Query();
    void Update();
    void Insert();
    QString BaseURL;
    std::vector<QString> GetHeadings();

private:
    QWidget *Parent;
    QString ParamURL;
    QString LoginURL;
    QString UpdateURL;
    QString InsertURL;
    QString TableName;
    QString UsingDB;
    QString Password;
    QString User;
    bool Connected;
    void ChangeTable(int x, int y);
};

#endif // DATABASE_H
