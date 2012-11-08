
#include <QDebug>
#include "createnewdatabase.h"


CreateNewDatabase::CreateNewDatabase(MainWindow *parent)
    : QDialog(parent), ui(new Ui_CreateNewDatabase),
      currentNam(nullptr)
{
    ui->setupUi(this);
}

void CreateNewDatabase::accept() {
    QDialog::accept();
}

void CreateNewDatabase::reject() {
    QDialog::reject();
}

void CreateNewDatabase::changeFieldDescriptions(int i) {
    ui->stackedWidget->setCurrentIndex(~i-1);
}

void CreateNewDatabase::acceptFieldAdd() {

    enum FieldName {
        text,
        choice,
        date,
        time,
        group,
        currency
    };

    switch(ui->stackedWidget->currentIndex())
    {
    case text:
        qDebug() << "text!\n";
        break;
    case choice:
        qDebug() << "choice!\n";
        break;
    case date:
        qDebug() << "date!\n";
        break;
    case time:
        qDebug() << "time!\n";
        break;
    case group:
        qDebug() << "group!\n";
        break;
    case currency:
        qDebug() << "currency!\n";
        break;
    }
}
