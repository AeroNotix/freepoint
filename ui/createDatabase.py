from PyQt4 import QtGui, QtCore
from qtsqlviewer.ui.create_new_table_UI import Ui_Dialog


class CreateNewTable(QtGui.QDialog):
    def __init__(self, parent=None):
        super(CreateNewTable, self).__init__(parent)
        self.gui = Ui_Dialog()
        self.gui.setupUi(self)
    def changeFieldDescriptions(self):
        print "here"
    
