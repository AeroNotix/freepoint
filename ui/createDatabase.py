from PyQt4 import QtGui, QtCore
from qtsqlviewer.ui.create_new_table_UI import Ui_Dialog


class CreateNewTable(QtGui.QDialog):
    def __init__(self, parent=None):
        super(CreateNewTable, self).__init__(parent)
        self.gui = Ui_Dialog()
        self.gui.setupUi(self)
        self.gui.tabWidget.tabBar().setVisible(False)
        
    def changeFieldDescriptions(self, index):
        """
        """
        self.gui.tabWidget.setCurrentIndex(~index-1)

    def acceptFieldAdd(self):
        print self.sender().parent().objectName()








