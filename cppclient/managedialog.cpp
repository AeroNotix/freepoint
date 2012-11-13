#include "managedialog.h"

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
