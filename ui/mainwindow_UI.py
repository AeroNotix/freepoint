# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'mainwindow.ui'
#
# Created: Wed Jun 20 13:29:23 2012
#      by: PyQt4 UI code generator 4.9.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.resize(800, 600)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.gridLayout = QtGui.QGridLayout(self.centralwidget)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.tableWidget = QtGui.QTableWidget(self.centralwidget)
        self.tableWidget.setColumnCount(3)
        self.tableWidget.setObjectName(_fromUtf8("tableWidget"))
        self.tableWidget.setRowCount(0)
        self.gridLayout.addWidget(self.tableWidget, 0, 0, 1, 1)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 800, 21))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        self.menuFile = QtGui.QMenu(self.menubar)
        self.menuFile.setObjectName(_fromUtf8("menuFile"))
        self.menuEdit = QtGui.QMenu(self.menubar)
        self.menuEdit.setObjectName(_fromUtf8("menuEdit"))
        self.menuSelect_Table = QtGui.QMenu(self.menuEdit)
        self.menuSelect_Table.setObjectName(_fromUtf8("menuSelect_Table"))
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName(_fromUtf8("statusbar"))
        MainWindow.setStatusBar(self.statusbar)
        self.actionQuit = QtGui.QAction(MainWindow)
        self.actionQuit.setObjectName(_fromUtf8("actionQuit"))
        self.actionConnection_details = QtGui.QAction(MainWindow)
        self.actionConnection_details.setObjectName(_fromUtf8("actionConnection_details"))
        self.actionManage_Connections = QtGui.QAction(MainWindow)
        self.actionManage_Connections.setObjectName(_fromUtf8("actionManage_Connections"))
        self.actionSomething = QtGui.QAction(MainWindow)
        self.actionSomething.setObjectName(_fromUtf8("actionSomething"))
        self.actionManageConnections = QtGui.QAction(MainWindow)
        self.actionManageConnections.setObjectName(_fromUtf8("actionManageConnections"))
        self.menuFile.addSeparator()
        self.menuFile.addAction(self.actionQuit)
        self.menuFile.addAction(self.actionConnection_details)
        self.menuEdit.addAction(self.menuSelect_Table.menuAction())
        self.menuEdit.addAction(self.actionManageConnections)
        self.menubar.addAction(self.menuFile.menuAction())
        self.menubar.addAction(self.menuEdit.menuAction())

        self.retranslateUi(MainWindow)
        QtCore.QObject.connect(self.tableWidget, QtCore.SIGNAL(_fromUtf8("cellChanged(int,int)")), MainWindow.changeTable)
        QtCore.QObject.connect(self.actionConnection_details, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.openConnectionDialog)
        QtCore.QObject.connect(self.actionQuit, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.close)
        QtCore.QObject.connect(self.tableWidget, QtCore.SIGNAL(_fromUtf8("cellDoubleClicked(int,int)")), MainWindow.storeCell)
        QtCore.QObject.connect(self.tableWidget, QtCore.SIGNAL(_fromUtf8("cellEntered(int,int)")), MainWindow.storeCell)
        QtCore.QObject.connect(self.actionManageConnections, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.openManageDialog)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QtGui.QApplication.translate("MainWindow", "MainWindow", None, QtGui.QApplication.UnicodeUTF8))
        self.menuFile.setTitle(QtGui.QApplication.translate("MainWindow", "File", None, QtGui.QApplication.UnicodeUTF8))
        self.menuEdit.setTitle(QtGui.QApplication.translate("MainWindow", "Edit", None, QtGui.QApplication.UnicodeUTF8))
        self.menuSelect_Table.setTitle(QtGui.QApplication.translate("MainWindow", "Select Table", None, QtGui.QApplication.UnicodeUTF8))
        self.actionQuit.setText(QtGui.QApplication.translate("MainWindow", "Quit.", None, QtGui.QApplication.UnicodeUTF8))
        self.actionConnection_details.setText(QtGui.QApplication.translate("MainWindow", "Add Connection", None, QtGui.QApplication.UnicodeUTF8))
        self.actionManage_Connections.setText(QtGui.QApplication.translate("MainWindow", "Manage Connections", None, QtGui.QApplication.UnicodeUTF8))
        self.actionSomething.setText(QtGui.QApplication.translate("MainWindow", "Something", None, QtGui.QApplication.UnicodeUTF8))
        self.actionManageConnections.setText(QtGui.QApplication.translate("MainWindow", "Manage Connections", None, QtGui.QApplication.UnicodeUTF8))

