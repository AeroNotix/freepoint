from PyQt4 import QtGui, QtCore

from qtsqlviewer.ui.connections_UI import Ui_Dialog


class ConnectionDialog(QtGui.QDialog):
    """
    Creates the ConnectionDialog.

    This dialog is for managing all the active connections
    currently configured for this installation.
    """

    def __init__(self, parent=None):
        """
        Customize our instance.
        """ 
        
        QtGui.QWidget.__init__(self, parent)
        self.gui = Ui_Dialog()
        self.gui.setupUi(self)
        self.parent = parent
        self.connection_map = dict()
        self.populateList()


    def populateList(self):
        """
        Function which inserts all new connections into the QListWidget
        """

        self.clearTable()

        for connection in self.parent.config.sections():
            host = self.parent.config.get(connection, "host")
            database = self.parent.config.get(connection, "database")
            table = self.parent.config.get(connection, "table")
            displaystring = QtCore.QString(host+"."+database+"."+table)
            self.gui.listWidget.addItem(QtGui.QListWidgetItem(displaystring))
            self.connection_map[displaystring] = connection
            
    def populateFields(self, idx):
        """
        Populates the text boxes with the correct data for whatever has just
        been clicked on.
        """
        connection = self.connection_map[self.gui.listWidget.item(idx).text()]
        self.gui.txt_host.setText(self.parent.config.get(connection, "host"))
        self.gui.txt_database.setText(self.parent.config.get(connection, "database"))
        self.gui.txt_table.setText(self.parent.config.get(connection, "table"))
        self.gui.txt_username.setText(self.parent.config.get(connection, "username"))
        self.gui.txt_password.setText(self.parent.config.get(connection, "password"))
        self.gui.txt_port.setText(self.parent.config.get(connection, "port"))
                
    def clearTable(self):
        """
        Clears all the table of the QListWidgetItems
        """

        for row in range(self.gui.listWidget.count()):
            self.gui.listWidget.takeItem(row)
