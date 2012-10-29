#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "database.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void ShowMessage(const QString &text, int t);
    void ShowError(const QString &text);

private:
    void PopulateTable       ();
    void ChangeTable         (int x, int y);
    void InsertData          (std::string json);
    void StoreCell           (int x, int y);
    void RevertCellData      (int x, int y);
    void ClearTable          ();
    void ChangeConnection    (int connection_number);
    void AddMenuActions      ();
    void Login               ();
    void OpenConnectionDialog();
    void CreateAction        ();

    Ui::MainWindow *ui;
    QAction *newAction;
    Database *db;
    bool populating;
};

#endif // MAINWINDOW_H
