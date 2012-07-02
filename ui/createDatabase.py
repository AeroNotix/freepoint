from PyQt4 import QtGui, QtCore
from qtsqlviewer.ui.create_new_table_UI import Ui_Dialog


formA = [
    QtGui.QComboBox,
    QtGui.QComboBox,
    QtGui.QComboBox,
    QtGui.QComboBox,
    QtGui.QComboBox,
    QtGui.QComboBox
    ]

class CreateNewTable(QtGui.QDialog):
    def __init__(self, parent=None):
        super(CreateNewTable, self).__init__(parent)
        self.gui = Ui_Dialog()
        self.gui.setupUi(self)

    def changeFieldDescriptions(self):
        for i in range(self.gui.verticalLayout_2.count()):
            self.gui.verticalLayout_2.itemAt(i).widget().close()
        for item in formA:
            self.gui.verticalLayout_2.addWidget(item(self.gui.groupBox_2))
