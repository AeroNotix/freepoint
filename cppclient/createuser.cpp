#include "createuser.h"

#ifdef _WIN32
    #include "QJson/Parser"
#elif defined __unix__
    #include "qjson/parser.h"
#endif

#include "settings.h"
#include "jsonpackets.h"

CreateUser::CreateUser(MainWindow *parent)
    : QDialog(parent), parent(parent),
      ui(new Ui_CreateUser), networkRequestPending(false),
      currentNam(nullptr)
{
    ui->setupUi(this);
}

void CreateUser::Create() {
    qDebug() << generateCreateUserString();
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
    qDebug() << reply;
}

void CreateUser::handleNetworkError(QNetworkReply::NetworkError) {
}

void CreateUser::accept(void) {
    if (ui->chk_generate->checkState() == Qt::Unchecked) {
        if (ui->txt_password->text().size() < 6 ||
            ui->txt_password->text().size() < 6) {
            parent->ShowError("Password is too small.");
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
