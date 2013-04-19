/// This is the MainWindow class it can be thought of as the 'core' of the application.
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <Qt>
#include <QPointer>
#include <QMainWindow>
#include <QKeyEvent>
#include <QToolBar>
#include <QNetworkCookie>

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
   /**
    * @brief Constructor.
    * @param parent this is a pointer to the parent object. All QObjects
    * need a pointer to their parent so they can set who they are managed
    * by.
    */
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
   /**
    * @brief Shows a message on the status bar.
    * @param text this is the message which you want to show on the
    * status bar.
    * @param t this is the time you want the message to show for.
    */
    void ShowMessage(const QString &text, int t);
   /**
    * @brief Shows a message on the status bar and as an error dialog.
    * @param text this is the message which you want to show on the
    * status bar and in the error dialog.
    */
    void ShowError(const QString &text);

    //@{
    /** Setter
     * @param uname the QString to set the username to.
     */
    void SetUsername(const QString &uname);
    const QString GetUsername(void) const;
    /** Setter 
     * @param pass the QString to set the password to.
     */
    void SetPassword(const QString &pass);
	void SetCookies(QList<QNetworkCookie>);
    const QString GetPassword(void) const;
    const QString GetTable(void) const;
    const QString GetDatabase(void) const;          // Getters.
	const QList<QNetworkCookie> GetCookies(void) const;
    const QStringList GetHeadings(void) const {
        return connection_names;
    }
    const QVariantMap GetMap(void) const {
        return connection_map;
    }
    const std::pair<QString, QString> GetDBTableInfo(QString connectionname) const;
    //@}

    /**
     * @brief Pass-through method to the underlying database to insert a whole new
     * row into the currently active table.
     * @param newrowdata a list of strings which will be interpreted as the
     * whole new row to be inserted into the db. The DB's state will hold
     * the right db connection and thus you don't have to worry about making
     * sure that it will go to the right table.
     */
    void InsertRow(QStringList newrowdata);
    /**
     * @brief AddNewConnection simply opens the connection configuration file and
     * adds in a new connection with the database:table entry. This method
     * will check for failures and signify an error in that case.
     * @param database the QString which signifies the new database name
     * @param table the QString which signifies the new table name
     */
    void AddNewConnection(QString database, QString table);
    /*
     * @brief CreateNew is called when a new table is to be created. The database
     * creator window will call this method and we just pass it straight through
     * to the database. We do this so we can compartmentalize our code.
     * @param jsondata a raw json formatted string signifying the new table
     * to be created. This string _must_ be in conformance with the API.
     * @see settings_server/README.md for details.
     */
	void CreateNew(QString jsondata);

public slots:
    /*
     * @brief When the database returns from inserting rows, this method will be
     * invoked to handle the response from the server.
     * @param reply This contains all the HTTP headers from the actual HTTP
     * response. Inside this we can determine if the insert was a success (or not).
     */
    void InsertedRow(QNetworkReply *reply);
    /*
     * @brief When the database returns from deleting rows, this method will be
     * invoked to handle the response from the server.
     * @param reply This contains all the HTTP headers from the actual HTTP
     * response. Inside this we can determine if the delete was a success (or not).
     */
    void DeletedData(QNetworkReply *reply);

private slots:
    void InsertData(QNetworkReply *reply);
    void UpdatedData(QNetworkReply *reply);
    void openConnectionDialog();
    void openManageDialog();
    void openCreateUserDialog();
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
    void ChangeConnection(QAction *action);

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
    void AddMenuActions();
    void Login();
    void OpenConnectionDialog();
    void CreateAction();
    void PopulateToolbar();
    void DeleteRows();
    void GenericHandleResponse(QNetworkReply *reply);

    QWidget *parent;
    // The UI does not inherit from QObject and thus cannot be inside
    // a QPointer.
    std::unique_ptr<Ui::MainWindow> ui;
    QPointer<Database> db;
    QList<QVariantMap> connections;
    QPointer<QToolBar> toolbar;
    QPointer<QActionGroup> actionGroupConnections;
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
	QList<QNetworkCookie> cookies;

signals:
    /**
     * @brief This signal gets fired when there is a new row item available for
     * insertion into the database.
     */
    void NewRowSIG(int x, int y, QTableWidgetItem* newrow);
    /**
     * @brief This signal gets fired when there is a new row available for
     * insertion into the TableWidget.
     */
    void InsertRowSIG(int x);
    /**
     * @brief This signal gets fired when an item needs to be deleted from the
     * TableWidget.
     */
    void DeleteItemSIG(int x, int y);
    /**
     * @brief This signal gets fired when a row needs to be deleted from the
     * TableWidget.
     */
    void DeleteRowSIG(int x);

};

#endif // MAINWINDOW_H
