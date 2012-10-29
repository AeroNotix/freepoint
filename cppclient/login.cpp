#include <iostream>

#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxy>

#include "login.h"
#include "ui_loginbox.h"
#include "qjson/parser.h"
#include "settings.h"

login::Login::Login(MainWindow *parent)
    : QDialog(parent), parent(parent), ui(new Ui_frm_login), networkRequestPending(false)  {
    ui->setupUi(this);
};

void login::Login::login(QString username, QString password) {
    /*
      Here we need to encode the username/password into the JSON string.
    */

    // make it so we only have a single request pending at a time.
    if (networkRequestPending) {
	return;
    }
    networkRequestPending = true;
    QNetworkAccessManager* nam;
    nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)),
		     this, SLOT(networkRequestFinished(QNetworkReply*)));

    QUrl url(LOGINURL);
    QNetworkReply* reply = nam->get(QNetworkRequest(url));
}

/*
  This method should not be called directly.

  This method is asynchronously called via the QNetworkAccessManager
  class.
*/
void login::Login::networkRequestFinished(QNetworkReply *reply) {

    /*
      Here we would need to check the incoming reply for the auth
      status with something like the below:

      QJson::Parser parser;
      bool ok;
      QVariant result = parser.parse(json, &ok);

      This also needs to set the members on the parent object. i.e.
      username/password.
    */

    // for now we just pretend we've authenticated.
    networkRequestPending = false;
    parent->ShowMessage("Logged in", 1000);
    QDialog::accept();
}

/*
  Communicate with the server to validate a username/password combo.

  This method mutates the parent's Username and Password members via
  the setter methods.
*/
void login::Login::accept(void) {
    QString username = ui->txt_username->text();
    QString password = ui->txt_password->text();
    login(username, password);
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
