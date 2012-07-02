# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'create_new_table.ui'
#
# Created: Mon Jul 02 16:57:16 2012
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
        Dialog.setWindowModality(QtCore.Qt.ApplicationModal)
        Dialog.resize(820, 599)
        Dialog.setMinimumSize(QtCore.QSize(820, 599))
        Dialog.setMaximumSize(QtCore.QSize(820, 599))
        Dialog.setWindowOpacity(1.0)
        self.buttonBox = QtGui.QDialogButtonBox(Dialog)
        self.buttonBox.setGeometry(QtCore.QRect(460, 540, 341, 32))
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.listView = QtGui.QListView(Dialog)
        self.listView.setGeometry(QtCore.QRect(540, 30, 256, 192))
        self.listView.setObjectName(_fromUtf8("listView"))
        self.groupBox = QtGui.QGroupBox(Dialog)
        self.groupBox.setGeometry(QtCore.QRect(10, 20, 114, 234))
        self.groupBox.setObjectName(_fromUtf8("groupBox"))
        self.verticalLayout = QtGui.QVBoxLayout(self.groupBox)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.radioButton = QtGui.QRadioButton(self.groupBox)
        self.radioButton.setObjectName(_fromUtf8("radioButton"))
        self.buttonGroup = QtGui.QButtonGroup(Dialog)
        self.buttonGroup.setObjectName(_fromUtf8("buttonGroup"))
        self.buttonGroup.addButton(self.radioButton)
        self.verticalLayout.addWidget(self.radioButton)
        self.radioButton_2 = QtGui.QRadioButton(self.groupBox)
        self.radioButton_2.setObjectName(_fromUtf8("radioButton_2"))
        self.buttonGroup.addButton(self.radioButton_2)
        self.verticalLayout.addWidget(self.radioButton_2)
        self.radioButton_3 = QtGui.QRadioButton(self.groupBox)
        self.radioButton_3.setObjectName(_fromUtf8("radioButton_3"))
        self.buttonGroup.addButton(self.radioButton_3)
        self.verticalLayout.addWidget(self.radioButton_3)
        self.radioButton_6 = QtGui.QRadioButton(self.groupBox)
        self.radioButton_6.setObjectName(_fromUtf8("radioButton_6"))
        self.buttonGroup.addButton(self.radioButton_6)
        self.verticalLayout.addWidget(self.radioButton_6)
        self.radioButton_4 = QtGui.QRadioButton(self.groupBox)
        self.radioButton_4.setObjectName(_fromUtf8("radioButton_4"))
        self.buttonGroup.addButton(self.radioButton_4)
        self.verticalLayout.addWidget(self.radioButton_4)
        self.radioButton_8 = QtGui.QRadioButton(self.groupBox)
        self.radioButton_8.setObjectName(_fromUtf8("radioButton_8"))
        self.buttonGroup.addButton(self.radioButton_8)
        self.verticalLayout.addWidget(self.radioButton_8)
        self.radioButton_5 = QtGui.QRadioButton(self.groupBox)
        self.radioButton_5.setObjectName(_fromUtf8("radioButton_5"))
        self.buttonGroup.addButton(self.radioButton_5)
        self.verticalLayout.addWidget(self.radioButton_5)
        self.radioButton_9 = QtGui.QRadioButton(self.groupBox)
        self.radioButton_9.setObjectName(_fromUtf8("radioButton_9"))
        self.buttonGroup.addButton(self.radioButton_9)
        self.verticalLayout.addWidget(self.radioButton_9)
        self.radioButton_10 = QtGui.QRadioButton(self.groupBox)
        self.radioButton_10.setObjectName(_fromUtf8("radioButton_10"))
        self.buttonGroup.addButton(self.radioButton_10)
        self.verticalLayout.addWidget(self.radioButton_10)
        self.groupBox_2 = QtGui.QGroupBox(Dialog)
        self.groupBox_2.setGeometry(QtCore.QRect(130, 20, 401, 50))
        self.groupBox_2.setObjectName(_fromUtf8("groupBox_2"))
        self.verticalLayout_2 = QtGui.QVBoxLayout(self.groupBox_2)
        self.verticalLayout_2.setObjectName(_fromUtf8("verticalLayout_2"))

        self.retranslateUi(Dialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), Dialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), Dialog.reject)
        QtCore.QObject.connect(self.buttonGroup, QtCore.SIGNAL(_fromUtf8("buttonClicked(int)")), Dialog.changeFieldDescriptions)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(QtGui.QApplication.translate("Dialog", "Dialog", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox.setTitle(QtGui.QApplication.translate("Dialog", "Field Types", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButton.setText(QtGui.QApplication.translate("Dialog", "Line of text", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButton_2.setText(QtGui.QApplication.translate("Dialog", "Comment", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButton_3.setText(QtGui.QApplication.translate("Dialog", "Choice", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButton_6.setText(QtGui.QApplication.translate("Dialog", "Currency", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButton_4.setText(QtGui.QApplication.translate("Dialog", "Number", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButton_8.setText(QtGui.QApplication.translate("Dialog", "Date", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButton_5.setText(QtGui.QApplication.translate("Dialog", "Time", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButton_9.setText(QtGui.QApplication.translate("Dialog", "Boolean choice", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButton_10.setText(QtGui.QApplication.translate("Dialog", "Person/Group", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_2.setTitle(QtGui.QApplication.translate("Dialog", "Field Description", None, QtGui.QApplication.UnicodeUTF8))

