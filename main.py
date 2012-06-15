'''
intention is to make a generic GUI for interacting with a mysql database
'''

import os
import sys
import argparse
import ConfigParser

from PyQt4 import QtGui, QtCore

import MySQLdb
import _mysql_exceptions

from database_viewer.ui.mainwindow_UI import Ui_MainWindow
from database_viewer.ui.dlg_sql_connection import SQLDisplaySetup

from database_viewer.table_tools.tools import get_headings, itersql
from database_viewer.table_tools.argument import Argument

CWD = os.path.dirname(__file__)
confs = ConfigParser.RawConfigParser()
fpath = os.path.join(CWD, "conf.cfg")

## Argument creation
PARSER = argparse.ArgumentParser()
ARGS = [
    Argument('db', help='Database you wish to connect to'),
    Argument('password', help='Password for the database'),
    Argument('user', help='User you want to connect as'),
    Argument('table', help='Which table to connect to'),
    Argument('host', help='Which host you want to connect to')
]

for arg in ARGS:
    PARSER.add_argument(arg.name, **arg.data)

RESULTS = PARSER.parse_args()


class MainGui(QtGui.QMainWindow):
    """
    Main GUI class which creates the window, responds to user input and holds
    the slots assigned to the class' signals.
    """

    def __init__(self, parent=None):
        '''
        Creation and main check

        :param parent: A Qt object which is the 'owner' of this object.
                       This is used if the parent is closed/killed therefore
                       this object must also be killed.
        '''

        QtGui.QMainWindow.__init__(self, parent)
        self.gui = Ui_MainWindow()
        self.gui.setupUi(self)
        self.toolbar = self.addToolBar("Toolbar")
        self.populated = False

        if RESULTS.db:
            print 'results'
            # if we got command line arguments, open that
            self.host = RESULTS.host
            self.password = RESULTS.password
            self.table = RESULTS.table
            self.user = RESULTS.user
            self.using_db = RESULTS.db
            self.populate_table()
        elif os.path.isfile(fpath):
            print 'os.path'
            # if we didn't get command line arguments check if
            # there is a config file.
            confs.read(fpath)
            try:
                self.host = confs.get("connection-1", "host")
                self.password = confs.get("connection-1", "password")
                self.table = confs.get("connection-1", "table")
                self.username = confs.get("connection-1", "user")
                self.using_db = confs.get("connection-1", "database")
                self.populate_table()
            except ConfigParser.NoSectionError:
                # if the file exists but it doesn't have the required section
                # then something is wrong. So we error.
                QtGui.QMessageBox.warning(self, "Error", "Config file error",
                                          QtGui.QMessageBox.Ok)
                sys.exit(-1)
        else:
            # else allow the user to enter the details via
            # the gui
            self.openConnectionDialog()

    def populate_table(self):
        """
        Opens and displays a MySQL table

        Mutates the table's headings and the data in the table with
        the data returned from the select on the passed in database.
        """

        self.clear_table()

        # defaulted to this, not in arg list because reasons
        if not self.query:
            self.query = '''SELECT * FROM %s''' % self.table

        # connect to mysql database
        try:
            database = MySQLdb.connect(
                host=self.host, user=self.user,
                passwd=self.password, db=self.using_db
            )
        except _mysql_exceptions.OperationalError, error:
            # on any error report to the user and return
            QtGui.QMessageBox.warning(self, "Error", str(error))
            return

        # get the headings so we can set up the table
        try:
            headings = get_headings(database, self.query)
        except _mysql_exceptions.OperationalError, error:
            QtGui.QMessageBox.warning(self, "Error", str(error))
            return

        # set the column size according to the headings
        self.gui.tableWidget.setColumnCount(len(headings))
        # set the labels to the column names
        self.gui.tableWidget.setHorizontalHeaderLabels(headings)

        # iterate through the query set and get the data into the table
        for idx, data in enumerate(
                 itersql(database, self.query)):

            if not data:
                break

            self.gui.tableWidget.insertRow(idx)
            for num, info in enumerate(data):
                self.gui.tableWidget.setItem(
                    idx, num, QtGui.QTableWidgetItem(str(info))
                )

        # data is in-memory, close the connection.
        database.close()

    def changeTable(self, x, y):
        '''
        Stub method for what will become SQL Inserts back into the database when
        self.gui.tableWidget emits a signal of "entryChanged()"
        '''
        print x, y

    def openConnectionDialog(self):
        '''
        Creates an instance of the connection dialog and show it.
        '''

        setup_dlg = SQLDisplaySetup(self)
        self.connect(setup_dlg.gui.buttonBox, QtCore.SIGNAL('accepted()'), self.populate_table)
        setup_dlg.exec_()
        return

    def clear_table(self):
        '''
        Method to clear the table.

        We iterate forwards but we delete the opposite side of the table because
        we can be safe knowing that we aren't iterating over the iterable of rows
        whilst changing the size of the iterable.

        :returns: :class:`None`
        '''
        if self.gui.tableWidget.rowCount():
            row_count = self.gui.tableWidget.rowCount()
            for row in range(row_count+1):
                self.gui.tableWidget.removeRow(row_count - row)

if __name__ == '__main__':

    APPLICATION = QtGui.QApplication(sys.argv)
    MAINWINDOW = MainGui()
    MAINWINDOW.show()
    sys.exit(APPLICATION.exec_())
