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
        self.populate_list()

    def populate_list(self):
        """
        Function which inserts all new connections into the QListWidget
        """

        for connection in self.parent.config.sections():
            host = self.parent.config.get(connection, "host")
            database = self.parent.config.get(connection, "database")
            table = self.parent.config.get(connection, "table")
            self.gui.listWidget.addItem(QtGui.QListWidgetItem(host+"."+database+"."+"table"))
