#include "database.h"
#include <QtGui>

Database::Database(QWidget *parent, QString user, QString passwd,QString using_db, QString table, QString URL)
{
  Parent = parent;
  User = user;
  Password = passwd;
  UsingDB = using_db;
  TableName = table;
  BaseURL = URL;
  ParamURL = BaseURL+"getdb/";
  LoginURL = BaseURL+"login/";
  UpdateURL = BaseURL+"update/";
  InsertURL = BaseURL+"insert/";
}

Database::~Database()
{
    return;
}
