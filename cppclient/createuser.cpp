#include "createuser.h"

#ifdef _WIN32
    #include "QJson/Parser"
#elif defined __unix__
    #include "qjson/parser.h"
#endif

#include "settings.h"
#include "jsonpackets.h"
#include "mainwindow.h"
#include "requests.h"

CreateUser::CreateUser(MainWindow *parent)
    : QDialog(parent), parent(parent),
      ui(new Ui_CreateUser), networkRequestPending(false),
      currentNam(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
	currentNam->setCookieJar(new QNetworkCookieJar(this));
	currentNam->cookieJar()->setCookiesFromUrl(parent->GetCookies(), QUrl(Settings::SERVERURL));
}

void CreateUser::Create() {
	QUrl url(Settings::USERURL);
	QObject::connect(currentNam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(networkRequestFinished(QNetworkReply*)));
	QByteArray data;
	data.append(generateCreateUserString());
	QNetworkRequest req(url);
	SetHeaders(req);
	QNetworkReply *reply = currentNam->post(req, data);
	QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));
}

QString CreateUser::generateCreateUserString() {
    QString ss;
    QTextStream s(&ss);
    s << "{"
      << quote("EMAIL", ui->txt_email->text())
      << ","
      << (ui->chk_generate->checkState() == Qt::Unchecked ?
		quote("PASSWORD", ui->txt_password->text()) :
		  "\"GENERATE\":true");
	s << "}";

    return *s.string();
}

void CreateUser::networkRequestFinished(QNetworkReply *reply) {
	QString body = reply->readAll();
	qDebug() << body;
	QByteArray json(body.toStdString().c_str());
	QJson::Parser parser;
	bool ok;
	QVariantMap result = parser.parse(json, &ok).toMap();
	if (!ok || json.size() == 0) {
		parent->ShowError("Malformed data received from the server. Contact Administrator.");
	}
	if (result.contains("error")) {
		parent->ShowError(result["error"].toString());
		return;
	}
	QDialog::accept();
}

void CreateUser::handleNetworkError(QNetworkReply::NetworkError e) {
	qDebug() << e;
}

void CreateUser::accept(void) {
    if (ui->chk_generate->checkState() == Qt::Unchecked) {
        if (ui->txt_password->text().size() < 6 ||
            ui->txt_password->text().size() < 6) {
            parent->ShowError("Password is too small.");
			return;
        }
        if (ui->txt_password->text() != ui->txt_password2->text()) {
            parent->ShowError("Passwords do not match.");
            return;
        }
    }
    Create();
}

void CreateUser::reject(void) {
    QDialog::reject();
}

void CreateUser::togglePassword(int i) {
    ui->txt_password->setEnabled(i == Qt::Unchecked);
    ui->txt_password2->setEnabled(i == Qt::Unchecked);
}
