/*
  Main entry point for the program.

  Instantiates all the required GUI classes and enters the
  event loop.

  We also instantiate a Settings instance, this is supposed to be a
  singleton class due to requiring a global namespace containing server
  values which should be  ble to be changed at runtime hence not using
  #defines.
 */
#include <stdexcept>

#include <QtGui/QApplication>
#include <QDir>

#include "mainwindow.h"
#include "table_tools.h"
#include "settings.h"
#include "server_setup.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	// we need the cwd for two things, might as well store it.
	QDir cwd = sgetcwd();
	if (!InitializeSettings(appendDir(cwd, "server.json"))) {
        ServerSetup setup;
        setup.exec();
        InitializeSettings(appendDir(cwd, "server.json"));
    }
	QDir resource = appendDir(cwd, "resources/resource.rcc");
	QResource::registerResource(resource.path());
    MainWindow w;
    w.show();
    return a.exec();
}
