import sys

from PyQt4 import QtCore, QtGui

from database_viewer.ui.sql_setup_UI import Ui_frm_sql_data_entry
from database_viewer.table_tools.tools import to_unicode

class SQLDisplaySetup(QtGui.QDialog):

    '''
    Form for enterting SQL information,

    The GUI method opposed to the command line arguments
    '''

    def __init__(self, parent=None):

        '''
        Creation of the dlg
        '''
        
        QtGui.QWidget.__init__(self, parent)
        self.gui = Ui_frm_sql_data_entry()
        self.gui.setupUi(self)
        self.parent = parent

    def accept(self):

        '''
        When the dlg's OK button is pressed
        '''
        self.hide()

        connection_details = {
            'user': to_unicode(self.gui.txt_username.text()),
            'password': to_unicode(self.gui.txt_password.text()),
            'db': to_unicode(self.gui.txt_sql_db.text()),
            'table': to_unicode(self.gui.txt_table.text()),
            'query': self.gui.txt_sql_entry.toPlainText()
            }
    
        self.parent.populate_table(**connection_details)
        
