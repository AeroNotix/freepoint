#ifdef _WIN32
    #include <direct.h>
#elif defined __unix__
    #include <unistd.h>
#endif

#include <string>
#include <stdexcept>

#include <QAction>
#include <QtGui>
#include <QIcon>
#include <QDir>

#include "mainwindow.h"

QAction* create_action(MainWindow *obj, QString text, QString tip, QString fname, const char *slot )
{
    QAction *action = new QAction(text, obj);

    QString empty("");
    if (fname != empty)
        action->setIcon(QIcon(fname));
    if (tip != empty) {
        action->setToolTip(tip);
        action->setStatusTip(tip);
    }
    if (slot != nullptr)
        obj->connect(action, SIGNAL(triggered()), SLOT(slot));

    QAction::connect(action, SIGNAL(hovered()), obj, SLOT(ShowMessage(text)));
    return action;
}

QDir sgetcwd() {
	char cwd[256];
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		throw std::runtime_error("Could not find CWD");
	return QDir(QString(cwd));
}

QDir append(QDir base, const char *path) {
	std::string rawpath(base.path().toStdString().append(path));
	return QDir(rawpath.c_str());
}
