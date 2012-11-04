#include <string>

#include <QtGui/QApplication>
#include <QDir>

#include "mainwindow.h"
#include "table_tools.h"
#include "settings.h"

int main(int argc, char *argv[])
{
	QDir cwd = sgetcwd();
	Settings settings(appendDir(cwd, "server.json").path());
	QDir resource = appendDir(cwd, "resources/resource.rcc");
	QResource::registerResource(resource.path());
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
