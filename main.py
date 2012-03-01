'''
intention is to make a generic GUI for interacting with a mysql database
'''

import sys

from PyQt4 import QtCore, QtGui
import MySQLdb

import database_viewer.ui.mainwindow_UI
from database_viewer.ui.mainwindow_UI import Ui_MainWindow
from database_viewer.table_tools.table_wrapper import populate_wrapper
from database_viewer.table_tools.tools import *

class MainGui(QtGui.QMainWindow):

    """
    Main GUI
    """

    def __init__(self, user='aero', parent=None):
        QtGui.QMainWindow.__init__(self, parent)
        self.gui = Ui_MainWindow()
        self.gui.setupUi(self)
        self.user = user
        

    def populate_table(self, db_host='localhost',
                       user=None, password=None,
                       db=None, table=None):
        """
        Opens and displays a MySQL table

        Mutates the table's headings and the data in the table with
        the data returned from the select on the passed in database.
        """
        
        db = MySQLdb.connect(host=db_host, user=user,
                             passwd=password, db=db)
        cursor = db.cursor()
        headings = get_headings(db, table)
        
        self.gui.tableWidget.setColumnCount(len(headings))
        self.gui.tableWidget.setHorizontalHeaderLabels(headings)

        
        
if __name__ == '__main__':
    
    APPLICATION = QtGui.QApplication(sys.argv)
    MAINWINDOW = MainGui()
    MAINWINDOW.show()
    MAINWINDOW.populate_table(user='aero', password='colgate',
                              db='db_reporting', table='tbl_Queries')
    sys.exit(APPLICATION.exec_())
