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
        self.populateList()

    def populateList(self):
        """
        Function which inserts all new connections into the QListWidget
        """

        self.clearTable()
        self.connection_map = dict()

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

    def clearFields(self):
        """
        Clears all the fields
        """
        self.gui.txt_host.setText('')
        self.gui.txt_database.setText('')
        self.gui.txt_table.setText('')
        self.gui.txt_username.setText('')
        self.gui.txt_password.setText('')
        self.gui.txt_port.setText('')

    def editRow(self):
        """
        Writes the changes made on the form into the config file
        """

        host = self.gui.txt_host.text()
        database = self.gui.txt_database.text()
        table = self.gui.txt_table.text()
        username = self.gui.txt_username.text()
        password = self.gui.txt_password.text()
        port = self.gui.txt_port.text()
        connection = self.getCurrentItem()
        self.parent.config.set(connection, "host", host)
        self.parent.config.set(connection, "database", database)
        self.parent.config.set(connection, "table", table)
        self.parent.config.set(connection, "username", username)
        self.parent.config.set(connection, "password", password)
        self.parent.config.set(connection, "port", port)

    def deleteRow(self):
        """
        Deletes the row
        """
        self.parent.config.remove_section(self.getCurrentItem())
        self.parent.rewriteConfig()
        self.gui.listWidget.blockSignals(True)
        self.populateList()
        self.gui.listWidget.blockSignals(False)
        self.clearFields()

    def getCurrentItem(self):
        """
        Get the currently selected item
        """
        idx = self.gui.listWidget.currentRow()
        return self.connection_map[self.gui.listWidget.item(idx).text()]
