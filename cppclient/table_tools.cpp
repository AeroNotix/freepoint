#ifdef _WIN32
    #include <direct.h>
#elif defined __unix__
    #include <unistd.h>
#endif

#include <string>
#include <stdexcept>

#include <QDateTime>
#include <QAction>
#include <QtGui>
#include <QIcon>
#include <QDir>

#include "mainwindow.h"

/*
  Create action will instantiate a QAction with the various parameters.

  Remember: This creates a QAction *on the heap* so make sure you delete
  it if you will be not using it anymore. I recommend using a QActionGroup
  so you can keep track of where these go.
*/
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

/*
  Uses the system call getcwd() to find the current working directory
  and returns it as a QDir.
*/
QDir sgetcwd() {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        throw std::runtime_error("Could not find CWD");
    return QDir(QString(cwd));
}

/*
  From QDir we append a string to the end, appending system path separators
  wherever we need.
*/
QDir appendDir(QDir base, std::string path) {
    QString base_path = base.path();
    QChar last = base_path[base_path.size() - 1];
    if (QString(last) != base.separator())
        base_path.append(base.separator());
    return QDir(base_path.append(QString(path.c_str())));
}

/*
  Instantiates a generic Yes/No dialog.
*/
bool AreYouSure() {
    QMessageBox *msgbox = new QMessageBox();
    msgbox->setText("Are you sure?");
    msgbox->addButton(QMessageBox::Yes);
    msgbox->addButton(QMessageBox::No);
    bool ans = msgbox->exec() == QMessageBox::Yes;
    delete msgbox;
    return ans;
}

/*
  Instantiates a MessageBox with message as it's contents.
*/
void MessageBox(QString message) {
    QMessageBox *msgbox = new QMessageBox();
    msgbox->setText(message);
    msgbox->addButton(QMessageBox::Ok);
    msgbox->exec();
    delete msgbox;
}

/*
  Writes what to the log file.

  TODO:: Consider making the log file more featureful and/or add the
  ability to change where logfiles are stored.
*/
void WriteLog(const char *what) {
    QFile file(appendDir(sgetcwd(), "log.txt").path());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;
    QTextStream s(&file);
    s << "Log entry on: "
      << QDateTime::currentDateTime().toString()
      << " with message: "
      << what
      << "\n";
    s.flush();
}
