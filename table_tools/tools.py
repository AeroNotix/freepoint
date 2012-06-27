"""
Set of tools that will come in handy when dealing with SQL
databases and Qt applications
"""

import functools
import urllib2, urllib

import simplejson
from simplejson.decoder import JSONDecodeError
from PyQt4 import QtCore


def to_unicode(string, encoding='utf-8'):
    '''
    Returns a string in the specified encoding,
    default is utf-8

    :param string: :class:`string` which needs to be converted
    :param encoding: :class:`string` which is a encoding type.
    '''
    return unicode(string).encode(encoding)

def table_wrapper(func):
    '''
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
    '''

    @functools.wraps(func)
    def inner(obj, *args, **kwargs):

        # Disable signals
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

        # Enable signals
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
    '''
    This class encapsulates all the interactions between the client code
    and the database.

    This allows us to have multiple instances of databases on each client
    and also bind together commonly used functionality.

    We also place per-instance variables such as connection details and
    recent queries. This allows a much finer API to emerge because we
    can keep track of a single database instance rather than having to
    know what attributes and methods are to do with a database.
    '''

    def __init__(self, parent=None,
                 host='localhost', user=None, passwd=None,
                 using_db=None, table=None, port=3306):
        '''
        Creates an instance of a Database which holds connection details
        and commonly used methods on a database.
        '''
        self.host = host
        self.user = user
        self.password = passwd
        self.using_db = using_db
        self.table = table
        if port == '':
            port = 3306
        self.port = port
        self._connection = None
        self.connected = False
        self.base_url = "http://localhost:12345/"
        self.param_url = self.base_url + "getdb/%s.%s"
        self.login_url = self.base_url + "login/"
        self.update_url = self.base_url + "update/"
        self.parent = parent

    def connect(self):
        '''
        Creates the database connection. This needs to be overloaded when
        moving to use the client/server model. Currently we're connecting
        directly to the MySQL database.

        We're experimentally checking in to the login server to check if
        the user details authenticate.
        '''

        try:
            json_payload = {
                'User': self.user,
                'Password': self.password,
            }
            http_post = urllib2.Request(
                self.login_url,
                urllib.urlencode(json_payload)
            )
            json = simplejson.loads(urllib2.urlopen(http_post).read())
            print json
        except IOError:
            self.parent.show_error("Cannot connect to dataserver.")
            return
        except JSONDecodeError:
            self.parent.show_error("The information from the server was invald.")
            return
        self.connected = True

    def close(self):
        '''
        Closes the database connection
        '''
        self.connected = False

    def query(self):
        """
        Queries the database for a select *.

        Will mutate the self.headings with the headings for the database and
        return a list.

        :returns: A list of the data returned.
        """
        try:
            json_payload = {
                'User': self.user,
                'Password': self.password,
                "Database": self.using_db,
                "Table": self.table
            }
            http_post = urllib2.Request(
                # string interpolation
                self.param_url % (self.using_db, self.table),
                urllib.urlencode(json_payload)
            )
            json = simplejson.loads(urllib2.urlopen(http_post).read())
        except IOError:
            self.parent.show_error("Cannot connect to dataserver.")
            return []
        except JSONDecodeError:
            self.parent.show_error(
                "The information from the server was invalid."
            )
            return []
        # Get the headings from the returned JSON and send an error message
        # to the user if we found nothing.
        self.headings = json.get("Headings")
        if not self.headings:
            self.parent.show_error(
                "The database did not return the correct data."
            )
            return []
        self.metadata = json.get("Metadata", False)
        rows = json.get("Rows", [])
        if not rows:
            self.parent.show_error("The server did not return any data.")
            return rows
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

        This is highly alpha code.

        :param xrow: :class:`Int` which is passed directly from the signal
        :param ycol: :class:`Int` which is passed directly from the signal
        :returns: None
        """

        json = {
            "Database": self.using_db,
            "Table": self.table,
            "Column": self.headings[ycol],
            "Data": self.parent.gui.tableWidget.item(xrow, ycol).text(),
            "ID": self.parent.gui.tableWidget.item(xrow, 0).text()
        }

        http_post = urllib2.Request(
            # string interpolation
            self.update_url,
            urllib.urlencode(json)
        )

        json = simplejson.loads(urllib2.urlopen(http_post).read())
        self.parent.show_message("Data has been saved to the database")
