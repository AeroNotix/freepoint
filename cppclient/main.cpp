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

int main(int argc, char *argv[])
{
	// we need the cwd for two things, might as well store it.
	QDir cwd = sgetcwd();
	if (!InitializeSettings(appendDir(cwd, "server.json"))) {
        // handle this here. For now we'll just quit
        throw std::runtime_error("Cannot find/parse server configuration");
    }
	QDir resource = appendDir(cwd, "resources/resource.rcc");
	QResource::registerResource(resource.path());
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
