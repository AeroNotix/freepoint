#include <QAction>
#include <QtGui>
#include <QIcon>

#include "mainwindow.h"

QAction* create_action(
    MainWindow *obj, QString text, QString tip, QString fname, std::function<void(void)> f
    )
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
