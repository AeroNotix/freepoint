#ifndef LOGIN_H
#define LOGIN_H

#include <QtGui>
#include "mainwindow.h"
#include "ui_loginbox.h"

// fwd decl
class QNetworkReply;

namespace login {
    class Login : public QDialog {

	Q_OBJECT

    public:
	explicit Login(MainWindow *parent = 0);
	virtual void accept(void);
	virtual void reject(void);
    public slots:
	void networkRequestFinished(QNetworkReply*);
    private:
	void login(QString username, QString password);
	MainWindow *parent;
	Ui_frm_login *ui;
	QString username;
	QString password;
	bool networkRequestPending;
    };
};
#endif
