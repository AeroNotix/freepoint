import sys

from PyQt4 import QtCore, QtGui

from database_viewer.ui.sql_setup_UI import Ui_frm_sql_data_entry
from database_viewer.table_tools.tools import to_unicode

class SQLDisplaySetup(QtGui.QDialog):

    '''
    Form for entering SQL information,

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
        if self.parent.populated:
            self.populate_fields()

    def accept(self):

        '''
        When the dlg's OK button is pressed
        '''
       
        self.hide()
        self.parent.host = to_unicode(self.gui.txt_host.text())
        self.parent.user = to_unicode(self.gui.txt_username.text())
        self.parent.password = to_unicode(self.gui.txt_password.text())
        self.parent.using_db = to_unicode(self.gui.txt_database.text())
        self.parent.table = to_unicode(self.gui.txt_table.text())
        self.parent.query = to_unicode(self.gui.txt_sql_entry.toPlainText())
        self.parent.populated = True

    def populate_fields(self):
        '''
        Populates the dialog's fields with the info stored on the parent which
        created this instance if we have already connected to a database.
        '''

        self.gui.txt_host.setText(self.parent.host)
        self.gui.txt_username.setText(self.parent.user)
        self.gui.txt_password.setText(self.parent.password)
        self.gui.txt_database.setText(self.parent.using_db)
        self.gui.txt_table.setText(self.parent.table)
        self.gui.txt_sql_entry.setText(self.parent.query)
