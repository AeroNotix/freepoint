
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
