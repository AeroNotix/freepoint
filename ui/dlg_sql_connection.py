from PyQt4 import QtGui

from qtsqlviewer.ui.sql_setup_UI import Ui_frm_sql_data_entry
from qtsqlviewer.table_tools.tools import to_unicode, Database

class SQLDisplaySetup(QtGui.QDialog):
    """
    Form for entering SQL information,

    The GUI method opposed to the command line arguments
    """

    def __init__(self, parent=None):
        """
        Creation of the dlg
        """
        QtGui.QWidget.__init__(self, parent)
        self.gui = Ui_frm_sql_data_entry()
        self.gui.setupUi(self)
        self.parent = parent
        if self.parent.populated:
            self.populate_fields()

    def accept(self):
        """
        When the dlg's OK button is pressed
        """

        self.hide()

        host = self.gui.txt_host.text()
        username = self.gui.txt_username.text()
        password = self.gui.txt_password.text()
        database = self.gui.txt_database.text()
        table = self.gui.txt_table.text()
        port = self.gui.txt_port.text()

        self.parent.database = Database(
            to_unicode(host),
            to_unicode(username),
            to_unicode(password),
            to_unicode(database),
            to_unicode(table),
            to_unicode(port)
            )

        self.parent.populated = True

        sections = len(self.parent.config.sections())
        for num in range(sections):
            num = str(num)
            if not all([
                host == self.parent.config.get("connection-%s" % num, "host"),
                database == self.parent.config.get("connection-%s" % num, "database")
                ]):
                continue
            else:
                return

        new_section = "connection-%s" % str(sections)

        if not self.parent.config.has_section(new_section):
            self.parent.config.add_section(new_section)
            self.parent.config.set(new_section, "host", host)
            self.parent.config.set(new_section, "username", username)
            self.parent.config.set(new_section, "password", password)
            self.parent.config.set(new_section, "database", database)
            self.parent.config.set(new_section, "table", table)
            self.parent.config.set(new_section, "port", port)
            with open(self.parent.configpath, 'wb') as configout:
                self.parent.config.write(configout)

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
