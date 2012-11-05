#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Qt>
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

private slots:
    void InsertData(QNetworkReply *reply);
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

private:
    void PopulateTable();
    void ParseTableConfig();
    void SetCurrentTable();
    void ClearTable();
    void insertRowData(QList<QStringList>);
	void SetDelegates(QMetadata);
	void ClearDelegates();
    void keyPressEvent(QKeyEvent * event);
    void StoreCell(int x, int y);
    void RevertCellData(int x, int y);
    void ChangeConnection(int connection_number);
    void AddMenuActions();
    void Login();
    void OpenConnectionDialog();
    void CreateAction();
    void PopulateToolbar();

    Ui::MainWindow *ui;
    QAction *newAction;
    Database *db;
    bool populating;
    QString username;
    QString password;
    QStringList headings;
    QList<QStringList> queryset;
    QList<QVariantMap> connections;
    bool networkRequestPending;
	QToolBar *toolbar;
    QString storeditem;
	QList<QItemDelegate*> delegates;
    QStringList connection_names;
    QVariantMap connection_map;
    int current_connection_index;

};

#endif // MAINWINDOW_H
