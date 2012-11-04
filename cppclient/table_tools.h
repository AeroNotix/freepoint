#include <string>

#include <QAction>
#include <QtCore/QString>
#include <QDir>

#include "mainwindow.h"

QAction* create_action(MainWindow *obj, QString text, QString tip, QString fname, const char*);
QDir sgetcwd();
QDir appendDir(QDir base, const char *path);
