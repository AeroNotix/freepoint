#include <vector>
#include <iostream>
#include <stdexcept>
#include <fstream>

#ifdef _WIN32
    #include "QJson/Parser"
#elif defined __unix__
    #include "qjson/parser.h"
#endif

#include <QActionGroup>
#include <QPointer>
#include <QCoreApplication>
#include <QDebug>
#include <QFileDialog>
#include <QTextStream>

#include "login.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"
#include "table_tools.h"
#include "delegates.h"
#include "jsonpackets.h"
#include "add_new_row.h"
#include "cxn_setup.h"
#include "createnewdatabase.h"
#include "managedialog.h"

/*
  Class ctor.

  This will create the SIGNAL/SLOT connections, parse the configuration
  files and create the login dialog.
*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), parent(parent), ui(new Ui::MainWindow),
      db(nullptr), connections(QList<QVariantMap>()),
      toolbar(addToolBar("toolbar")), current_connection_index(0)
{

    QObject::connect(this, SIGNAL(NewRowSIG(int, int, QTableWidgetItem*)),
                     this, SLOT(NewRow(int, int, QTableWidgetItem*)));
    QObject::connect(this, SIGNAL(InsertRowSIG(int)), this, SLOT(InsertRow(int)));
    QObject::connect(this, SIGNAL(DeleteRowSIG(int)), this, SLOT(DeleteRow(int)));
    QObject::connect(this, SIGNAL(DeleteItemSIG(int, int)), this, SLOT(DeleteItem(int, int)));

    // instantiate the UI, set some options
    ui->setupUi(this);
    ui->tableWidget->setAlternatingRowColors(true);
    setStatusBar(ui->statusbar);

    // Show the login screen
    Login();

    // Parse the config file, if there's a problem with it, re-write
    // it.
    if (!ParseTableConfig()) {
        CXNSetup *cxn = new CXNSetup(this);
        cxn->exec();
        ParseTableConfig();
        delete cxn;
    }
    // If we're here it means we have a config, so set the table and
    // populate it.
    SetCurrentTable();
    PopulateToolbar();
    AddMenuActions();
}

MainWindow::~MainWindow() {
    ClearDelegates();
}

/*
  PopulateTable clears the current data in the table and then requests
  that the database send back a queryset.
*/
void MainWindow::PopulateTable(void) {
    networkRequestPending = true;
    ClearTable();
    db->Query();
}

/*
  ParseTableConfig takes references to the connection map details and
  the list of connections.
*/
bool MainWindow::ParseTableConfig() {
    try {
        connection_map = ReadJSONFromFile(appendDir(sgetcwd(), "config.json").path());
    }
    catch (JSONParseError jerror) {
        throw;
    }
    catch (JSONOpenError jerror) {
        return false;
    }

    connection_names = connection_map["connections"].toStringList();
    return true;
}

/*
  SetCurrentTable creates a new instance of the database if there
  currently isn't one. If there is one it sets which database we're
  currently viewing.
*/
void MainWindow::SetCurrentTable() {
    if (!db) {
        db = QPointer<Database>(
            new Database(this,
                         GetUsername(), GetPassword(),
                         GetDatabase(), GetTable())
            );
    } else {
        db->SetUsername(GetUsername());
        db->SetPassword(GetPassword());
        db->SetTable(GetTable());
        db->SetDatabase(GetDatabase());
    }
    PopulateTable();
}

/*
  RefreshTable gets a new queryset from the table and inserts it into
  the frame.
*/
void MainWindow::RefreshTable() {
    if (networkRequestPending)
        return;
    ClearTable();
    PopulateTable();
}

/*
  Creates an instance of the CXNSetup dialog and shows it. This dialog
  is assigned to a menu item and shouldn't be called programatically.
*/
void MainWindow::openConnectionDialog() {
    CXNSetup *cxn = new CXNSetup(this);
    cxn->exec();
    delete cxn;
}

void MainWindow::openManageDialog() {
    ManageConnectionDialog *mcd = new ManageConnectionDialog(this);
    mcd->exec();
    delete mcd;
}

/*
  When a cell is entered, the User is able to change the data in that
  cell to whatever they like (within the confines of the widget), however
  it's a possibility that the data that they enter will not end up in
  the database so we need to be able to revert the cell to it's previous
  contents. This is what we are doing here.
*/
void MainWindow::storeCell(int x, int y) {
    QTableWidgetItem* cell = ui->tableWidget->item(x, y);
    storedcoords = std::pair<int, int>(x, y);
    if (!cell)
        storeditem = QString("");
    else
        storeditem = ui->tableWidget->item(x, y)->text();
}

/*
  Creates an instance of the AddNewRow dialog and shows it. This dialog
  is assigned to a menu item and shouldn't be called programatically.
*/
void MainWindow::InsertRow() {
    AddNewRow *add = new AddNewRow(headings, delegates, this);
    add->exec();
    delete add;
}

