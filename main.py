"""
intention is to make a generic GUI for interacting with a mysql database
"""

import os
import sys
import argparse
import ConfigParser
import urllib

from PyQt4 import QtGui, QtCore

import _mysql_exceptions
import simplejson

from qtsqlviewer.ui.mainwindow_UI import Ui_MainWindow
from qtsqlviewer.ui.dlg_sql_connection import SQLDisplaySetup

from qtsqlviewer.table_tools.tools import get_headings, table_wrapper, Database
from qtsqlviewer.table_tools.argument import Argument
from qtsqlviewer.table_tools.mysql_error_codes import mysqlerror

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
        self.toolbar = self.addToolBar("Toolbar")
        self.populated = False
        self.cell = ''
        self.param_url = "http://localhost:12345/params/%s"
        self.config = ConfigParser.RawConfigParser()
        self.configpath = os.path.join(CWD, "conf.cfg")
        self.current_table = "connection-0"
        self.actionList = []

        if RESULTS.db:
            # if we got command line arguments, open that
            self.database = Database(
                RESULTS.host,
                RESULTS.user,
                RESULTS.password,
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
                host = self.config.get("connection-0", "host")
                port = self.config.get("connection-0", "port")
                if not host:
                    host = "localhost"
                if not port:
                    port = 3306
                self.database = Database(
                    host,
                    self.config.get("connection-0", "username"),
                    self.config.get("connection-0", "password"),
                    self.config.get("connection-0", "database"),
                    self.config.get("connection-0", "table"),
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
        else:
            # else allow the user to enter the details via
            # the gui
            self.openConnectionDialog()
        self.addMenuActions()

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

        try:
            http_get = urllib.urlopen(
                # string interpolation
                self.param_url % self.database.using_db
            )
            json = simplejson.loads(http_get.read())
        except IOError as error:
            print "Error: %s" % error

        # get the headings so we can set up the table
        try:
            query = """SELECT * FROM %s""" % self.database.table
            queryset = self.database.query(query)
            self.headings = get_headings(self.database, query)
        except _mysql_exceptions.OperationalError, error:
            QtGui.QMessageBox.warning(self, "Error", str(error))
            return
        except _mysql_exceptions.ProgrammingError as error:
            self.show_message(mysqlerror(error), time=10000)

        # set the column size according to the headings
        self.gui.tableWidget.setColumnCount(len(self.headings))
        # set the labels to the column names
        self.gui.tableWidget.setHorizontalHeaderLabels(self.headings)

        # iterate through the query set and get the data into the table
        # for idx, data in enumerate(itersql(self.database, query)):
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
        When a cell is edited the data is written back into the database.

        This is highly alpha code. There is no error handling, no locking.
        Nothing. It's a work in progress.

        :param xrow: :class:`Int` which is passed directly from the signal
        :param ycol: :class:`Int` which is passed directly from the signal
        :returns: None
        """

        try:
            self.database.connect()
        except _mysql_exceptions.OperationalError as error:
            if error[0] == 2003:
                self.show_message("Cannot connect to database", time=10000)
                return
            else:
                raise

        cur = self.database.cursor()
        sql = ' '.join(
            # we create somethings ourselves using string interpolation
            # this is because the MySQLdb doesn't let you parameterize
            # things like table names and column titles.
            [
                "UPDATE %s" % self.database.table,
                "SET %s" % self.headings[ycol] + "=%s",
                "WHERE id=%s"
            ]
        )
        cur.execute(
            # we pass our SQL string as the first argument and the second
            # argument are the strings to parameterize into the first.
            sql,
            (
                self.gui.tableWidget.item(xrow, ycol).text(),
                self.gui.tableWidget.item(xrow, 0).text()
            )
        )

        try:
            self.database.commit()
            self.database.close()
        except _mysql_exceptions.OperationalError as error:
            self.show_message(mysqlerror(error), time=10000)
            return

        self.show_message("Data has been saved to the database")

    def openConnectionDialog(self):
        """
        Creates an instance of the connection dialog and shows it.
        """
        setup_dlg = SQLDisplaySetup(self)
        setup_dlg.exec_()
        return

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

    def storeCell(self, xrow, ycol):
        """
        When a cell is entered we store it in case the validation for the
        data that the user enters fails.

        :param xrow: :class:`Int` which is passed directly from the signal
        :param ycol: :class:`Int` which is passed directly from the signal
        :returns: None
        """
        self.cell = self.gui.tableWidget.item(xrow, ycol).text()

    def show_message(self, message, time=1000):
        """
        Method which sets the status message for a period of time
        """
        self.gui.statusbar.showMessage(message, time)

    def changeConnection(self, section):
        '''
        Changes the connection to the one specified under the `section`
        heading.
        '''

        if self.current_table == section:
            return

        self.database.close()
        self.clear_table()

        self.database.host = self.config.get(section, "host")
        self.database.database = self.config.get(section, "database")
        self.database.table = self.config.get(section, "table")
        self.database.user = self.config.get(section, "username")
        self.database.password = self.config.get(section, "password")
        self.database.port = self.config.get(section, "port")

        self.populate_table()
        self.current_table = section

    def addMenuActions(self):
        '''
        The submenu which contains a list of all connection details
        needs to dynamically populated at launch. This is the method
        which does that.

        First we remove all the current menu items, then we reattach
        all the menu items. We do this because in-between calling this
        the amount of actions can change for this menu depening on
        if a new connection was created or not.

        :returns: :class:`None`
        '''

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
        '''
        Parses the config file. We can use this method after we edit the config file
        so that the config file is reloaded.
        '''
        self.config.read(self.configpath)
        self.addMenuActions()

if __name__ == '__main__':
    APPLICATION = QtGui.QApplication(sys.argv)
    MAINWINDOW = MainGui()
    MAINWINDOW.setStatusBar(MAINWINDOW.gui.statusbar)
    MAINWINDOW.show()
    sys.exit(APPLICATION.exec_())
