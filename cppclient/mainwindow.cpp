#include <vector>
#include <iostream>

#include "qjson/parser.h"

#include "login.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      // this needs to be changed to pull it from the form, remember.
      db(new Database(this, "aero", "passwd", "db_freepoint", "buchungsdatei"))
{
    ui->setupUi(this);
    setStatusBar(ui->statusbar);
    Login();
    PopulateTable();
}

MainWindow::~MainWindow() {
    ClearTable();
    delete ui;
    delete db;
}

void MainWindow::PopulateTable(void) {
    ClearTable();
    db->Query();
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
    for (auto row : rawrows_initial) {
        rows.append(row.toStringList());
    }

    insertRowData(rows);
    blockSignals(true);
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


/*
  Delete items in the table by iterating through their
*/
void MainWindow::ClearTable() {
    int rows = ui->tableWidget->rowCount();
    int cols = ui->tableWidget->columnCount();

    for (int x = 0; x < rows; ++x)
        for (int z = 0; z < cols; ++z) {
            delete ui->tableWidget->itemAt(x, z);
            ui->tableWidget->removeRow(rows - x);
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
