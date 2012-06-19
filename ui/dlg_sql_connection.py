from PyQt4 import QtGui

from qtsqlviewer.ui.sql_setup_UI import Ui_frm_sql_data_entry
from qtsqlviewer.table_tools.tools import to_unicode, Database


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
        self.parent.database = Database(
            to_unicode(self.gui.txt_host.text()),
            to_unicode(self.gui.txt_username.text()),
            to_unicode(self.gui.txt_password.text()),
            to_unicode(self.gui.txt_database.text()),
            to_unicode(self.gui.txt_table.text()),
            to_unicode(self.gui.txt_port.text())
            )
        self.parent.populated = True

    def populate_fields(self):
        '''
        Populates the dialog's fields with the info stored on the parent which
        created this instance if we have already connected to a database.
        '''

        self.gui.txt_host.setText(self.parent.database.host)
        self.gui.txt_username.setText(self.parent.database.user)
        self.gui.txt_password.setText(self.parent.database.password)
        self.gui.txt_database.setText(self.parent.database.using_db)
        self.gui.txt_table.setText(self.parent.database.table)
        self.gui.txt_port.setText(str(self.parent.database.port))
