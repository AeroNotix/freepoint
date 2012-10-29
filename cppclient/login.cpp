#include "login.h"
#include "ui_loginbox.h"
#include <iostream>

login::Login::Login(MainWindow *parent)
    : QDialog(parent), parent(parent), ui(new Ui_frm_login)  {
    this->ui->setupUi(this);
};
  
void login::Login::accept(void) {
    parent->ShowMessage("Logged in", 10000);
    QDialog::accept();
}
