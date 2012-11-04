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

QAction* create_action(MainWindow *obj, QString text, QString tip, QString fname, const char *slot)
{
    QAction *action = new QAction(text, obj);
    QString empty("");
    if (fname != empty)
        action->setIcon(QIcon(fname));
    if (tip != empty) {
        action->setToolTip(tip);
        action->setStatusTip(tip);
    }
    obj->connect(action, SIGNAL(triggered()), obj, slot);
    return action;
}

QDir sgetcwd() {
	char cwd[256];
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		throw std::runtime_error("Could not find CWD");
	return QDir(QString(cwd));
}

QDir appendDir(QDir base, std::string path) {
	QString base_path = base.path();
	QChar last = base_path[base_path.size() - 1];
	if (QString(last) != QString(path[path.size() - 1]))
		base_path.append(base.separator());
	return QDir(base_path.append(QString(path.c_str())));
}
