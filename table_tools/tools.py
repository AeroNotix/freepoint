"""
Set of tools that will come in handy when dealing with SQL
databases and Qt applications
"""

import functools
import urllib2

import simplejson
from simplejson.decoder import JSONDecodeError
from PyQt4 import QtCore, QtGui


def create_action(obj,
                  text, fname=None,  slot=None, shortcut=None,
                  tip=None, signal="triggered()"):
    """
    Helper method to add in buttons to a toolbar
    """

    action = QtGui.QAction(text, obj)

    if fname is not None:
        action.setIcon(QtGui.QIcon(fname))
    if shortcut is not None:
        action.setShortcut(shortcut)
    if tip is not None:
        action.setToolTip(tip)
        action.setStatusTip(tip)
    if slot is not None:
        obj.connect(action, QtCore.SIGNAL(signal), slot)
    obj.connect(action, QtCore.SIGNAL("hovered()"),
                lambda text=text: obj.show_message(text))
    obj.toolbar.addAction(action)
    return action

def to_unicode(string, encoding='utf-8'):
    """
    Returns a string in the specified encoding,
    default is utf-8

    :param string: :class:`string` which needs to be converted
    :param encoding: :class:`string` which is a encoding type.
    """
    return unicode(string).encode(encoding)

def table_wrapper(func):
    """
    Wrapper function to enable or disable signals.

    Disables:
        cellChanged(int, int)
        cellDoubleClicked(int, int)
        cellEntered(int, int)

    During table population we insert rows from the database
    programmatically. This causes Qt to fire off signals
    associated with the functions we are calling. This in
    turn causes the callbacks assigned to those signals to
    be fired. This causes a problem when those functions
    have side-effects such as writing to a database.
    """

    @functools.wraps(func)
    def inner(obj, *args, **kwargs):
        """
        Outline of pre/post code

        * Enabled/Disables sorting
        * Disconnects/Re-connects:
              cellChanged(int, int)
              cellDoubleClicked(int, int)
              cellEntered(int, int)
        """

        # Disable sorting/signals
        obj.gui.tableWidget.setSortingEnabled(False)
        obj.disconnect(obj.gui.tableWidget,
                       QtCore.SIGNAL("cellChanged(int, int)"),
                       obj.changeTable)
        obj.disconnect(obj.gui.tableWidget,
                       QtCore.SIGNAL("cellDoubleClicked(int, int)"),
                       obj.storeCell)
        obj.disconnect(obj.gui.tableWidget,
                       QtCore.SIGNAL("cellEntered(int, int)"),
                       obj.storeCell)

        # Call the original function
        func(obj, *args, **kwargs)

        # Enable sorting/signals
        obj.gui.tableWidget.setSortingEnabled(True)
        obj.connect(obj.gui.tableWidget,
                    QtCore.SIGNAL("cellChanged(int, int)"),
                    obj.changeTable)
        obj.connect(obj.gui.tableWidget,
                    QtCore.SIGNAL("cellDoubleClicked(int, int)"),
                    obj.storeCell)
        obj.connect(obj.gui.tableWidget,
                    QtCore.SIGNAL("cellEntered(int, int)"),
                    obj.storeCell)


    return inner


