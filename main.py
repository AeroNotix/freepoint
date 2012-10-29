'''
Main entry point for the program.

Instantiates all the required GUI classes and enters the
event loop.
'''
import time

import os
import sys
import argparse
import ConfigParser
import csv

from PyQt4 import QtGui, QtCore

from qtsqlviewer.ui.mainwindow_UI import Ui_MainWindow
from qtsqlviewer.ui.dlg_sql_connection import SQLDisplaySetup
from qtsqlviewer.ui.connection_dialog import ConnectionDialog
from qtsqlviewer.ui.login import Login

from qtsqlviewer.table_tools.tools import (
    table_wrapper, Database, create_action
)
from qtsqlviewer.table_tools.argument import Argument
from qtsqlviewer.table_tools.delegates import Delegator

# this is strange because it gets used implicitly when using the
# Qt syntax for QResource objects. It may seem that this doesn't
# get used but there are side-effects when importing this module
# which hook into Qt's internal resource management methods.
from qtsqlviewer.ui import resource_rc
from qtsqlviewer.ui.createDatabase import CreateNewTable

from qtsqlviewer.settings import module_path

## Argument creation
PARSER = argparse.ArgumentParser()
ARGS = (
    Argument('db',       help='Database you wish to connect to'         ),
    Argument('password', help='Password for the database',   default='' ),
    Argument('user',     help='User you want to connect as', default='' ),
    Argument('table',    help='Which table to connect to'               ),
)

for arg in ARGS:
    PARSER.add_argument(arg.name, **arg.data)
RESULTS = PARSER.parse_args()
CWD = module_path()

