#include "managedialog.h"
#include "jsonpackets.h"
#include "table_tools.h"

ManageConnectionDialog::ManageConnectionDialog(MainWindow *parent)
    : QDialog(parent), parent(parent),
      ui(new Ui_ConnectionsDialog) {
    ui->setupUi(this);z
};

void ManageConnectionDialog::accept() {
    QDialog::accept();
}

void ManageConnectionDialog::reject() {
    QDialog::reject();
}
