'''
intention is to make a generic GUI for interacting with a mysql database
'''

import sys
import argparse

from PyQt4 import QtCore, QtGui

import MySQLdb
import _mysql_exceptions

import database_viewer.ui.mainwindow_UI
from database_viewer.ui.mainwindow_UI import Ui_MainWindow
from database_viewer.ui.dlg_sql_connection import SQLDisplaySetup

from database_viewer.table_tools.table_wrapper import populate_wrapper
from database_viewer.table_tools.tools import *
from database_viewer.table_tools.argument import *

## Argument creation
parser = argparse.ArgumentParser()
args = [
    Argument('db', help='Database you wish to connect to'),
    Argument('password', help='password for the database'),
    Argument('user', help='user for the database'),
    Argument('table', help='which table to connect to')
]

for arg in args:
    parser.add_argument(arg.name, **arg.data)
    
results = parser.parse_args()


class MainGui(QtGui.QMainWindow):

    """
    Main GUI
    """

    def __init__(self, parent=None):
        QtGui.QMainWindow.__init__(self, parent)
        self.gui = Ui_MainWindow()
        self.gui.setupUi(self)
        self.toolbar = self.addToolBar("Toolbar")

        if results.db:
            # if we got command line arguments, open that
            self.populate_table(user=results.user,
                                password=results.password,
                                db=results.db,
                                table=results.table)
        else:
            # else allow the user to enter the details via
            # the gui
            setup_dlg = SQLDisplaySetup(self)
            setup_dlg.show()

            
        
    def populate_table(self, db_host='localhost',
                       user=None, password=None,
                       db=None, table=None, query=None):
        """
        Opens and displays a MySQL table

        Mutates the table's headings and the data in the table with
        the data returned from the select on the passed in database.
        """
        if not query:
            query='''SELECT * FROM tbl_Queries'''

        # connect to mysql database
        try:
            db = MySQLdb.connect(host=db_host, user=user,
                                 passwd=password, db=db)
            
        except _mysql_exceptions.OperationalError, e:

            # on any error report to the user and return
            QtGui.QMessageBox.warning(self, "Error", str(e))
            return
        
        cursor = db.cursor()

        # get the headings so we can set up the table
        headings = get_headings(db, query)

        # set the column size according to the headings
        self.gui.tableWidget.setColumnCount(len(headings))
        # set the labels to the column names
        self.gui.tableWidget.setHorizontalHeaderLabels(headings)

        # iterate through the query set and get the data into the table
        for idx, data in enumerate(
                 itersql(db, query)):

            if not data:
                break

            self.gui.tableWidget.insertRow(idx)
            for num, info in enumerate(data):
                self.gui.tableWidget.setItem(idx, num,
                            QtGui.QTableWidgetItem(str(info)))
                
        # data is in-memory, close the connection. 
        db.close()
            
if __name__ == '__main__':
    
    APPLICATION = QtGui.QApplication(sys.argv)
    MAINWINDOW = MainGui()
    MAINWINDOW.show()
    sys.exit(APPLICATION.exec_())