/*
  Pass-through method to call the tableWidget's insertRow method.
*/
void MainWindow::InsertRow(int x) {
    ui->tableWidget->insertRow(x);
}

/*
  AddNewConnection simply opens the connection configuration file and
  adds in a new connection with the database:table entry. This method
  will check for failures and signify an error in that case.
*/
void MainWindow::AddNewConnection(QString database, QString table) {
    QString fname = appendDir(sgetcwd(), "config.json").path();
    if (!WriteJSONConfigFile(connection_names, connection_map, fname, database, table))
        ShowError("Error writing new config file! Contact Administrator.");
}

/*
  Pass-through method to the underlying database to insert a whole new
  row into the currently active table.
*/
void MainWindow::InsertRow(QStringList newrowdata) {
    db->Insert(newrowdata);
}

/*
  When the database returns from inserting a row, this method is invoked
  so we can handle the insertion of multiple rows.
*/
void MainWindow::InsertData(QNetworkReply *reply) {
    QString text = reply->readAll();
    QByteArray json(text.toStdString().c_str());
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(json, &ok).toMap();

    if (!ok || json.size() == 0) {
        ShowError("Malformed data received from server. Contact Administrator.\n\nPossibly the database connection details are incorrect.");
        networkRequestPending = false;
        return;
    }

    QList<QVariant> rawrows_initial = result["Rows"].toList();
    headings = result["Headings"].toStringList();
    ui->tableWidget->setColumnCount(headings.size());
    ui->tableWidget->setHorizontalHeaderLabels(headings);

    if (db->ParseMetadata(result))
        SetDelegates(db->GetMetadata());

    QList<QStringList> rows;
    for (int x = 0; x < rawrows_initial.size(); ++x) {
        rows.append(rawrows_initial[x].toStringList());
    }

    insertRowData(rows);
    networkRequestPending = false;
}

/*
  Convienience method which emits signals to the main event loop to
  both insert new rows and assign data to them.
*/
void MainWindow::insertRowData(QList<QStringList> rows) {
    ui->tableWidget->blockSignals(true);
    int rowno = rows.size();

    for (int x = 0; x < rowno; ++x) {
        emit InsertRowSIG(x);
        for (int y = 0; y < rows[x].size(); ++y) {
            emit NewRowSIG(x, y, new QTableWidgetItem(rows[x][y]));
        }
    }
    ui->tableWidget->blockSignals(false);
}

/*
  When the database returns from inserting rows, this method will be
  invoked to handle the response from the server.
*/
void MainWindow::InsertedRow(QNetworkReply *reply) {
    return GenericHandleResponse(reply);
}

/*
  When a whole row is selected and the backspace or the delete button
  are pressed, we execute a delete on those rows.
*/
void MainWindow::DeleteRows() {
    QList<QString> deleters;
    QList<QModelIndex> raw = ui->tableWidget->selectionModel()->selectedRows();
    for (int x = 0; x < raw.size(); ++x)
        deleters.append(ui->tableWidget->item(raw[x].row(), 0)->text());
    db->Delete(deleters);
}

/*
  When the database returns from deleting rows, this method will be
  invoked to handle the response from the server.
*/
void MainWindow::DeletedData(QNetworkReply *reply) {
    return GenericHandleResponse(reply);
}

/*
  Several replies are handled with the same code. This is a generic
  handler for that.
*/
void MainWindow::GenericHandleResponse(QNetworkReply *reply) {
    QString text = reply->readAll();
    QByteArray json(text.toStdString().c_str());
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(json, &ok).toMap();

    if (!ok || json.size() == 0) {
        ShowError("Malformed data from the server. Contact Administrator.");
        return;
    }

    if (!result["Success"].toBool()) {
        ShowError(result["error"].toString());
        return;
    }

    RefreshTable();
}

/*
  When a cell is changed, this handler is invoked. This is invoked on a
  per-cell basis.
*/
void MainWindow::changeTable(int x, int y) {
    if (x < 0 || y < 0)
        return;

    if (y == 0)
        return RevertCellData();

    db->ChangeTable(
        ui->tableWidget->item(x, y)->text(),
        headings[y],
        ui->tableWidget->item(x,0)->text()
        );
}

/*
  When the database returns from updating a single cell, this method
  is invoked to handle the response.
*/
void MainWindow::UpdatedData(QNetworkReply *reply) {
    QString text = reply->readAll();
    QByteArray json(text.toStdString().c_str());
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(json, &ok).toMap();

    if (!ok || json.size() == 0) {
        RevertCellData();
        return;
    }
    if (!result["Success"].toBool())
        RevertCellData();
    else
        ShowMessage("Database updated successfully", 3000);
}

