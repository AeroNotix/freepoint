#include <string>

#include <QtGui/QApplication>
#include <QDir>

#include "mainwindow.h"
#include "table_tools.h"

int main(int argc, char *argv[])
{
	QDir resource = appendDir(sgetcwd(), "resources/resource.rcc");
	QResource::registerResource(resource.path());
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
