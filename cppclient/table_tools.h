#include <functional>

#include <QAction>
#include <QtCore/QString>

#include "mainwindow.h"

QAction* create_action(MainWindow *obj, QString text, QString tip, QString fname, const char*);
