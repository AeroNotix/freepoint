'''
intention is to make a generic GUI for interacting with a mysql database
'''

import sys
import argparse

from PyQt4 import QtGui

import MySQLdb
import _mysql_exceptions

from database_viewer.ui.mainwindow_UI import Ui_MainWindow
from database_viewer.ui.dlg_sql_connection import SQLDisplaySetup

from database_viewer.table_tools.tools import get_headings, itersql
from database_viewer.table_tools.argument import Argument

## Argument creation
PARSER = argparse.ArgumentParser()
ARGS = [
    Argument('db', help='Database you wish to connect to'),
    Argument('password', help='password for the database'),
    Argument('user', help='user for the database'),
    Argument('table', help='which table to connect to')
]

for arg in ARGS:
    PARSER.add_argument(arg.name, **arg.data)

RESULTS = PARSER.parse_args()


class MainGui(QtGui.QMainWindow):
    """
    Main GUI
    """

    def __init__(self, parent=None):
        '''
        Creation and main check
        '''

        QtGui.QMainWindow.__init__(self, parent)
        self.gui = Ui_MainWindow()
        self.gui.setupUi(self)
        self.toolbar = self.addToolBar("Toolbar")
        self.populated = False

        if RESULTS.db:
            # if we got command line arguments, open that
            self.populate_table(user=RESULTS.user,
                                password=RESULTS.password,
                                using_db=RESULTS.db,
                                table=RESULTS.table)
        else:
            # else allow the user to enter the details via
            # the gui
            self.openConnectionDialog()


    def populate_table(self, host='localhost',
                       user=None, password=None,
                       using_db=None, table=None,
                       query=None):
        """
        Opens and displays a MySQL table

        Mutates the table's headings and the data in the table with
        the data returned from the select on the passed in database.
        """

        # defaulted to this, not in arg list because reasons
        if not query:
            query = '''SELECT * FROM %s''' % table

        # connect to mysql database
        try:
            database = MySQLdb.connect(
                host=host, user=user,
                passwd=password, db=using_db
            )

        except _mysql_exceptions.OperationalError, error:
            # on any error report to the user and return
            QtGui.QMessageBox.warning(self, "Error", str(error))
            return

        self.clear_table()

        # get the headings so we can set up the table
        headings = get_headings(database, query)

        # set the column size according to the headings
        self.gui.tableWidget.setColumnCount(len(headings))
        # set the labels to the column names
        self.gui.tableWidget.setHorizontalHeaderLabels(headings)

        # iterate through the query set and get the data into the table
        for idx, data in enumerate(
                 itersql(database, query)):

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
        pass

    def openConnectionDialog(self):
        '''
        Creates an instance of the connection dialog and show it.
        '''

        setup_dlg = SQLDisplaySetup(self)
        setup_dlg.show()

    def clear_table(self):
        '''
        Method to clear the table.

        We iterate forwards but we delete the opposite side of the table because
        we can be safe knowing that we aren't iterating over the iterable of rows
        whilst changing the size of the iterable.
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
