#include "login.h"
#include "ui_loginbox.h"
#include <iostream>
#include "qjson/parser.h"

login::Login::Login(MainWindow *parent)
    : QDialog(parent), parent(parent), ui(new Ui_frm_login)  {
    ui->setupUi(this);
};

bool login::Login::login(QString username, QString password) const {
    /*
      Here we would do something like this along with pinging the
      server to get auth status.

      QJson::Parser parser;
      bool ok;
      QVariant result = parser.parse(json, &ok);
    */
    return true;
}

/*
  Communicate with the server to validate a username/password combo.

  This method mutates the parent's Username and Password members via
  the setter methods.
*/
void login::Login::accept(void) {

    QString username = ui->txt_username->text();
    QString password = ui->txt_password->text();

    if (!login(username, password)) {
	    return parent->ShowError(QString("Login Failure!"));
	}
    parent->SetUsername(username);
    parent->SetPassword(password);
    parent->ShowMessage("Logged in", 1000);
    QDialog::accept();
}

/*
  If cancel is pressed on the login screen, just quit the whole thing
  as the person shouldn't be able to access the next screen without
  first logging in.
*/
void login::Login::reject(void) {
    QDialog::reject();
    exit(-1);
}