class MainGui(QtGui.QMainWindow):
    """
    Main GUI class which creates the window, responds to user input and holds
    the slots assigned to the class' signals.
    """

    def __init__(self, parent=None):
        """
        Creation and main check

        :param parent: A Qt object which is the 'owner' of this object.
                       This is used if the parent is closed/killed therefore
                       this object must also be killed.
        """

        QtGui.QMainWindow.__init__(self, parent)
        self.gui = Ui_MainWindow()
        self.gui.setupUi(self)
        self.login()
        self.toolbar = self.addToolBar("Toolbar")
        self.populated = False
        self.cell = ''
        self.config = ConfigParser.RawConfigParser()
        self.configpath = os.path.join(CWD, "conf.cfg")
        self.actionList = []
        self.lock = QtCore.QMutex()
        self.current_table = None
        self.populating = False

        if RESULTS.db:
            # if we got command line arguments, open that
            self.database = Database(
                self,
                self.username,
                self.password,
                RESULTS.db,
                RESULTS.table,
            )
            self.populate_table()
            self.parse_config()
        elif os.path.isfile(self.configpath):
            # if we didn't get command line arguments check if
            # there is a config file.
            self.config.read(self.configpath)
            try:
                self.current_table = self.config.sections()[0]
                self.database = Database(
                    self,
                    self.username,
                    self.password,
                    self.config.get(self.current_table, "database"),
                    self.config.get(self.current_table, "table"),
                    )
                self.populate_table()
            except ConfigParser.NoSectionError as error:
                # if the file exists but it doesn't have the required section
                # then something is wrong. So we error.
                QtGui.QMessageBox.warning(
                    self,
                    "Error",
                    "Config file error: %s" % str(error),
                    QtGui.QMessageBox.Ok
                )
            except IndexError:
                QtGui.QMessageBox.warning(
                    self,
                    "Error",
                    "Missing configuration: Please create.",
                    QtGui.QMessageBox.Ok
                )
                self.openConnectionDialog()
        else:
            # else allow the user to enter the details via
            # the gui
            self.openConnectionDialog()

        self.addMenuActions()
        self.parse_config()
        self.populate_toolbar()

    def populate_table(self):
        """
        Opens and displays the data taken from the self.database.query()

        We have wrapped this function in the decorator which will stop certain
        signals running when we are inserting data into the table.

        First we clear the table of all contents and make a simple login call to
        the server. We do this so that we both know that we can connect and we
        then will know that the login details are valid. Eventually we will have
        a login wrapper which will wrap all functions which need to be validated.

        We then ask the server for a query set, which is by default a SELECT * on
        each table but this can be customized using the database settings metadata
        on the metadata database.

        Once we have the data itself, we get the headings and the metadata from
        the database. We then create an instance of the Delegator QItemDelegate
        subclass using the heading names and the metadata. Refer to
        :class:`Delegator` documentation to see how this works. Essentially it
        parses the JSON metadata and creates instances of further QItemDelegate
        subclasses in order to provide the proper editors and data setting
        behaviour.

        Once this has been completed we insert the data into the table and return
        control to the decorator which reapplies the signals and returns itself.

        We call the close() method on the database because the GUI class doesn't
        care about the implementation of the database and the idea is that the
        database instance can be swapped out providing that the contract between
        the GUI and the database, i.e. argument and return types  is not violated.
        """
        if self.populating:
            return
        self.populating = True
        try:
            if self.rowInserter.isRunning():
                return
        except AttributeError:
            pass

        self.blockSignals(True)
        # We can't continue if we cannot make a connection to the database.
        if not self.database.connect():
            return

        # get the headings so we can set up the table
        self.queryset = self.database.query()
        self.headings = self.database.get_headings()

        self.delegator = Delegator(
            self.headings, self.database.metadata, parent=self
        )

        self.gui.tableWidget.setItemDelegate(self.delegator)
        # set the column size according to the headings
        self.gui.tableWidget.setColumnCount(len(self.headings))
        # set the labels to the column names
        self.gui.tableWidget.setHorizontalHeaderLabels(self.headings)

        self.clear_table()

        # Create an instance of the threaded RowInserter and disconnect
        # troublesome signals. If these signals are connected, the very
        # action of inserting a row programmatically will execute some
        # other code. We don't want that.
        self.rowInserter = RowInserter(len(self.queryset), self)
        self.gui.tableWidget.blockSignals(True)
        self.gui.tableWidget.setSortingEnabled(False)
        self.connect(
            self.rowInserter, QtCore.SIGNAL("finished()"), self.__threaded_populate
            )
        self.connect(
            self.rowInserter, QtCore.SIGNAL("addrow(int)"), self.insertNewRow
            )
        # Start the RowInserter's async'd method. This will in-turn
        # start another async thread to insert the real data.
        self.rowInserter.start()
        self.database.close()
        self.populated = True

    def insertNewRow(self, i):
        self.gui.tableWidget.insertRow(i)

    def insertRowData(self, obj):
        self.gui.tableWidget.setItem(obj[0], obj[1], obj[2])

    def changeTable(self, xrow, ycol):
        """
        When data in the form is changed we contact the server to change the data.

        :param xrow: :class:`Int`
        :param ycol: :class:`Int`
        :returns: None
        """
        self.lock.lock()
        validation_check = self.delegator.apply_validators(xrow, ycol)
        if validation_check[0]:
            self.database.changeTable(xrow, ycol)
        else:
            self.revertCellData(xrow, ycol)
            self.show_error(
                "Data failed to validate. Failed on check: validation_%s." % validation_check[1]
            )
        self.lock.unlock()

    def insertData(self, json):
        """
        Pass through method which calls the underlying database call.

        The reason for this is that, centrally, all class references have explicit
        knowledge of the parent (QMainWindow) but they shouldn't/don't have any
        idea of the classes that the MainWindow also references. It allows us
        to make the MainWindow control/marshal access and change the API by pushing
        the calls through a central class rather than everything calling methods
        directly.
        """
        return self.database.insertData(json)

    def storeCell(self, xrow, ycol):
        """
        When a cell is entered we store it in case the validation for the
        data that the user enters fails.

        :param xrow: :class:`Int` which is passed directly from the signal
        :param ycol: :class:`Int` which is passed directly from the signal
        :returns: None
        """
        self.cell = self.gui.tableWidget.item(xrow, ycol).text()

    def revertCellData(self, xrow, ycol):
        """
        Reverts the cell at (xrow, ycol) to the value which is held in self.cell

        :param xrow: :class:`Int`
        :param ycol: :class:`Int`
        :returns: None
        """
        self.gui.tableWidget.blockSignals(True)
        self.gui.tableWidget.setItem(
            xrow, ycol, QtGui.QTableWidgetItem(self.cell)
        )
        self.gui.tableWidget.blockSignals(False)

    def clear_table(self):
        """
        Method to clear the table.

        We iterate forwards but we delete the opposite side of the table
        because we can be safe knowing that we aren't iterating over the
        iterable of rows whilst changing the size of the iterable.

        :returns: :class:`None`
        """

        self.gui.tableWidget.setRowCount(0)
        return
        row_count = self.gui.tableWidget.rowCount()
        if not row_count:
            return
        for row in range(row_count+1):
            self.gui.tableWidget.removeRow(row_count - row)
        self.populated = False

    def show_message(self, message, time=1000):
        """
        Method which sets the status message for a period of time
        """
        self.gui.statusbar.showMessage(message, time)

    def show_error(self, error_message_string):
        """
        Error messages are a special case of messages. We also need to show
        the user an alert dialog along with the message in the status bar.
        """
        QtGui.QMessageBox.warning(self, "Error", error_message_string)
        self.show_message(error_message_string, time=10000)

    def changeConnection(self, section):
        """
        Changes the connection to the one specified under the `section`
        heading.

        if the clicked item is the current table then we check if it's
        the only table, if so we allow the 'update' to happen. If not,
        we return.

        We do this because otherwise clicking a single item would yield
        nothing and maybe the user wishes to refresh the table (despite,
        having a refresh button) and if the list is len()>1 we don't
        allow it because it may be an accidental click.
        """

        if self.current_table == section:
            if len(self.actionList) == 1:
                pass
            else:
                return

        self.database.close()
        self.clear_table()

        self.database.database = self.config.get(section, "database")
        self.database.table = self.config.get(section, "table")

        self.populate_table()
        self.current_table = section

    def addMenuActions(self):
        """
        The submenu which contains a list of all connection details
        needs to dynamically populated at launch. This is the method
        which does that.

        First we remove all the current menu items, then we reattach
        all the menu items. We do this because in-between calling this
        the amount of actions can change for this menu depening on
        if a new connection was created or not.

        :returns: :class:`None`
        """

        self.actionGroupConnections = QtGui.QActionGroup(self)

        # If it's the first load our actionList will be empty so we
        # don't need to remove them. If not, we iterate through the list
        # and remove the actions.
        if not self.actionList:
            pass
        else:
            for action in self.actionList:
                self.gui.menuSelect_Table.removeAction(action)
            self.actionList = []

        for _, section in enumerate(self.config.sections()):
            database = self.config.get(section, "database")
            table = self.config.get(section, "table")

            action = QtGui.QAction(self)
            action.setText(database+"."+table)
            action.setObjectName(section)
            action.setCheckable(True)
            if section == "connection-0":
                action.setChecked(True)

            # Slot as a lambda because we need to pass an argument
            # to it.
            self.connect(action, QtCore.SIGNAL("triggered()"),
                         lambda arg=section: self.changeConnection(arg))
            # Add the action to the GUI.
            self.gui.menuSelect_Table.addAction(action)
            # Keep a reference to it.
            self.actionList.append(action)
            # Group it with the others to enable mutual exclusivity.
            self.actionGroupConnections.addAction(action)

    def parse_config(self):
        """
        Parses the config file. We can use this method after we edit the config file
        so that the config file is reloaded.
        """
        self.config.read(self.configpath)
        self.addMenuActions()

    def rewriteConfig(self):
        """
        Writes a the updated config back to disk
        """
        self.config.write(open(self.configpath, 'wb'))
        self.parse_config()

    def openManageDialog(self):
        """
        Creates an instance of the connection manager and shows it.
        """
        manage_dlg = ConnectionDialog(self)
        manage_dlg.exec_()
        return

    def openConnectionDialog(self):
        """
        Creates an instance of the new connection dialog and shows it.
        """
        setup_dlg = SQLDisplaySetup(self)
        setup_dlg.exec_()
        return

    def createTableDialog(self):
        """
        Creates an instance of the create table dialog and shows it
        """
        create_dlg = CreateNewTable(self)
        create_dlg.exec_()
        return

    def login(self):
        """
        Invokes the Login form.
        """
        login = Login(self)
        login.exec_()

    def export_as_csv(self):
        """
        Method which we attach to the toolbar button to create and setup the
        Save As.. dialog
        """
        savedialog = QtGui.QFileDialog()
        savedialog.setAcceptMode(1)               # Sets it to a save dialog
        savedialog.setDefaultSuffix("csv")
        self.connect(
            savedialog, QtCore.SIGNAL("fileSelected(QString)"), self.write_csv
        )
        savedialog.exec_()

    def write_csv(self, fname):
        """
        The method which actually writes the CSV to disk.

        This should be in a separate thread because if the dataset is large
        then we will end up overloading the main thread.
        """

        try:
            fout = open(fname, "wb")
        except IOError:
            self.show_error("Cannot open output file. Is it open elsewhere?")
            return
        csvout = csv.writer(fout)
        csvout.writerow(self.database.get_headings())
        rows = self.gui.tableWidget.rowCount()
        cols = self.gui.tableWidget.columnCount()
        for row in range(rows):
            thisrow = [self.safeGetCell(row,col) for col in range(cols)]
            csvout.writerow(thisrow)
        fout.close()

    def safeGetCell(self, row, col):
        """
        safeGetCell wraps a call to the tableWidget.item(x,y) method so
        that we can wrap it in a try:except block due to the above method
        returning None when called with co-ords which is a blank item
        in the table. The None item indicates an empty cell, so we
        return a blank string instead.
        """

        try:
            return self.gui.tableWidget.item(row, col).text()
        except AttributeError:
            return ""

    def insert_row(self):
        """
        Inserts a new row into the table ready for editing.
        """
        self.delegator.createUIForm(self)

    def delete_rows(self):
        row_ids = []
        for row in self.gui.tableWidget.selectionModel().selectedRows():
            row_ids.append(self.gui.tableWidget.item(row.row(), 0).text())
        if len(row_ids) == 0:
            return

        self.database.delete_rows(map(str, row_ids))

    def refresh(self):
        self.populate_table()

    def populate_toolbar(self):
        """
        Method which allows us to programmatically add the toolbar buttons
        instead of littering the initializer with calls to the create_action
        function.
        """
        create_action(
            self, "Refresh", fname=":/view-refresh", slot=self.refresh
            )
        create_action(
            self, "Insert Row", fname=":/list-add", slot=self.insert_row
            )
        create_action(
            self, "Export as CSV", fname=":/document-save-as",
            slot=self.export_as_csv
            )
        create_action(
            self, "Create new table", fname=":/bookmark-new",
            slot=self.createTableDialog
            )
        create_action(self, "Prev Table", fname=":/go-previous", slot=self.previous)
        create_action(self, "Next Table", fname=":/go-next", slot=self.next)
        create_action(self, "Quit", fname=":/system-log-out", slot=sys.exit)

    def keyPressEvent(self, event):
        """
        We overload this method so that we may assign methods to keypresses
        easily.

        :param event: The event that bubbles through to this method.
        :note: Do not call this method directly. Instead, allow the UI to
               pass along calls to it.
        """

        actions = {
            QtCore.Qt.Key_F5:     self.populate_table,
            QtCore.Qt.Key_Insert: self.insert_row,
            QtCore.Qt.Key_Delete: self.delete_rows
        }

        action = actions.get(event.key())
        if action:
            action()

    def __threaded_populate(self):
        """
        __threaded_populate is supposed to be a private method which will set
        off inserting rows and adding data to the table in the proper order.


        We use signals/slots to communicate with the main thread. How
        we do this is similar to CSP-style concurrency if you are fami-
        liar with that. The general gist is that you have some kind of
        event loop/mailbox/channel on which to 'pass' messages down.

        We attach a signal type to a function by using the
        QtCore.SIGNAL function and then we execute a thread which sends
        back data to the GUI event loop. This frees up the event loop
        but also executes a second thread for insertion and processing.
        """
        try:
            if not self.tableThreader.isFinished():
                return
        except AttributeError:
            pass

        self.tableThreader = TableUpdater(self.queryset[:], self)
        self.connect(
            self.tableThreader, QtCore.SIGNAL("finished()"), self.__reattach
            )
        self.connect(
            self.tableThreader, QtCore.SIGNAL("insertData(PyQt_PyObject)"),
            self.insertRowData
            )
        self.tableThreader.start()

    def __reattach(self):
        """
        __reattach is the callback attached to the RowInserter thread call
        we use this to re-enable signals and allow for sorting on the table
        data.
        """
        self.gui.tableWidget.blockSignals(False)
        self.gui.tableWidget.setSortingEnabled(True)

    def next(self):
        """
        Advances to the next connection
        """

        if not self.current_table:
            return
        n = self.current_table.split('-')
        next_connection = n[0]+'-'+str(int(n[1])+1)
        if self.config.has_section(next_connection):
            self.changeConnection(next_connection)

    def previous(self):
        """
        Selects the previous connection
        """

        if not self.current_table:
            return

        p = self.current_table.split('-')
        prev_connection = p[0]+'-'+str(int(p[1])-1)
        if self.config.has_section(prev_connection):
            self.changeConnection(prev_connection)

