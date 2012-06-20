# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'sql_setup.ui'
#
# Created: Wed Jun 20 16:10:41 2012
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
        frm_sql_data_entry.resize(486, 274)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(_fromUtf8(":/view-refresh")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        frm_sql_data_entry.setWindowIcon(icon)
        self.gridLayout = QtGui.QGridLayout(frm_sql_data_entry)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.label_2 = QtGui.QLabel(frm_sql_data_entry)
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.gridLayout.addWidget(self.label_2, 1, 0, 1, 1)
        self.lbl_db_name_field = QtGui.QLabel(frm_sql_data_entry)
        self.lbl_db_name_field.setObjectName(_fromUtf8("lbl_db_name_field"))
        self.gridLayout.addWidget(self.lbl_db_name_field, 0, 0, 1, 1)
        self.txt_table = QtGui.QLineEdit(frm_sql_data_entry)
        self.txt_table.setObjectName(_fromUtf8("txt_table"))
        self.gridLayout.addWidget(self.txt_table, 5, 1, 1, 1)
        self.lbl_username_field = QtGui.QLabel(frm_sql_data_entry)
        self.lbl_username_field.setObjectName(_fromUtf8("lbl_username_field"))
        self.gridLayout.addWidget(self.lbl_username_field, 2, 0, 1, 1)
        self.label = QtGui.QLabel(frm_sql_data_entry)
        self.label.setObjectName(_fromUtf8("label"))
        self.gridLayout.addWidget(self.label, 5, 0, 1, 1)
        self.buttonBox = QtGui.QDialogButtonBox(frm_sql_data_entry)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.gridLayout.addWidget(self.buttonBox, 6, 0, 1, 2)
        self.txt_port = QtGui.QLineEdit(frm_sql_data_entry)
        self.txt_port.setObjectName(_fromUtf8("txt_port"))
        self.gridLayout.addWidget(self.txt_port, 1, 1, 1, 1)
        self.label_4 = QtGui.QLabel(frm_sql_data_entry)
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.gridLayout.addWidget(self.label_4, 4, 0, 1, 1)
        self.txt_password = QtGui.QLineEdit(frm_sql_data_entry)
        self.txt_password.setObjectName(_fromUtf8("txt_password"))
        self.gridLayout.addWidget(self.txt_password, 3, 1, 1, 1)
        self.txt_username = QtGui.QLineEdit(frm_sql_data_entry)
        self.txt_username.setObjectName(_fromUtf8("txt_username"))
        self.gridLayout.addWidget(self.txt_username, 2, 1, 1, 1)
        self.lbl_password_field = QtGui.QLabel(frm_sql_data_entry)
        self.lbl_password_field.setObjectName(_fromUtf8("lbl_password_field"))
        self.gridLayout.addWidget(self.lbl_password_field, 3, 0, 1, 1)
        self.txt_database = QtGui.QLineEdit(frm_sql_data_entry)
        self.txt_database.setObjectName(_fromUtf8("txt_database"))
        self.gridLayout.addWidget(self.txt_database, 4, 1, 1, 1)
        self.txt_host = QtGui.QLineEdit(frm_sql_data_entry)
        self.txt_host.setObjectName(_fromUtf8("txt_host"))
        self.gridLayout.addWidget(self.txt_host, 0, 1, 1, 1)
        self.label_3 = QtGui.QLabel(frm_sql_data_entry)
        self.label_3.setText(_fromUtf8(""))
        self.label_3.setPixmap(QtGui.QPixmap(_fromUtf8(":/mysql")))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.gridLayout.addWidget(self.label_3, 0, 2, 7, 1)
        self.lbl_db_name_field.setBuddy(self.txt_host)
        self.lbl_username_field.setBuddy(self.txt_username)
        self.label_4.setBuddy(self.txt_database)
        self.lbl_password_field.setBuddy(self.txt_password)

        self.retranslateUi(frm_sql_data_entry)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), frm_sql_data_entry.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), frm_sql_data_entry.reject)
        QtCore.QMetaObject.connectSlotsByName(frm_sql_data_entry)
        frm_sql_data_entry.setTabOrder(self.txt_host, self.txt_port)
        frm_sql_data_entry.setTabOrder(self.txt_port, self.txt_username)
        frm_sql_data_entry.setTabOrder(self.txt_username, self.txt_password)
        frm_sql_data_entry.setTabOrder(self.txt_password, self.txt_database)
        frm_sql_data_entry.setTabOrder(self.txt_database, self.txt_table)
        frm_sql_data_entry.setTabOrder(self.txt_table, self.buttonBox)

    def retranslateUi(self, frm_sql_data_entry):
        frm_sql_data_entry.setWindowTitle(QtGui.QApplication.translate("frm_sql_data_entry", "Add Connection", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("frm_sql_data_entry", "Port", None, QtGui.QApplication.UnicodeUTF8))
        self.lbl_db_name_field.setText(QtGui.QApplication.translate("frm_sql_data_entry", "MySQL Host", None, QtGui.QApplication.UnicodeUTF8))
        self.lbl_username_field.setText(QtGui.QApplication.translate("frm_sql_data_entry", "Username", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("frm_sql_data_entry", "Table", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("frm_sql_data_entry", "Database", None, QtGui.QApplication.UnicodeUTF8))
        self.lbl_password_field.setText(QtGui.QApplication.translate("frm_sql_data_entry", "Password", None, QtGui.QApplication.UnicodeUTF8))

import resource_rc
