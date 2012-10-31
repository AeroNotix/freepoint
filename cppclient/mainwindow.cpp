#include <unistd.h>

#include <vector>
#include <iostream>
#include <stdexcept>
#include <fstream>

#ifdef _WIN32
    #include "QJson/Parser"
#elif defined __unix__
    #include "qjson/parser.h"
#endif

#include "login.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      db(nullptr), connections(QList<QVariantMap>())
{
    ui->setupUi(this);
    setStatusBar(ui->statusbar);
    SetCurrentTable();
    Login();
    PopulateTable();
}

MainWindow::~MainWindow() {
    ClearTable();
    delete ui;
    delete db;
}

void MainWindow::PopulateTable(void) {
    networkRequestPending = true;
    ClearTable();
    if (db != nullptr)
        db->Query();
    else
        ShowError("Database has not been initialized!");
}

/*
  SetCurrentTable
*/
bool MainWindow::SetCurrentTable() {

    // possibly move this into a Util library
    //
    // Check if this compiles on windows.
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        ShowError("Error finding CWD. Contact Administrator.");
        exit(-1);
    }

    // Get a filepath lib
    std::string cwds(cwd);
    cwds.append("/");
    cwds.append("config.json");

    std::ifstream fs(cwds);
    std::string fcontent, s;
    while (fs.good()) {
        std::getline(fs, s);
        fcontent.append(s);
    }
    QByteArray json(fcontent.c_str());
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(json, &ok).toMap();
    if (!ok)
        return false;
    QStringList connlist = result["connections"].toStringList();
    for (int x = 0; x < connlist.size(); ++x) {
        connections.append(result[connlist[x]].toMap());
    }
    return true;
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
    blockSignals(true);
    QString text = reply->readAll();
    QByteArray json(text.toStdString().c_str());
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(json, &ok).toMap();

    if (!ok) {
        ShowError("Malformed data received from server. Contact Administrator.");
    }

    QList<QVariant> rawrows_initial = result["Rows"].toList();
    headings = result["Headings"].toStringList();
    ui->tableWidget->setColumnCount(headings.size());
    ui->tableWidget->setHorizontalHeaderLabels(headings);

    QList<QStringList> rows;

    for (int x = 0; x < rawrows_initial.size(); ++x) {
        rows.append(rawrows_initial[x].toStringList());
    }

    insertRowData(rows);
    blockSignals(true);
    networkRequestPending = false;
}

void MainWindow::keyPressEvent(QKeyEvent * event) {
    if (event->key() == Qt::Key_F5)
        RefreshTable();
    else
        QWidget::keyPressEvent(event);
}

void MainWindow::insertRowData(QList<QStringList> rows) {

    unsigned int rowno = rows.size();

    // add required rows
    for (unsigned int  x = 0; x < rowno; ++x)
        ui->tableWidget->insertRow(x);

    for (unsigned int x = 0; x < rowno; ++x) {
        for (unsigned int y = 0; y < rows[x].size(); ++y) {
            ui->tableWidget->setItem(x, y, new QTableWidgetItem(rows[x][y]));
        }
    }
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

void MainWindow::RevertCellData(int x, int y) {
    if (x && y) {} // nop
    ShowMessage("RevertingCellData", 1000);
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

void MainWindow::openConnectionDialog() {}
void MainWindow::storeCell(int x, int y) {}
void MainWindow::storeCell() {}
void MainWindow::openManageDialog() {}