/*
  If there is a problem either before a cell is sent to be updated
  or after it, we need to be able to revert that cell back to how
  it was before. This is the method which will do that.
*/
void MainWindow::RevertCellData() {
    int x = storedcoords.first;
    int y = storedcoords.second;
    ui->tableWidget->item(x, y)->setText(storeditem);
    ShowMessage("Reverting cell data", 3000);
}

void MainWindow::ChangeConnection(QAction *action) {
    QString connection_name = action->objectName();
    db->SetDatabase(connection_map[connection_name].toMap()["database"].toString());
    db->SetTable(connection_map[connection_name].toMap()["table"].toString());
    PopulateTable();
}

void MainWindow::AddMenuActions() {

    actionGroupConnections = new QActionGroup(this);
    connect(actionGroupConnections, SIGNAL(triggered(QAction*)),
            this, SLOT(ChangeConnection(QAction*)));
    for (int x = 0; x < connection_names.size(); ++x) {

        QString database = connection_map[connection_names[x]].toMap()["database"].toString();
        QString table = connection_map[connection_names[x]].toMap()["table"].toString();

        QAction *action = new QAction(this);
        action->setText(database+"."+table);
        action->setObjectName(connection_names[x]);
        action->setCheckable(true);
        if (connection_names[x] == QString("connection-0"))
            action->setChecked(true);

        ui->menuSelect_Table->addAction(action);
        actionGroupConnections->addAction(action);
    }
}

/*
  Pops open a file dialog and asks the user for a filename to export the
  currently active table to a CSV file.
*/
void MainWindow::ExportAsCSV() {
    QFileDialog *savedialog = new QFileDialog();
    savedialog->setAcceptMode(QFileDialog::AcceptSave);
    savedialog->setDefaultSuffix("csv");
    QObject::connect(savedialog, SIGNAL(fileSelected(QString)), this, SLOT(WriteCSV(QString)));
    savedialog->exec();
    delete savedialog;
}

/*
  When the ExportAsCSV creates a window, it's fileSelected(QString) is
  attached to this method so we can handle if the user really wanted to
  export the table to disk.
*/
void MainWindow::WriteCSV(QString csvfilename) {
    QFile file(csvfilename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        ShowError("Error saving file.");
        return;
    }

    QTextStream s(&file);

    for (int x = 0; x < headings.size(); ++x)
        s << headings[x] << ",";
    s << "\r\n";

    for (int x = 0; x < ui->tableWidget->rowCount(); ++x) {
        for (int y = 0; y < ui->tableWidget->columnCount(); ++y) {
            QTableWidgetItem* cell = ui->tableWidget->item(x, y);
            if (!cell)
                s << ",";
            else
                s << cell->text() << ",";
        }
        s << "\r\n";
    }
}

/*
  Creates an instance of the CreateNewDatabase dialog and shows it.
  This method is assigned to a toolbar button and should not be called
  programatically.
*/
void MainWindow::CreateNewTable() {
    CreateNewDatabase *cnd = new CreateNewDatabase(this);
    cnd->exec();
    delete cnd;
}

void MainWindow::CreateNew(QString jsondata) {
    db->Create(jsondata);
}

/*
  Goes back a table.
*/
void MainWindow::PreviousTable() {
    actionGroupConnections->actions()[current_connection_index]->setChecked(false);
    if ((current_connection_index - 1) < 0)
        current_connection_index = connection_names.size() - 1;
    else
        --current_connection_index;
    actionGroupConnections->actions()[current_connection_index]->setChecked(true);
    SetCurrentTable();
}

/*
  Goes forward a table.
*/
void MainWindow::NextTable() {
    actionGroupConnections->actions()[current_connection_index]->setChecked(false);
    if ((current_connection_index + 1) > connection_names.size() - 1)
        current_connection_index = 0;
    else
        ++current_connection_index;
    actionGroupConnections->actions()[current_connection_index]->setChecked(true);
    SetCurrentTable();
}

/*
  Quits the whole application.
*/
void MainWindow::Exit() {
    QCoreApplication::quit();
}

/*
  Delete items in the table by iterating through the table's items.
*/
void MainWindow::ClearTable() {
    int rows = ui->tableWidget->rowCount();
    int cols = ui->tableWidget->columnCount();

    for (int x = 0; x < rows+1; ++x) {
        for (int y = 0; y < cols; ++y) {
            emit DeleteItemSIG(x, y);
        }
        emit DeleteRowSIG(rows - x);
    }
}

/*
  A convienience method to delete the QTableWidgetItem at a certain
  pair of co-ords.
 */
void MainWindow::DeleteItem(int x, int y) {
    delete ui->tableWidget->itemAt(x, y);
}

/*
  A convienience method to delete the row at a certain co-ord.
 */
