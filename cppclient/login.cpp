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
    QByteArray data(generateLoginString(username, password));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    nam->post(req,data);
}

const char* login::Login::generateLoginString(QString username, QString password) {

    // refactor this shit.... use a string buffer.
    std::string user = username.toStdString();
    std::string pass = password.toStdString();
    std::string out = "{\"USER\":\"";
    out.append(user);
    out.append("\",\"PASSWORD\":\"");
    out.append(pass);
    out.append("\"}");
    return out.c_str();
}

/*
  This method should not be called directly.

  This method is asynchronously called via the QNetworkAccessManager
  class.
*/
void login::Login::networkRequestFinished(QNetworkReply *reply) {

    networkRequestPending = false;
    QString text = reply->readAll();
    QByteArray json(text.toStdString().c_str());
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(json, &ok).toMap();

    if (!ok) {
	parent->ShowError("Malformed data received from server. Contact Administrator.");
    }

    bool loginStatus = result["Success"].toBool();
    if (loginStatus) {
	networkRequestPending = false;
	parent->SetUsername(storedUser);
	parent->SetPassword(storedPass);
	parent->ShowMessage("Logged in", 1000);
	QDialog::accept();
	return;
    }

    storedUser = QString("");
    storedPass = QString("");
    parent->ShowError("Login failure! Try again.");
}

/*
  Communicate with the server to validate a username/password combo.

  This method mutates the parent's Username and Password members via
  the setter methods.
*/
void login::Login::accept(void) {
    QString storedUser = ui->txt_username->text();
    QString storedPass = ui->txt_password->text();
    login(storedUser, storedPass);
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
