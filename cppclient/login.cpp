#include "login.h"
#include "ui_loginbox.h"
#include <iostream>

login::Login::Login(MainWindow *parent)
    : QDialog(parent), parent(parent), ui(new Ui_frm_login)  {
    ui->setupUi(this);
};

/*
  Communicate with the server to validate a username/password combo.

  This method mutates the parent's Username and Password members via
  the setter methods.
*/
void login::Login::accept(void) {

    parent->SetUsername(ui->txt_username->text());
    parent->SetPassword(ui->txt_password->text());

    parent->ShowMessage("Logged in", 10000);
    QDialog::accept();
}
