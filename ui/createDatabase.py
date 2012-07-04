import simplejson
import urllib2, urllib

from PyQt4 import QtGui
from qtsqlviewer.ui.create_new_table_UI import Ui_Dialog


class CreateNewTable(QtGui.QDialog):
    def __init__(self, parent=None):
        super(CreateNewTable, self).__init__(parent)
        self.gui = Ui_Dialog()
        self.gui.setupUi(self)
        self.gui.tabWidget.tabBar().setVisible(False)
        self.change_map = {
            "txt_group": self.add_text_row,
            "choice_group": self.add_choice_row,
            "date_group": self.add_date_row,
            "time_group": self.add_time_row
            }
        self.json_data = {
            "HEADINGS": {}
            }
        self.row_num = -1

    def add_text_row(self):
        """
        This method reads the tab widget associated with the text row option
        widgets and will create a new entry in the json dictionary ready for
        when we serialize the json.
        """
        rowname = str(self.gui.txt_grp_rowname.text())
        if not len(rowname):
            return
        if not self.check_row(rowname):
            return

        self.json_data["HEADINGS"][rowname] = {
            "ROWNUM": self.get_row_num(),
            "ROWDATA": {
                "TYPE": "VARCHAR",
                "LEN" : str(self.gui.txt_grp_length.value()),
                "UNIQUE": self.gui.txt_grp_unique.isChecked(),
                }
            }

        print self.json_data

    def add_choice_row(self):
        rowname = str(self.gui.choice_grp_rowname.text())
        if not len(rowname):
            return
        if not self.check_row(rowname):
            return
        choices =  map(str, list(self.gui.choice_grp_choices.toPlainText().split("\n")))
        self.json_data["HEADINGS"][rowname] = {
            "ROWNUM": self.get_row_num(),
            "ROWDATA": {
                "TYPE": "CHOICE",
                "UNIQUE": self.gui.txt_grp_unique.isChecked(),
                "CHOICES": choices[:],
                }
            }
        print self.json_data

    def add_date_row(self):
        rowname = str(self.gui.date_grp_rowname.text())
        if not len(rowname):
            return
        if not self.check_row(rowname):
            return
        self.json_data["HEADINGS"][rowname] = {
            "ROWNUM": self.get_row_num(),
            "ROWDATA": {
                "TYPE": "DATE",
                "UNIQUE": self.gui.date_grp_unique.isChecked(),
                }
            }
        print self.json_data

    def add_time_row(self):
        rowname = str(self.gui.time_grp_rowname.text())
        if not len(rowname):
            return
        if not self.check_row(rowname):
            return

        self.json_data["HEADINGS"][rowname] = {
            "ROWNUM": self.get_row_num(),
            "ROWDATA": {
                "TYPE": "TIME",
                "UNIQUE": self.gui.time_grp_unique.isChecked()
                }
            }
        self.json_data

    def changeFieldDescriptions(self, index):
        """
        This method receives the index that the field type box has
        changed to. It returns the unary version of the index, for
        some strange reason so we again unarize this integer and
        set the tab to that number.
        """
        self.gui.tabWidget.setCurrentIndex(~index-1)

    def acceptFieldAdd(self):
        """
        Each tab has it's own Add>> button which is linked to this signal.

        We find which widget called this function via the self.sender()
        function, then find it's parent and then that objects name. We
        then find the associated function in the function map and return
        that function.
        """
        return self.change_map[ str(self.sender().parent().objectName()) ]()

    def check_row(self, rowname):
        """
        Performs all checks required to see if a given row is existant already
        and if so, prompts the user if we should overwrite it.
        """
        if self.row_exists(rowname):
            return self.check_overwrite(rowname)
        return True
    
    def row_exists(self, rowname):
        """
        Checks to see if a row exists in the HEADINGS map.
        """
        if self.json_data["HEADINGS"].get(rowname):
            return True
        return False

    def check_overwrite(self, rowname):
        """
        Given a rowname, we ask the user if they would like to overwrite this
        row. This only asks the user if they would like to overwrite the entry
        it does not actually perform the write, we simply return whether the
        user would like to overwrite the row.
        """
        msgbox = QtGui.QMessageBox()
        msgbox.setText("Row Exists: %s. Overwrite?" % rowname)
        msgbox.addButton(QtGui.QMessageBox.Yes)
        msgbox.addButton(QtGui.QMessageBox.No)
        return msgbox.exec_() == QtGui.QMessageBox.Yes

    def accept(self):
        #print self.json_data
        self.row_num = 0

        payload = {"payload": simplejson.dumps(self.json_data)}
        print payload
        req = urllib2.Request(
            "http://localhost:12345/create/",
            urllib.urlencode(payload)
            )

        f = urllib2.urlopen(req)
        f.close()

    def get_row_num(self):
        self.row_num += 1
        return self.row_num
