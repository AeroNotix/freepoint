"""
intention is to make a generic GUI for interacting with a mysql database
"""

import os
import sys
import argparse
import ConfigParser
import csv

from PyQt4 import QtGui, QtCore

import _mysql_exceptions

from qtsqlviewer.ui.mainwindow_UI import Ui_MainWindow
from qtsqlviewer.ui.dlg_sql_connection import SQLDisplaySetup
from qtsqlviewer.ui.connection_dialog import ConnectionDialog
from qtsqlviewer.ui.login import Login

from qtsqlviewer.table_tools.tools import table_wrapper, Database, create_action
from qtsqlviewer.table_tools.argument import Argument
from qtsqlviewer.table_tools.mysql_error_codes import mysqlerror

from qtsqlviewer.ui import resource_rc

CWD = os.path.dirname(__file__)

## Argument creation
PARSER = argparse.ArgumentParser()
ARGS = (
    Argument('db', help='Database you wish to connect to'),
    Argument('password', help='Password for the database', default=''),
    Argument('user', help='User you want to connect as', default=''),
    Argument('table', help='Which table to connect to'),
    Argument('host', help='Which host you want to connect to',
             default="localhost"),
    Argument('port', help='The port you want to connect through',
             default=3306)
)

for arg in ARGS:
    PARSER.add_argument(arg.name, **arg.data)
RESULTS = PARSER.parse_args()


class TableDelegate(QtGui.QItemDelegate):
    def __init__(self, headers, metadata, parent=None):
        QtGui.QItemDelegate.__init__(self, parent)
        self.headers = headers
        self.metadata = metadata
        self.field_types = {
            "COMBO": QtGui.QComboBox
            }
    def createEditor(self, parent, option, index):
