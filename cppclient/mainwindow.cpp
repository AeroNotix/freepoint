#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  Database db(this, "aero", "passwd", "db", "table", "localhost:6060");
  ui->setupUi(this);
  setStatusBar(ui->statusbar);
  Login();
  ShowMessage(db.BaseURL, 1000);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::PopulateTable()
{
  ShowMessage("Populating table",1000);
}

void MainWindow::ShowMessage(const QString &text, int t)
{
  ui->statusbar->showMessage(text, t);
}

void MainWindow::ShowError(const QString &text)
{
  ShowMessage(text, 1000);
}

void MainWindow::Login()
{
  ShowMessage("Logging in....", 1000);
}

void MainWindow::RevertCellData(int x, int y)
{
  ShowMessage("RevertingCellData", 1000);
}
