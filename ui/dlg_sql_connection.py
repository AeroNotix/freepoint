import sys

from PyQt4 import QtCore, QtGui

from database_viewer.ui.sql_setup_UI import Ui_frm_sql_data_entry

class SQLDisplaySetup(QtGui.QDialog):

    '''
    Form for enterting SQL information,

    The GUI method opposed to the command line arguments
    '''

    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.gui = Ui_frm_sql_data_entry()
        self.gui.setupUi(self)

    