class RowInserter(QtCore.QThread):
    """
    RowInserter implements the threading associated with *creating* rows
    and adding blank rows to the table. We do this since there is a mis-
    match between adding rows in sub-threads and the way that they are
    added to the table in the main thread.
    """

    def __init__(self, number, obj):
        """
        Creates an instance of RowInserter

        number is the amount of rows to add to the QTableWidget and obj
        is the instance of a QMainWindow of which the QTableWidget is
        attached to.
        """
        super(RowInserter, self).__init__()
        self.n = number
        self.obj = obj

    def run(self):
        """
        Never call this directly. Use the .start() method as that will
        set up some Qt specific things and then call this method.

        Task to run in a separate thread.
        """

        for row in range(self.n):
            self.emit(QtCore.SIGNAL("addrow(int)"), row)
        self.exit()

class TableUpdater(QtCore.QThread):
    """
    TableUpdater implements the threading of inserting QTableWidgetItems
    into the table. We do this because if the data were to grow to extr-
    emely large levels then having it single-threaded would mean that
    the GUI event loop would block whilst the data was being inserted
    into the table.
    """
    def __init__(self, queryset, obj):
        """
        Creates an instance of TableUpdater.

        queryset is the actual data to be inserted into the table and obj
        is the instance of QMainWindow which the QTableWidget is a child
        of.
        """
        self.queryset = queryset
        self.obj = obj
        super(TableUpdater, self).__init__()

    def run(self):
        """
        Never call this directly. Use the .start() method as that will
        set up some Qt specific things and then call this method.

        Task to run in a separate thread.
        """

        # take references via the dot operator for efficiency because the
        # below can potentially be a hot loop for large datasets
        setItem = self.obj.gui.tableWidget.setItem
        QTableWidgetItem = QtGui.QTableWidgetItem
        # iterate through the query set and get the data into the table
        for idx, data in enumerate(self.queryset):
            if not data:
                break
            for num, info in enumerate(data):
                self.emit(QtCore.SIGNAL("insertData(PyQt_PyObject)"),
                          [idx, num, QTableWidgetItem(unicode(info))]
                )
        self.obj.blockSignals(False)
        self.exit()
        self.obj.populating = False
        self.obj.blockSignals(False)

if __name__ == '__main__':
    APPLICATION = QtGui.QApplication(sys.argv)
    MAINWINDOW = MainGui()
    MAINWINDOW.setStatusBar(MAINWINDOW.gui.statusbar)
    MAINWINDOW.showMaximized()
    sys.exit(APPLICATION.exec_())


    
