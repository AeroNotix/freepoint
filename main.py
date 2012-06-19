'''
intention is to make a generic GUI for interacting with a mysql database
'''

import os
import sys
import argparse
import ConfigParser
import urllib

from PyQt4 import QtGui, QtCore

import MySQLdb
import _mysql_exceptions
import simplejson

from qtsqlviewer.ui.mainwindow_UI import Ui_MainWindow
from qtsqlviewer.ui.dlg_sql_connection import SQLDisplaySetup

from qtsqlviewer.table_tools.tools import get_headings, itersql, table_wrapper
from qtsqlviewer.table_tools.argument import Argument

CWD = os.path.dirname(__file__)
CONFS = ConfigParser.RawConfigParser()
FPATH = os.path.join(CWD, "conf.cfg")

## Argument creation
PARSER = argparse.ArgumentParser()
ARGS = (
    Argument('db', help='Database you wish to connect to'),
    Argument('password', help='Password for the database'),
    Argument('user', help='User you want to connect as'),
    Argument('table', help='Which table to connect to'),
    Argument('host', help='Which host you want to connect to'),
    Argument('port', help='The port you want to connect through')
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
        self.host = ''
        self.port = ''
        self.cell_data = None
        self.param_url = "http://localhost:12345/params/%s"

        if RESULTS.db:
            # if we got command line arguments, open that
            self.host = RESULTS.host
            self.password = RESULTS.password
            self.table = RESULTS.table
            self.user = RESULTS.user
            self.using_db = RESULTS.db
            self.port = RESULTS.port
            self.populate_table()
        elif os.path.isfile(FPATH):
            # if we didn't get command line arguments check if
            # there is a config file.
            CONFS.read(FPATH)
            try:
                self.host = CONFS.get("connection-1", "host")
                self.password = CONFS.get("connection-1", "password")
                self.table = CONFS.get("connection-1", "table")
                self.username = CONFS.get("connection-1", "user")
                self.using_db = CONFS.get("connection-1", "database")
                self.port = CONFS.get("connection-1", "database")
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

    def reconnect(self):
        # connect to mysql database
            self.database = MySQLdb.connect(
                host=self.host, user=self.user,
                passwd=self.password, db=self.using_db,
                port=int(self.port)
            )

    @table_wrapper
    def populate_table(self):
        """
        Opens and displays a MySQL table

        Mutates the table's headings and the data in the table with
        the data returned from the select on the passed in database.
        """

        self.clear_table()

        if not self.host:
            self.host = 'localhost'
        if not self.port:
            self.port = 3306

        try:
            self.reconnect()
        except _mysql_exceptions.OperationalError, error:
            # on any error report to the user and return
            QtGui.QMessageBox.warning(self, "Error", str(error))
            return

        try:
            http_get = urllib.urlopen(
                # string interpolation
                self.param_url % self.using_db
            )
            json = simplejson.loads(http_get.read())
        except IOError as error:
            print "Error: %s" % error

        # get the headings so we can set up the table
        try:
            query = '''SELECT * FROM %s''' % self.table
            cursor = self.database.cursor()
            queryset = [result for result in itersql(self.database, query)]
            self.headings = get_headings(self.database, query)
        except _mysql_exceptions.OperationalError, error:
            QtGui.QMessageBox.warning(self, "Error", str(error))
            return

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
                    idx, num, QtGui.QTableWidgetItem(str(info))
                )
        # close the connection
        self.database.close()

    def changeTable(self, xrow, ycol):
        '''
        When a cell is edited the data is written back into the database.

        This is highly alpha code. There is no error handling, no locking.
        Nothing. It's a work in progress.

        :param xrow: :class:`Int` which is passed directly from the signal
        :param ycol: :class:`Int` which is passed directly from the signal
        :returns: None
        '''

        self.reconnect()
        cur = self.database.cursor()
        sql = ' '.join(
            # we create somethings ourselves using string interpolation
            # this is because the MySQLdb doesn't let you parameterize
            # things like table names and column titles.
            [
                "UPDATE %s" % self.table,
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
        self.database.commit()
        self.database.close()

    def openConnectionDialog(self):
        '''
        Creates an instance of the connection dialog and show it.
        '''

        setup_dlg = SQLDisplaySetup(self)

        self.connect(
            setup_dlg.gui.buttonBox,
            QtCore.SIGNAL('accepted()'),
            self.populate_table
        )

        setup_dlg.exec_()
        return

    def clear_table(self):
        '''
        Method to clear the table.

        We iterate forwards but we delete the opposite side of the table
        because we can be safe knowing that we aren't iterating over the
        iterable of rows whilst changing the size of the iterable.

        :returns: :class:`None`
        '''
        if self.gui.tableWidget.rowCount():
            row_count = self.gui.tableWidget.rowCount()
            for row in range(row_count + 1):
                self.gui.tableWidget.removeRow(row_count - row)

    def storeCell(self, xrow, ycol):
        '''
        When a cell is entered we store it in case the validation for the
        data that the user enters fails.

        :param xrow: :class:`Int` which is passed directly from the signal
        :param ycol: :class:`Int` which is passed directly from the signal
        :returns: None
        '''
        self.cell = self.gui.tableWidget.item(xrow, ycol).text()


if __name__ == '__main__':

    APPLICATION = QtGui.QApplication(sys.argv)
    MAINWINDOW = MainGui()
    MAINWINDOW.show()
    sys.exit(APPLICATION.exec_())
