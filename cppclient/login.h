#ifndef LOGIN_H
#define LOGIN_H

#include <QtGui>
#include "mainwindow.h"
#include "ui_loginbox.h"

namespace login {
    class Login : public QDialog {

	Q_OBJECT

    public:
	explicit Login(MainWindow *parent = 0);
	virtual void accept(void);
	virtual void reject(void);
    private:

	bool login(QString username, QString password) const;
	MainWindow *parent;
	Ui_frm_login *ui;
    };
};
#endif
