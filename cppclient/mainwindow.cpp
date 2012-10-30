#include <vector>
#include <iostream>

#include "login.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      // this needs to be changed to pull it from the form, remember.
      db(new Database(this, "aero", "passwd", "db", "table"))
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
    blockSignals(true);
    queryset = db->Query();
    headings = db->GetHeadings();

    /*

      This code is not used yet.

      We need to implement the dynamic selection of delegator according
      to the metadata. This involves replicating the parsing of the JSON
      data from the db as in the Python code.

      We also need to replicate the delegator hierarchy as in the Python
      code.

      For now, the standard delegator will work ok.

    delegator = new Delegator(
        headings, db->GetMetadata(), this
        );

    // set our delegator to the dynamically chosen one.
    ui->tableWidget->setItemDelegate(delegator);
    */

    // set the column size according to how many headings we have.
    ui->tableWidget->setColumnCount(headings.size());
    // set the heading strings to the headings labels we received.
    ui->tableWidget->setHorizontalHeaderLabels(headings);

    ClearTable();
    for (int x = 0; x < queryset.size(); ++x) {
        ui->tableWidget->insertRow(x);
        for (int z = 0; z < queryset[x].size(); ++z) {
            // items deleted in dtor using the ClearTable method
            ui->tableWidget->setItem(x, z, new QTableWidgetItem(queryset[x][z]));
        }
    }
}

void MainWindow::ClearTable() {
    int rows = ui->tableWidget->rowCount();
    int cols = ui->tableWidget->columnCount();

    for (int x = 0; x < rows; ++x)
        for (int z = 0; z < cols; ++z)
            delete ui->tableWidget->itemAt(x, z);
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
