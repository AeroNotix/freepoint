# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'sql_setup.ui'
#
# Created: Sat Mar  3 15:18:31 2012
#      by: PyQt4 UI code generator 4.9.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_frm_sql_data_entry(object):
    def setupUi(self, frm_sql_data_entry):
        frm_sql_data_entry.setObjectName(_fromUtf8("frm_sql_data_entry"))
        frm_sql_data_entry.resize(407, 179)
        self.gridLayout = QtGui.QGridLayout(frm_sql_data_entry)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.txt_sql_entry = QtGui.QTextEdit(frm_sql_data_entry)
        self.txt_sql_entry.setObjectName(_fromUtf8("txt_sql_entry"))
        self.gridLayout.addWidget(self.txt_sql_entry, 0, 2, 5, 1)
        self.txt_sql_db = QtGui.QLineEdit(frm_sql_data_entry)
        self.txt_sql_db.setObjectName(_fromUtf8("txt_sql_db"))
        self.gridLayout.addWidget(self.txt_sql_db, 0, 1, 1, 1)
        self.lbl_db_name_field = QtGui.QLabel(frm_sql_data_entry)
        self.lbl_db_name_field.setObjectName(_fromUtf8("lbl_db_name_field"))
        self.gridLayout.addWidget(self.lbl_db_name_field, 0, 0, 1, 1)
        self.lbl_username_field = QtGui.QLabel(frm_sql_data_entry)
        self.lbl_username_field.setObjectName(_fromUtf8("lbl_username_field"))
        self.gridLayout.addWidget(self.lbl_username_field, 1, 0, 1, 1)
        self.txt_username = QtGui.QLineEdit(frm_sql_data_entry)
        self.txt_username.setObjectName(_fromUtf8("txt_username"))
        self.gridLayout.addWidget(self.txt_username, 1, 1, 1, 1)
        self.lbl_password_field = QtGui.QLabel(frm_sql_data_entry)
        self.lbl_password_field.setObjectName(_fromUtf8("lbl_password_field"))
        self.gridLayout.addWidget(self.lbl_password_field, 2, 0, 1, 1)
        self.txt_password = QtGui.QLineEdit(frm_sql_data_entry)
        self.txt_password.setObjectName(_fromUtf8("txt_password"))
        self.gridLayout.addWidget(self.txt_password, 2, 1, 1, 1)
        self.label_4 = QtGui.QLabel(frm_sql_data_entry)
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.gridLayout.addWidget(self.label_4, 3, 0, 1, 1)
        self.txt_table = QtGui.QLineEdit(frm_sql_data_entry)
        self.txt_table.setObjectName(_fromUtf8("txt_table"))
        self.gridLayout.addWidget(self.txt_table, 3, 1, 1, 1)
        self.buttonBox = QtGui.QDialogButtonBox(frm_sql_data_entry)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.gridLayout.addWidget(self.buttonBox, 4, 0, 1, 2)
        self.lbl_db_name_field.setBuddy(self.txt_sql_db)
        self.lbl_username_field.setBuddy(self.txt_username)
        self.lbl_password_field.setBuddy(self.txt_password)
        self.label_4.setBuddy(self.txt_table)

        self.retranslateUi(frm_sql_data_entry)
        QtCore.QMetaObject.connectSlotsByName(frm_sql_data_entry)

    def retranslateUi(self, frm_sql_data_entry):
        frm_sql_data_entry.setWindowTitle(QtGui.QApplication.translate("frm_sql_data_entry", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.lbl_db_name_field.setText(QtGui.QApplication.translate("frm_sql_data_entry", "MySQL Database", None, QtGui.QApplication.UnicodeUTF8))
        self.lbl_username_field.setText(QtGui.QApplication.translate("frm_sql_data_entry", "Username", None, QtGui.QApplication.UnicodeUTF8))
        self.lbl_password_field.setText(QtGui.QApplication.translate("frm_sql_data_entry", "Password", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("frm_sql_data_entry", "Table", None, QtGui.QApplication.UnicodeUTF8))