#        print index
#        cmb = QtGui.QComboBox(parent)
#        cmb.addItems(["Yes", "No"])
#        return cmb
        pass
        

    def setModelData(self, editor, model, index):
        model.setData(index, editor.currentText())


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
        
        if RESULTS.db:
            # if we got command line arguments, open that
            self.database = Database(
                self,
                RESULTS.host,
                self.username,
                self.password,
                RESULTS.db,
                RESULTS.table,
                RESULTS.port
            )
            self.populate_table()
            self.parse_config()
        elif os.path.isfile(self.configpath):
            # if we didn't get command line arguments check if
            # there is a config file.
            self.config.read(self.configpath)
            try:
                self.current_table = self.config.sections()[0]
                host = self.config.get(self.current_table, "host")
                port = self.config.get(self.current_table, "port")
                if not host:
                    host = "localhost"
                if not port:
                    port = 3306
                self.database = Database(
                    self,
                    host,
                    self.username,
                    self.password,
                    self.config.get(self.current_table, "database"),
                    self.config.get(self.current_table, "table"),
                    port
                    )
                self.populate_table()
            except ConfigParser.NoSectionError as error:
                # if the file exists but it doesn't have the required section
                # then something is wrong. So we error.
                QtGui.QMessageBox.warning(self,
                                          "Error",
                                          "Config file error: %s" % str(error),
                                          QtGui.QMessageBox.Ok)
            except IndexError:
                QtGui.QMessageBox.warning(self,
                                          "Error",
                                          "Missing configuration: Please create.",
                                          QtGui.QMessageBox.Ok)
                self.openConnectionDialog()
        else:
            # else allow the user to enter the details via
            # the gui
            self.openConnectionDialog()
        self.addMenuActions()
        self.parse_config()
        self.populate_toolbar()

    @table_wrapper
    def populate_table(self):
        """
        Opens and displays a MySQL table

        Mutates the table's headings and the data in the table with
        the data returned from the select on the passed in database.
        """

        self.clear_table()

        try:
            self.database.connect()
        except _mysql_exceptions.OperationalError, error:
            # on any error report to the user and return
            QtGui.QMessageBox.warning(self, "Error", str(error))
            return


        # get the headings so we can set up the table
        try:
            queryset = self.database.query()
            self.headings = self.database.get_headings()
        except _mysql_exceptions.OperationalError, error:
            self.show_error(str(error))
            return
        except _mysql_exceptions.ProgrammingError as error:
            self.show_message(mysqlerror(error), time=10000)
            return

        self.gui.tableWidget.setItemDelegate(
            TableDelegate(self.headings, self.database.metadata)
            )
        # set the column size according to the headings
        self.gui.tableWidget.setColumnCount(len(self.headings))
        # set the labels to the column names
        self.gui.tableWidget.setHorizontalHeaderLabels(self.headings)

        # iterate through the query set and get the data into the table
        for idx, data in enumerate(queryset):
            if not data:
                break

            self.gui.tableWidget.insertRow(idx)
            for num, info in enumerate(data):
                self.gui.tableWidget.setItem(
                    idx, num, QtGui.QTableWidgetItem(unicode(info))
                )
        # close the connection
        self.database.close()
        self.populated = True

    def changeTable(self, xrow, ycol):
        """
        When data in the form is changed we contact the server to change the data.

        :param xrow: :class:`Int`
        :param ycol: :class:`Int`
        :returns: None
        """
        self.database.changeTable(xrow, ycol)

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
        self.gui.tableWidget.setItem(xrow, ycol, QtGui.QTableWidgetItem(self.cell))

    def clear_table(self):
        """
        Method to clear the table.

        We iterate forwards but we delete the opposite side of the table
        because we can be safe knowing that we aren't iterating over the
        iterable of rows whilst changing the size of the iterable.

        :returns: :class:`None`
        """
        if self.gui.tableWidget.rowCount():
            row_count = self.gui.tableWidget.rowCount()
            for row in range(row_count + 1):
                self.gui.tableWidget.removeRow(row_count - row)
        self.populated = False

    def show_message(self, message, time=1000):
        """
        Method which sets the status message for a period of time
        """
        self.gui.statusbar.showMessage(message, time)

    def show_error(self, error_message_string):
        QtGui.QMessageBox.warning(self, "Error", error_message_string)
        self.show_message(error_message_string, time=10000)

    def changeConnection(self, section):
        """
        Changes the connection to the one specified under the `section`
        heading.
        """

        if self.current_table == section:
            if len(self.actionList) == 1:
                pass
            else:
                return

        self.database.close()
        self.clear_table()

        self.database.host = self.config.get(section, "host")
        self.database.database = self.config.get(section, "database")
        self.database.table = self.config.get(section, "table")
        self.database.port = self.config.get(section, "port")

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

        if not self.actionList:
            pass
        else:
            for action in self.actionList:
                self.gui.menuSelect_Table.removeAction(action)
            self.actionList = []

        for idx, section in enumerate(self.config.sections()):
            host = self.config.get(section, "host")
            database = self.config.get(section, "database")
            table = self.config.get(section, "table")

            action = QtGui.QAction(self)
            action.setText(host+"."+database+"."+table)
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

    def login(self):
        """
        Invokes the Login form.
        """
        login = Login(self)
        login.exec_()

    def export_as_csv(self):
        """
        Method which we attach to the toolbar button to create and setup the Save As.. dialog
        """
        savedialog = QtGui.QFileDialog()
        savedialog.setAcceptMode(1)      # Sets it to a save dialog
        savedialog.setDefaultSuffix("csv")
        self.connect(savedialog, QtCore.SIGNAL("fileSelected(QString)"), self.write_csv)
        savedialog.exec_()

    def write_csv(self, fname):
        """
        The method which actually writes the CSV to disk.

        This should be in a separate thread because if the dataset is large then we will end
        up overloading the main thread.
        """
        try:
            fout = open(fname, "wb")
        except IOError:
            self.show_error("Cannot open output file. Is it open elsewhere?")
            return
        csvout =  csv.writer(fout)
        csvout.writerow(self.database.get_headings())
        for row in range(self.gui.tableWidget.rowCount()):
            thisrow = list()
            for col in range(self.gui.tableWidget.columnCount()):
                thisrow.append(self.gui.tableWidget.item(row, col).text())
            csvout.writerow(thisrow)
        fout.close()

    def populate_toolbar(self):
        """
        Method which allows us to programmatically add the toolbar buttons instead of littering
        the initializer with calls to the create_action function.
        """
        create_action(self, "Refresh", fname=":/view-refresh", slot=self.populate_table)
        create_action(self, "Insert Row", fname=":/list-add")
        create_action(self, "Export as CSV", fname=":/document-save-as", slot=self.export_as_csv)
        create_action(self, "Quit", fname=":/system-log-out", slot=sys.exit)

if __name__ == '__main__':
    APPLICATION = QtGui.QApplication(sys.argv)
    MAINWINDOW = MainGui()
    MAINWINDOW.setStatusBar(MAINWINDOW.gui.statusbar)
    MAINWINDOW.show()
    sys.exit(APPLICATION.exec_())
