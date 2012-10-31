#include <iostream>
#include <sstream>

#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxy>

#include "login.h"
#include "ui_loginbox.h"
#ifdef _WIN32
    #include "QJson/Parser"
#elif defined __unix__
    #include "qjson/parser.h"
#endif
#include "settings.h"


using namespace login;

Login::Login(MainWindow *parent)
    : QDialog(parent), parent(parent),
      ui(new Ui_frm_login), networkRequestPending(false),
      currentNam(nullptr)
{
    ui->setupUi(this);
};

/*
  login is the backend function which creates a network request to the
  server and handles the connection of signals so we can handle the out
  -come of that request.
*/
void Login::login() {
    // We only have a single request pending at a time.
    if (networkRequestPending) {
        return;
    }
    networkRequestPending = true;

    currentNam = new QNetworkAccessManager(this);
    QObject::connect(currentNam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(networkRequestFinished(QNetworkReply*)));

    QByteArray data(generateLoginString());
    QUrl url(LOGINURL);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkReply *reply = currentNam->post(req, data);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));
}

/*
  generateLoginString takes a username and a password object and creates
  the JSON representation of those objects.

  Output is a char* because will use it for HTTP Post data.
*/
const char* Login::generateLoginString() {

    std::string dq = "\"";
    std::stringstream s(std::stringstream::in | std::stringstream::out);
    s << "{" << dq << "USER" << dq << ":" << dq << storedUser.toStdString()
      << dq << "," << dq << "PASSWORD" << dq << ":" << dq << storedPass.toStdString()
      << dq << "}";
    return s.str().c_str();
}

/*
  SLOT:-

  This method should not be called directly.

  This method is asynchronously called via the QNetworkAccessManager
  class.
*/
void Login::networkRequestFinished(QNetworkReply *reply) {

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
        parent->SetUsername(storedUser);
        parent->SetPassword(storedPass);
        parent->ShowMessage("Logged in", 1000);
        QDialog::accept();
        return;
    }
    errorCleanup();
}

/*
  SLOT:-

  We attach a signal to this slot so we can asynchronously cleanup members
  that won't be used because the NetworkRequests fail.
*/
void Login::handleNetworkError(QNetworkReply::NetworkError error) {
    errorCleanup();
}

/*
  Communicate with the server to validate a username/password combo.

  This method mutates the parent's Username and Password members via
  the setter methods.
*/
void Login::accept(void) {
    storedUser = ui->txt_username->text();
    storedPass = ui->txt_password->text();
    login();
}

/*
  If cancel is pressed on the login screen, just quit the whole thing
  as the person shouldn't be able to access the next screen without
  first logging in.
*/
void Login::reject(void) {
    QDialog::reject();
    exit(-1);
}

/*
  Simple method where we can put any and all cleanup into
*/
void Login::errorCleanup() {
    storedUser = QString("");
    storedPass = QString("");
    parent->ShowError("Login failure! Try again.");
}
