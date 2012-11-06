#include "cxn_setup.h"

CXNSetup::CXNSetup(MainWindow *parent)
    : QDialog(parent),
      parent(parent), ui(new Ui_CXNSetup)
{
    ui->setupUi(this);
}

void CXNSetup::accept() {
    if (parent)
        parent->AddNewConnection(ui->txt_database->text(), ui->txt_table->text());
    QDialog::accept();
}

void CXNSetup::reject() {
    QDialog::reject();
    exit(-1);
}
