from PyQt4 import QtGui, QtCore

from freepoint.ui.connections_UI import Ui_Dialog


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
            database = self.parent.config.get(connection, "database")
            table = self.parent.config.get(connection, "table")
            displaystring = QtCore.QString(database+"."+table)
            self.gui.listWidget.addItem(QtGui.QListWidgetItem(displaystring))
            self.connection_map[displaystring] = connection

    def populateFields(self, idx):
        """
        Populates the text boxes with the correct data for whatever has just
        been clicked on.
        """
        connection = self.connection_map[self.gui.listWidget.item(idx).text()]
        self.gui.txt_database.setText(self.parent.config.get(connection, "database"))
        self.gui.txt_table.setText(self.parent.config.get(connection, "table"))
                
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
        self.gui.txt_database.setText('')
        self.gui.txt_table.setText('')

    def editRow(self):
        """
        Writes the changes made on the form into the config file
        """

        database = self.gui.txt_database.text()
        table = self.gui.txt_table.text()
        connection = self.getCurrentItem()
        self.parent.config.set(connection, "database", database)
        self.parent.config.set(connection, "table", table)
        self.gui.listWidget.blockSignals(True)
        self.clearTable()
        self.parent.rewriteConfig()
        self.populateList()
        self.clearFields()
        self.gui.listWidget.blockSignals(False)
        self.parent.addMenuActions()

    def deleteRow(self):
        """
        Deletes the row
        """

        idx = self.gui.listWidget.currentRow()
        section = self.getCurrentItem()
        if section is None:
            return
        self.parent.config.remove_section(section)
        self.gui.listWidget.blockSignals(True)
        self.gui.listWidget.takeItem(idx)
        self.clearTable()
        self.parent.rewriteConfig()
        self.populateList()
        self.clearFields()
        self.gui.listWidget.blockSignals(False)
        self.parent.addMenuActions()

    def getCurrentItem(self):
        """
        Get the currently selected item
        """

        idx = self.gui.listWidget.currentRow()
        try:
            return self.connection_map[self.gui.listWidget.item(idx).text()]
        except AttributeError:
            return None