class Database(object):
    """
    This class encapsulates all the interactions between the client code
    and the database.

    This allows us to have multiple instances of databases on each client
    and also bind together commonly used functionality.

    We also place per-instance variables such as connection details and
    recent queries. This allows a much finer API to emerge because we
    can keep track of a single database instance rather than having to
    know what attributes and methods are to do with a database.
    """

    def __init__(self, parent=None,
                 user=None, passwd=None,
                 using_db=None, table=None):
        """
        Creates an instance of a Database which holds connection details
        and commonly used methods on a database.

        :returns: :class:`Database`
        """

        self.user = user
        self.password = passwd
        self.using_db = using_db
        self.table = table
        self._connection = None
        self.connected = False
        self.base_url = "http://localhost:12345/"
        self.param_url = self.base_url + "getdb/"
        self.login_url = self.base_url + "login/"
        self.update_url = self.base_url + "update/"
        self.insert_url = self.base_url + "insert/"

        self.parent = parent
        if not self.parent:
            self.error = lambda s:s
        else:
            self.error = self.parent.show_error

    def connect(self):
        """
        connect sends the USER/PASSWORD combination up to the server to
        be authenticated by that. The server will return some JSON which
        indicated success or failure. We parse out the response and use
        the raw boolean value as the return value.

        :returns: :class:`Bool` showing whether the connect succeeded
        """

        try:
            json_payload = simplejson.dumps({
                'USER': unicode(self.user),
                'PASSWORD': unicode(self.password),
            })
            http_post = urllib2.Request(
                self.login_url,
                json_payload
            )
            json = simplejson.loads(urllib2.urlopen(http_post).read())
        except IOError:
            self.error("Cannot connect to dataserver.")
            return False
        except JSONDecodeError:
            self.error("The information from the server was invald.")
            return False

        self.connected = json['Success']
        return self.connected

    def close(self):
        """
        Closes the database connection

        This method mutates the self.connected attribute to show other callers
        that we have closed the connection. It's a simple attribute because we
        don't hold open connections to the server but instead communicate via
        the REST-ish API. So setting this value is more of a logical thing
        rather than a concrete close of the connection.
        """
        self.connected = False

    def query(self):
        """
        Queries the database for a select *.

        Will mutate the self.headings with the headings for the database and
        return a list.

        This method will communicate with the parent object via the show_error
        method attached to it if the parent is available.

        :returns: A list of the data returned.
        """

        try:
            json_payload = simplejson.dumps({
                'User': unicode(self.user),
                'Password': unicode(self.password),
                "Database": unicode(self.using_db),
                "Table": unicode(self.table)
            })
            http_post = urllib2.Request(
                self.param_url,
                json_payload
            )
            json = simplejson.loads(urllib2.urlopen(http_post).read())
        except IOError:
            self.error("Cannot connect to dataserver.")
            return []
        except JSONDecodeError:
            self.error("The information from the server was invalid.")
            return []

        # Get the headings from the returned JSON and send an error message
        # to the user if we found nothing.
        self.headings = json.get("Headings")
        if not self.headings:
            self.error("The database did not return the correct data.")
            return []

        self.metadata = json.get("Metadata", False)
        rows = json.get("Rows", [])
        if not rows:
            return []
        return rows

    def get_headings(self):
        """
        We use a get method for this as the API isn't stable and having to
        change the call sites is a PITA so we just leave the call site with a
        method to get and we can change the implementation as we see fit.
        """
        return self.headings

    def changeTable(self, xrow, ycol):
        """
        When a cell is edited the data is written back into the database.

        :param xrow: :class:`Int` which is passed directly from the signal
        :param ycol: :class:`Int` which is passed directly from the signal
        :returns: None
        """

        json_payload = simplejson.dumps({
            "DATABASE": unicode(self.using_db),
            "TABLE": unicode(self.table),
            "COLUMN": unicode(self.headings[ycol]),
            "DATA": unicode(
                    self.parent.gui.tableWidget.item(xrow, ycol).text()
            ),
            "ID": unicode(self.parent.gui.tableWidget.item(xrow, 0).text())
        })

        http_post = urllib2.Request(
            # string interpolation
            self.update_url,
            json_payload
        )

        json = simplejson.loads(urllib2.urlopen(http_post).read())
        if json.get('Success') == False:
            self.parent.gui.tableWidget.blockSignals(True)
            self.parent.revertCellData(xrow, ycol)
            self.parent.gui.tableWidget.blockSignals(False)
            self.error("Data could not be saved to the database.")
        else:
            self.parent.show_message("Data has been saved to the database.")

    def insertData(self, json):
        """
        Inserts a new row into the table
        """

        json_payload = simplejson.dumps({
                "DATA":json,
                "TABLE": self.table,
                "DATABASE": self.using_db
        })
        http_post = urllib2.Request(
            # string interpolation
            self.insert_url,
            json_payload
            )

        json = simplejson.loads(urllib2.urlopen(http_post).read())
        if json.get("Success"):
            self.parent.show_message("Data has been saved to the database.")
        else:
            self.parent.show_error("Data could not be saved to the database")
        self.parent.populate_table()
