# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'loginbox.ui'
#
# Created: Wed Jun 27 11:11:59 2012
#      by: PyQt4 UI code generator 4.9.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_frm_login(object):
    def setupUi(self, frm_login):
        frm_login.setObjectName(_fromUtf8("frm_login"))
        frm_login.resize(225, 155)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(_fromUtf8(":/system-lock-screen")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        frm_login.setWindowIcon(icon)
        self.gridLayout = QtGui.QGridLayout(frm_login)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.lbl_username = QtGui.QLabel(frm_login)
        self.lbl_username.setObjectName(_fromUtf8("lbl_username"))
        self.gridLayout.addWidget(self.lbl_username, 0, 0, 1, 1)
        self.txt_username = QtGui.QLineEdit(frm_login)
        self.txt_username.setMaxLength(255)
        self.txt_username.setObjectName(_fromUtf8("txt_username"))
        self.gridLayout.addWidget(self.txt_username, 0, 1, 1, 1)
        self.lbl_password = QtGui.QLabel(frm_login)
        self.lbl_password.setObjectName(_fromUtf8("lbl_password"))
        self.gridLayout.addWidget(self.lbl_password, 1, 0, 1, 1)
        self.txt_password = QtGui.QLineEdit(frm_login)
        self.txt_password.setMaxLength(64)
        self.txt_password.setEchoMode(QtGui.QLineEdit.Password)
        self.txt_password.setObjectName(_fromUtf8("txt_password"))
        self.gridLayout.addWidget(self.txt_password, 1, 1, 1, 1)
        self.buttonBox = QtGui.QDialogButtonBox(frm_login)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.gridLayout.addWidget(self.buttonBox, 2, 0, 1, 2)
        self.lbl_username.setBuddy(self.txt_username)
        self.lbl_password.setBuddy(self.txt_password)

        self.retranslateUi(frm_login)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), frm_login.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), frm_login.reject)
        QtCore.QMetaObject.connectSlotsByName(frm_login)

    def retranslateUi(self, frm_login):
        frm_login.setWindowTitle(QtGui.QApplication.translate("frm_login", "Login", None, QtGui.QApplication.UnicodeUTF8))
        self.lbl_username.setText(QtGui.QApplication.translate("frm_login", "Username:", None, QtGui.QApplication.UnicodeUTF8))
        self.lbl_password.setText(QtGui.QApplication.translate("frm_login", "Password:", None, QtGui.QApplication.UnicodeUTF8))

import resource_rc
