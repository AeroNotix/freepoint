#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <Qt>
#include <QPointer>
#include <QMainWindow>
#include <QKeyEvent>
#include <QToolBar>

#include "database.h"

// forward decl
class QItemDelegate;

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
    void SetUsername(const QString &test);
    const QString GetUsername(void) const;
    void SetPassword(const QString &test);
    const QString GetPassword(void) const;
    const QString GetTable(void) const;
    const QString GetDatabase(void) const;
    void InsertRow(QStringList newrowdata);
    void AddNewConnection(QString database, QString table);
	void CreateNew(QString jsondata);

public slots:
    void InsertedRow(QNetworkReply *reply);
    void DeletedData(QNetworkReply *reply);

private slots:
    void InsertData(QNetworkReply *reply);
    void UpdatedData(QNetworkReply *reply);
    void openConnectionDialog();
    void openManageDialog();
    void storeCell(int x, int y);
    void RefreshTable();
    void InsertRow();
    void changeTable(int, int);
    void ExportAsCSV();
    void CreateNewTable();
    void PreviousTable();
    void NextTable();
    void Exit();
    void NewRow(int x, int y, QTableWidgetItem *newrow);
    void InsertRow(int x);
    void DeleteItem(int x, int y);
    void DeleteRow(int x);
    void WriteCSV(QString);

private:
    void PopulateTable();
    bool ParseTableConfig();
    void SetCurrentTable();
    void ClearTable();
    void insertRowData(QList<QStringList>);
    void SetDelegates(QMetadata);
    void ClearDelegates();
    void keyPressEvent(QKeyEvent *event);
    void StoreCell(int x, int y);
    void RevertCellData();
    void ChangeConnection(int connection_number);
    void AddMenuActions();
    void Login();
    void OpenConnectionDialog();
    void CreateAction();
    void PopulateToolbar();
    void DeleteRows();
    void GenericHandleResponse(QNetworkReply *reply);

    QWidget *parent;
    std::unique_ptr<Ui::MainWindow> ui;
    QPointer<Database> db;
    QList<QVariantMap> connections;
    QPointer<QToolBar> toolbar;
    int current_connection_index;
    bool populating;
    QString username;
    QString password;
    QStringList headings;
    QList<QStringList> queryset;
    bool networkRequestPending;
    QString storeditem;
    std::pair<int, int> storedcoords;
    QList<QItemDelegate*> delegates;
    QStringList connection_names;
    QVariantMap connection_map;

signals:
    void NewRowSIG(int x, int y, QTableWidgetItem* newrow);
    void InsertRowSIG(int x);
    void DeleteItemSIG(int x, int y);
    void DeleteRowSIG(int x);

};

#endif // MAINWINDOW_H
