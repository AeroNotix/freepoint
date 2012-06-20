# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'connections.ui'
#
# Created: Wed Jun 20 16:10:24 2012
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
        self.listWidget = QtGui.QListWidget(Dialog)
        self.listWidget.setObjectName(_fromUtf8("listWidget"))
        self.gridLayout.addWidget(self.listWidget, 0, 0, 6, 1)
        self.label = QtGui.QLabel(Dialog)
        self.label.setObjectName(_fromUtf8("label"))
        self.gridLayout.addWidget(self.label, 0, 1, 1, 1)
        self.txt_host = QtGui.QLineEdit(Dialog)
        self.txt_host.setObjectName(_fromUtf8("txt_host"))
        self.gridLayout.addWidget(self.txt_host, 0, 3, 1, 2)
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
        self.label_4 = QtGui.QLabel(Dialog)
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.gridLayout.addWidget(self.label_4, 3, 1, 1, 1)
        self.txt_port = QtGui.QLineEdit(Dialog)
        self.txt_port.setObjectName(_fromUtf8("txt_port"))
        self.gridLayout.addWidget(self.txt_port, 3, 3, 1, 2)
        self.label_5 = QtGui.QLabel(Dialog)
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.gridLayout.addWidget(self.label_5, 4, 1, 1, 2)
        self.txt_username = QtGui.QLineEdit(Dialog)
        self.txt_username.setObjectName(_fromUtf8("txt_username"))
        self.gridLayout.addWidget(self.txt_username, 4, 3, 1, 2)
        self.label_6 = QtGui.QLabel(Dialog)
        self.label_6.setObjectName(_fromUtf8("label_6"))
        self.gridLayout.addWidget(self.label_6, 5, 1, 1, 2)
        self.txt_password = QtGui.QLineEdit(Dialog)
        self.txt_password.setObjectName(_fromUtf8("txt_password"))
        self.gridLayout.addWidget(self.txt_password, 5, 3, 1, 2)
        self.btn_deleteRow = QtGui.QPushButton(Dialog)
        self.btn_deleteRow.setObjectName(_fromUtf8("btn_deleteRow"))
        self.gridLayout.addWidget(self.btn_deleteRow, 6, 2, 1, 2)
        self.buttonBox = QtGui.QDialogButtonBox(Dialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.gridLayout.addWidget(self.buttonBox, 6, 4, 1, 1)
        self.btn_editRow = QtGui.QPushButton(Dialog)
        self.btn_editRow.setObjectName(_fromUtf8("btn_editRow"))
        self.gridLayout.addWidget(self.btn_editRow, 6, 1, 1, 1)
        self.label.setBuddy(self.txt_host)
        self.label_2.setBuddy(self.txt_database)
        self.label_3.setBuddy(self.txt_table)
        self.label_4.setBuddy(self.txt_port)
        self.label_5.setBuddy(self.txt_username)
        self.label_6.setBuddy(self.txt_password)

        self.retranslateUi(Dialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), Dialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), Dialog.reject)
        QtCore.QObject.connect(self.listWidget, QtCore.SIGNAL(_fromUtf8("currentRowChanged(int)")), Dialog.populateFields)
        QtCore.QObject.connect(self.btn_editRow, QtCore.SIGNAL(_fromUtf8("clicked()")), Dialog.editRow)
        QtCore.QObject.connect(self.btn_deleteRow, QtCore.SIGNAL(_fromUtf8("clicked()")), Dialog.deleteRow)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(QtGui.QApplication.translate("Dialog", "Edit Connection", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Dialog", "Host", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("Dialog", "Database", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Dialog", "Table", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("Dialog", "Port", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("Dialog", "Username", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("Dialog", "Password", None, QtGui.QApplication.UnicodeUTF8))
        self.btn_deleteRow.setText(QtGui.QApplication.translate("Dialog", "Delete", None, QtGui.QApplication.UnicodeUTF8))
        self.btn_editRow.setText(QtGui.QApplication.translate("Dialog", "Edit", None, QtGui.QApplication.UnicodeUTF8))

import resource_rc
