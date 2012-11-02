import simplejson
import urllib2, urllib

from PyQt4 import QtGui
from freepoint.ui.create_new_table_UI import Ui_Dialog
from freepoint.settings import SERVERURL

class CreateNewTable(QtGui.QDialog):
    """
    CreateNewTable allows users to create a new database table.

    The member change_map holds a mapping of keys to functions. This is
    because the GUI design presented a unique problem of having a radio
    widget determine what other widgets were presented on screen. This
    was solved by using a tab widget and hiding the tab selectors this
    means that the way the widgets are hidden/shown is by selecting the
    correct tab programmatically.

    Each tab should contain at least:

    a QtGui.QLineEdit : This should have an object name of "XXX_grp_rowname"
                        where XXX is the identifier for that particular row.

    a QtGui.QButton   : This should have an object name of "btn_XXX_group"
                        where XXX is the identifier of that row (the same,
                        as the QLineEdit).

    And be inside a QtGui.QGroupBox with an object name of "XXX_group"
    this is the identifer which you should use as a key for the change_map.

    All Add>> Buttons should have their clicked() signal assigned to the
    acceptFieldAdd button. This uses the sender() method to determine which
    to find the group which sent the press.
    """
    def __init__(self, parent=None):
        super(CreateNewTable, self).__init__(parent)
        self.gui = Ui_Dialog()
        self.gui.setupUi(self)
        self.gui.tabWidget.tabBar().setVisible(False)
        self.change_map = {
            "txt_group": self.add_text_row,
            "choice_group": self.add_choice_row,
            "date_group": self.add_date_row,
            "time_group": self.add_time_row,
            "curr_group": self.add_curr_row,
            }

        self.json_data = {
            "HEADINGS": {},
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

        self.add_generic_data("txt_grp")
        self.json_data["HEADINGS"][rowname]["ROWDATA"].update({
                "TYPE": "VARCHAR",
                "LEN" : int(self.gui.txt_grp_length.value())
                })

        self.gui.txt_grp_length.setValue(255)
        self.generic_cleanup("txt_grp")

    def add_choice_row(self):
        rowname = str(self.gui.choice_grp_rowname.text())
        if not len(rowname):
            return
        if not self.check_row(rowname):
            return

        choices =  map(
            str, list(self.gui.choice_grp_choices.toPlainText().split("\n"))
        )

        self.add_generic_data("choice_grp")
        self.json_data["HEADINGS"][rowname]["ROWDATA"].update({
                "TYPE": "CHOICE",
                "CHOICES": choices[:]
                })
        self.generic_cleanup("choice_grp")
        self.gui.choice_grp_choices.clear()

    def add_date_row(self):
        rowname = str(self.gui.date_grp_rowname.text())
        if not len(rowname):
            return
        if not self.check_row(rowname):
            return

        self.add_generic_data("date_grp")
        self.json_data["HEADINGS"][rowname]["ROWDATA"].update({
                "TYPE": "DATE",
                })
        self.generic_cleanup("date_grp")

    def add_curr_row(self):
        rowname = str(self.gui.curr_grp_rowname.text())
        if not len(rowname):
            return
        if not self.check_row(rowname):
            return

        self.add_generic_data("curr_grp")
        self.json_data["HEADINGS"][rowname]["ROWDATA"].update({
                "TYPE": "CURR",
                })
        self.generic_cleanup("curr_grp")

    def add_time_row(self):
        rowname = str(self.gui.time_grp_rowname.text())
        if not len(rowname):
            return
        if not self.check_row(rowname):
            return

        self.add_generic_data("time_grp")
        self.json_data["HEADINGS"][rowname]["ROWDATA"].update({
                "TYPE": "TIME",
                })
        self.generic_cleanup("time_grp")

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
        ans = msgbox.exec_() == QtGui.QMessageBox.Yes
        if ans:
            self.row_num -= 1
            return True
        return False

    def accept(self):
        """
        Accept finalizes our database and sends a request to the server.

        Through adding table rows using the radio boxes and various
        widgets for customizing rows we have all the data the server
        needs to create our table. The server directly converts the
        JSON encoded fieldtype data into SQL statements which are
        executed.

        With the exception of the database name and the
        table name, everything is quoted to remove the possibility of
        SQL injection attacks.

        On the server, we simply marshal the incoming JSON data into the
        required datastructure and store the raw JSON as table metadata.

        The table metadata is send to the client on the request to show
        the table, it is then marshaled again into a datastructure (Python
        dictionary) and then iterated through to create the GUI.

        There is no error handling here because simply if the server
        has an accountable for error, they will send it in the JSON
        fields, otherwise we don't want to catch the error and we will
        crash spectacularly.
        """
        self.row_num = 0

        payload = simplejson.dumps({
                "DATABASE": str(self.gui.txt_database_name.text()),
                "TABLE": str(self.gui.txt_table_name.text()),
                "HEADINGS": self.json_data["HEADINGS"],
                "PAYLOAD": simplejson.dumps(self.json_data)
                })
        req = urllib2.Request(
            SERVERURL + "create/",
            payload
            )
        json = simplejson.loads(urllib2.urlopen(req).read())
        super(CreateNewTable, self).accept()

    @property
    def get_row_num(self):
        """
        Increments the row number and returns it.
        """
        self.row_num += 1
        return self.row_num

    def add_row_to_list(self, row):
        """
        We create a row string which is just the name of the row for now.

        Eventually this row will be a real QListWidgetItem which will hold
        arbitrary data so we can click an item in the QListWidget and have
        that repopulate the data so that quick edits can be made when
        creating a database.
        """
        self.gui.list_db_rows.addItem(QtGui.QListWidgetItem(row))

    def add_generic_data(self, tab_name):
        """
        add_generic_data factors out generic code which is used to add data
        to the json_data map. As each column has a ROWNUM, UNIQUE and NULL
        value, then this can be determined in the same way we factored it out
        to be done here.
        """
        rowname = str(getattr(self.gui, "%s_rowname" % tab_name).text())

        self.json_data["HEADINGS"][rowname] = {
            "ROWNUM": self.get_row_num,
            "ROWDATA": {
                "UNIQUE": getattr(self.gui, "%s_unique" % tab_name).isChecked(),
                "NULL": getattr(self.gui, "%s_isnull" % tab_name).isChecked()
                }
            }

    def generic_cleanup(self, tab_name):
        """
        generic cleanup factors our generic code which is used to clean up
        the tab which the new column's data is added to. Each column has a
        name, a unique check and an is null check. We reset each of these
        and add the row to the list of rows.
        """

        rowname = str(getattr(self.gui, "%s_rowname" % tab_name).text())
        self.add_row_to_list(rowname)

        getattr(self.gui, "%s_rowname" % tab_name).setText("")
        getattr(self.gui, "%s_unique" % tab_name).setCheckState(False)
        getattr(self.gui, "%s_isnull" % tab_name).setCheckState(False)
