/// The dialog with which to add new connections to a user's config.
#ifndef CXN_SETUP_H
#define CXN_SETUP_H

#include <iostream>
#include <string>

#include <QtGui>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "mainwindow.h"
#include "ui_cxn_setup.h"


/**
 * @brief Creates a dialog window to add new connections.
 */
class CXNSetup :
    public QDialog {

    Q_OBJECT

public:
    enum WindowMode {
        StandAlone, /*!< Sets the mode to kill the application when the dialog is closed. */
        Dialog      /*!< Sets the mode to a modal dialog. */
    };

   /**
    * @brief Constructor.
    * @param mode selects which type of window to use. This determines
    * how the reject() method is invoked. It will either kill the
    * application (CXNSetup::StandAlone) or it will simply reject
    * (CXNSetup::Dialog).
    * @param parent this is a pointer to the parent object. All QObjects
    * need a pointer to their parent so they can set who they are managed
    * by.
    */
    explicit CXNSetup(CXNSetup::WindowMode mode, MainWindow *parent = 0);
    virtual ~CXNSetup() {
        delete ui;
    }

   /**
    * @brief Accepts the dialog.
    */
    void accept();
   /**
    * @brief Rejects the dialog.
    */
    void reject();

private:
    MainWindow *parent;
    Ui_CXNSetup *ui;
    CXNSetup::WindowMode mode;
};
#endif
