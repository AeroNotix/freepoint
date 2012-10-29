#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"
#include <iostream>
#include "login.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      // this needs to be changed to pull it from the form, remember.
      db(new Database(this, "aero", "passwd", "db", "table", "localhost:6060"))
{
    ui->setupUi(this);
    setStatusBar(ui->statusbar);
    Login();
    ShowMessage(db->BaseURL, 1000);
}

MainWindow::~MainWindow() {
    delete ui;
    delete db;
}

void MainWindow::PopulateTable() {
    ShowMessage("Populating table",1000);
}

void MainWindow::ShowMessage(const QString &text, int t) {
    ui->statusbar->showMessage(text, t);
}
 
void MainWindow::ShowError(const QString &text) {
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
