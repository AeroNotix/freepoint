# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'connections.ui'
#
# Created: Mon Jul 09 10:08:39 2012
#      by: PyQt4 UI code generator 4.9.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName(_fromUtf8("Dialog"))
        Dialog.resize(569, 251)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(_fromUtf8(":/address-book-new")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        Dialog.setWindowIcon(icon)
        self.gridLayout = QtGui.QGridLayout(Dialog)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.buttonBox = QtGui.QDialogButtonBox(Dialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.gridLayout.addWidget(self.buttonBox, 4, 4, 1, 1)
        self.listWidget = QtGui.QListWidget(Dialog)
        self.listWidget.setObjectName(_fromUtf8("listWidget"))
        self.gridLayout.addWidget(self.listWidget, 0, 0, 4, 1)
        self.label_2 = QtGui.QLabel(Dialog)
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.gridLayout.addWidget(self.label_2, 1, 1, 1, 2)
        self.txt_database = QtGui.QLineEdit(Dialog)
        self.txt_database.setObjectName(_fromUtf8("txt_database"))
        self.gridLayout.addWidget(self.txt_database, 1, 3, 1, 2)
        self.label_3 = QtGui.QLabel(Dialog)
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.gridLayout.addWidget(self.label_3, 2, 1, 1, 1)
        self.txt_table = QtGui.QLineEdit(Dialog)
        self.txt_table.setObjectName(_fromUtf8("txt_table"))
        self.gridLayout.addWidget(self.txt_table, 2, 3, 1, 2)
        self.btn_deleteRow = QtGui.QPushButton(Dialog)
        self.btn_deleteRow.setObjectName(_fromUtf8("btn_deleteRow"))
        self.gridLayout.addWidget(self.btn_deleteRow, 4, 2, 1, 2)
        self.btn_editRow = QtGui.QPushButton(Dialog)
        self.btn_editRow.setObjectName(_fromUtf8("btn_editRow"))
        self.gridLayout.addWidget(self.btn_editRow, 4, 1, 1, 1)
        spacerItem = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.gridLayout.addItem(spacerItem, 3, 3, 1, 1)
        self.label_2.setBuddy(self.txt_database)
        self.label_3.setBuddy(self.txt_table)

        self.retranslateUi(Dialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), Dialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), Dialog.reject)
        QtCore.QObject.connect(self.listWidget, QtCore.SIGNAL(_fromUtf8("currentRowChanged(int)")), Dialog.populateFields)
        QtCore.QObject.connect(self.btn_editRow, QtCore.SIGNAL(_fromUtf8("clicked()")), Dialog.editRow)
        QtCore.QObject.connect(self.btn_deleteRow, QtCore.SIGNAL(_fromUtf8("clicked()")), Dialog.deleteRow)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(QtGui.QApplication.translate("Dialog", "Edit Connection", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("Dialog", "Database", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Dialog", "Table", None, QtGui.QApplication.UnicodeUTF8))
        self.btn_deleteRow.setText(QtGui.QApplication.translate("Dialog", "Delete Connection", None, QtGui.QApplication.UnicodeUTF8))
        self.btn_editRow.setText(QtGui.QApplication.translate("Dialog", "Save Edit", None, QtGui.QApplication.UnicodeUTF8))

import resource_rc
