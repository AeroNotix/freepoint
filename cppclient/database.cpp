#include <vector>

#include <QtGui>
#include <QtCore/QString>

#include "database.h"

Database::Database
(QWidget *parent, QString user, QString passwd, QString using_db, QString table)
    : Parent(parent), User(user), Password(passwd), UsingDB(using_db),
      TableName(table) {};

Database::~Database() {
    return;
}

void Database::Close() {
    return;
}

QList<QList<QString> > Database::Query() {
    // Mock data until I write the json/db interface.
    //
    // the db->gui interface is working, it's just the server->db
    // interface I need to write.
    headings = {"ID", "TEST"};
    queryset = {
        {"1", "DATAZ"},
        {"2", "OVA DATAZ"}
    };
    return queryset;
}

QList<QString> Database::GetHeadings() {
    return headings;
}