void MainWindow::DeleteRow(int x) {
    ui->tableWidget->removeRow(x);
}

/*
  An overriding of the base classes' keyPressEvent method so that we
  may assign functions to certain keys.
*/
void MainWindow::keyPressEvent(QKeyEvent * event) {
    switch (event->key()) {
    case Qt::Key_F5:
        RefreshTable();
        return;
    case Qt::Key_Backspace:
    case Qt::Key_Delete:
        DeleteRows();
        return;
    }

    QWidget::keyPressEvent(event);
}

/*
  This method will be invoked when we receive a signal containing the
  co-ords and an item to insert into the table.
*/
void MainWindow::NewRow(int x, int y, QTableWidgetItem *newrow) {
    ui->tableWidget->setItem(x, y, newrow);
}

/*
  When a new table is parsed for it's metadata, we need to tell the table
  to use certain delegates for certain columns because this way we can
  have much better editing widgets for certain columns.
*/
void MainWindow::SetDelegates(QMetadata metadata) {

    for (int x = 0; x < headings.size(); ++x) {
        QString rowtype = metadata[headings[x]].toMap()["ROWDATA"].toMap()["TYPE"].toString();
        QStringList choices;
        if (rowtype == QString("BOOL") ||
            rowtype == QString("CHOICE")) {
            choices = metadata[headings[x]].toMap()["ROWDATA"].toMap()["CHOICES"].toStringList();
        }
        QItemDelegate *newdelegate = SelectDelegate(rowtype, choices, this);
        delegates.append(newdelegate);
        ui->tableWidget->setItemDelegateForColumn(x, newdelegate);
    }
}

/*
  Method to iterate through all the delegates currently active on the
  current table and delete them.
*/
void MainWindow::ClearDelegates() {
    for (int x = 0; x < delegates.size(); ++x)
        delete delegates[x];
    delegates.clear();
}

/*
  Shows a simple message on the StatusBar for the specified time.
*/
void MainWindow::ShowMessage(const QString &text, int t) {
    ui->statusbar->showMessage(text, t);
}

/*
  Pops open an error dialog with a message and on the StatusBar.
*/
void MainWindow::ShowError(const QString &text) {
    QMessageBox *msgBox = new QMessageBox;
    msgBox->setText(text);
    msgBox->exec();
    ShowMessage(text, 1000);
    delete msgBox;
}

void MainWindow::Login() {
    login::Login *l = new login::Login(this);
    l->exec();
    delete l;
}

/*
  This method creates the actions and assigns them to the toolbar.
*/
void MainWindow::PopulateToolbar() {
    QList<QAction*> Actions;
    Actions.reserve(7);
    Actions.append(create_action(
                       this,
                       "Refresh",
                       "Refreshes the table and it's data.",
                       ":/view-refresh",
                       SLOT(RefreshTable())));
    Actions.append(create_action(
                      this,
                      "Add Row",
                      "Opens the add row dialog so you can insert a now row into the table.",
                      ":/list-add",
                      SLOT(InsertRow())));
    Actions.append(create_action(
                      this,
                      "Export as CSV",
                      "Click here to export the current table to CSV file.",
                      ":/document-save-as",
                      SLOT(ExportAsCSV())));
    Actions.append(create_action(
                       this,
                       "Create new table",
                       "Click here to enter the create new table dialog.",
                       ":/bookmark-new",
                       SLOT(CreateNewTable())));
    Actions.append(create_action(
                      this,
                      "Prev Table",
                      "Click here to go to the previous table.",
                      ":/go-previous",
                      SLOT(PreviousTable())));
    Actions.append(create_action(
                      this,
                      "Next Table",
                      "Click here to go to the next table.",
                      ":/go-next",
                      SLOT(NextTable())));
    Actions.append(create_action(
                      this,
                      "Quit",
                      "Click here to quit.",
                      ":/system-log-out",
                      SLOT(Exit())));

    for (int x = 0; x < Actions.size(); ++x)
        toolbar->addAction(Actions[x]);
}


/*
  Getters/Setters
*/
void MainWindow::SetUsername(const QString &text) {
    username = text;
}

const QString MainWindow::GetUsername(void) const {
    return username;
}

void MainWindow::SetPassword(const QString &text) {
    password = text;
}

const QString MainWindow::GetPassword(void) const {
    return password;
}

const QString MainWindow::GetTable(void) const {
    return connection_map[connection_names[current_connection_index]].toMap()["table"].toString();
}

const QString MainWindow::GetDatabase(void) const {
    return connection_map[connection_names[current_connection_index]].toMap()["database"].toString();
}

const std::pair<QString, QString> MainWindow::GetDBTableInfo(QString connectionname) const {
    return std::pair<QString, QString>(connection_map[connectionname].toMap()["database"].toString(),
                                       connection_map[connectionname].toMap()["table"].toString());
}
