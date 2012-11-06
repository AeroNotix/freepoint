#include <vector>
#include <iostream>
#include <stdexcept>
#include <fstream>

#ifdef _WIN32
    #include "QJson/Parser"
#elif defined __unix__
    #include "qjson/parser.h"
#endif

#include <QCoreApplication>

#include "login.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"
#include "table_tools.h"
#include "delegates.h"
#include "jsonpackets.h"
#include "add_new_row.h"
#include "cxn_setup.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      db(nullptr), connections(QList<QVariantMap>()),
      toolbar(addToolBar("toolbar")), current_connection_index(0)
{
    ui->setupUi(this);
    setStatusBar(ui->statusbar);
    Login();
    if (!ParseTableConfig()) {
        CXNSetup cxn(this);
        cxn.exec();
    }
    SetCurrentTable();
    PopulateToolbar();
}

MainWindow::~MainWindow() {
    ClearDelegates();
    delete ui;
    delete db;
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

void MainWindow::SetCurrentTable() {
    if (db == nullptr)
        db = new Database(this);

    db->SetUsername(GetUsername());
    db->SetPassword(GetPassword());
    db->SetTable(GetTable());
    db->SetDatabase(GetDatabase());
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

void MainWindow::openConnectionDialog() {
    throw std::runtime_error("Not implemented! openConnectionDialog");
}

void MainWindow::openManageDialog() {
    throw std::runtime_error("Not implemented! openManageDialog");
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

void MainWindow::InsertRow() {
    AddNewRow a(headings, delegates, this);
    a.exec();
}

void MainWindow::AddNewConnection(QString database, QString table) {
    qDebug() << database << " : " << table << "\n";
}

void MainWindow::InsertRow(QStringList newrowdata) {
    db->Insert(newrowdata);
}

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

void MainWindow::ExportAsCSV() {
    throw std::runtime_error("Not implemented! ExportAsCSV");
}

void MainWindow::CreateNewTable() {
    throw std::runtime_error("Not implemented! CreateNewTable");
}

void MainWindow::PreviousTable() {
    if ((current_connection_index - 1) < 0)
        current_connection_index = connection_names.size() - 1;
    SetCurrentTable();
}

void MainWindow::NextTable() {
    if ((current_connection_index + 1) > connection_names.size() - 1)
        current_connection_index = 0;
    SetCurrentTable();
}

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
        for (int z = 0; z < cols; ++z) {
            delete ui->tableWidget->itemAt(x, z);
        }
        ui->tableWidget->removeRow(rows - x);
    }
}

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

void MainWindow::keyPressEvent(QKeyEvent * event) {
    if (event->key() == Qt::Key_F5)
        RefreshTable();
    else
        QWidget::keyPressEvent(event);
}

void MainWindow::insertRowData(QList<QStringList> rows) {
    ui->tableWidget->blockSignals(true);
    int rowno = rows.size();

    // add required rows
    for (int  x = 0; x < rowno; ++x)
        ui->tableWidget->insertRow(x);

    for (int x = 0; x < rowno; ++x) {
        for (int y = 0; y < rows[x].size(); ++y) {
            ui->tableWidget->setItem(x, y, new QTableWidgetItem(rows[x][y]));
        }
    }
    ui->tableWidget->blockSignals(false);
}

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

void MainWindow::ClearDelegates() {
    for (int x = 0; x < delegates.size(); ++x)
        delete delegates[x];
    delegates.clear();
}

void MainWindow::ShowMessage(const QString &text, int t) {
    ui->statusbar->showMessage(text, t);
}

void MainWindow::ShowError(const QString &text) {
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
    ShowMessage(text, 1000);
}

void MainWindow::Login() {
    login::Login l(this);
    l.exec();
}

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

void MainWindow::RevertCellData() {
    int x = storedcoords.first;
    int y = storedcoords.second;
    ui->tableWidget->item(x, y)->setText(storeditem);
    ShowMessage("Reverting cell data", 3000);
}

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
