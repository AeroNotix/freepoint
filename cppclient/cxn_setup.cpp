#include "cxn_setup.h"

CXNSetup::CXNSetup(CXNSetup::WindowMode mode, MainWindow *parent)
    : QDialog(parent),
      parent(parent), ui(new Ui_CXNSetup), mode(mode)
{
    ui->setupUi(this);
}

void CXNSetup::accept() {
    if (parent)
        parent->AddNewConnection(ui->txt_database->text(), ui->txt_table->text());
    QDialog::accept();
}

void CXNSetup::reject() {
    switch (mode) {
    case CXNSetup::StandAlone:
        QDialog::reject();
        exit(-1);
    case CXNSetup::Dialog:
        QDialog::reject();
    };
}
