"""
Set of tools that will come in handy when dealing with SQL
databases and Qt applications
"""

import contextlib
import functools
import urllib2, urllib

import MySQLdb
import simplejson
from simplejson.decoder import JSONDecodeError
from PyQt4 import QtCore

def get_headings(db, query):
    '''
    get the headings from a table in the specified database

    Takes a reference to a database, and a string for which
    table you want to query, returns the headers.

    MySQL database syntax.

    :param db: A :class:`MySQLdb.connect`.
    :param query: A :class:`string` which contains the query to be executed
    '''

    cursor = db.cursor()
    cursor.execute('''CREATE TEMPORARY TABLE temp (%s)''' % query)
    cursor.execute('''SHOW COLUMNS FROM temp''')
    headings = cursor.fetchall()
    ioheadings = []
    [ioheadings.append(item[0]) for item in headings]
    cursor.execute('''DROP TABLE temp''')
    return ioheadings


@contextlib.contextmanager
def sql_query(db, query):
    '''
    Ability to use a set of sql results
    within a with statement.

    :param db: A :class:`MySQLdb.connect`.
    :param query: A :class:`string` which contains the query to be executed
    '''

    cursor = db.cursor()
    cursor.execute(query)
    yield cursor.fetchall()


def itersql(db, query):
    '''
    Yields a row of data from the query

    Used to iterate over query rather than querying
    then assigning, then looping over that data.

    :param db: A :class:`MySQLdb.connect`.
    :param query: A :class:`string` which contains the query to be executed
    :returns: Yields a row from the database.
    '''

    cursor = db.cursor()
    cursor.execute(query)

    for row in cursor.fetchall():
        yield row

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
        except IOError as error:
            self.parent.show_error("Cannot connect to dataserver.")
        except JSONDecodeError as error:
            self.parent.show_error("The information from the server was invald.")
        self._connection = MySQLdb.connect(
            host=self.host, user=self.user,
            passwd=self.password, db=self.using_db,
            port=int(self.port), charset="utf8",
            use_unicode=True
        )
        self.connected = True

    def close(self):
        '''
        Closes the database connection
        '''
        try:
            self._connection.close()
            self.connected = False
        except:
            pass

    def cursor(self):
        return self._connection.cursor()

    def query(self, query):
        '''
        Returns a QuerySet for the passed in query

        :param query: :class:`str` which is the query to be executed
        '''
        try:
            json_payload = {
                'User': self.user,
                'Password': self.password,
                "Database": "%s.%s" % (self.using_db, self.table),
            }
            http_post = urllib2.Request(
                # string interpolation
                self.param_url % (self.using_db, self.table),
                urllib.urlencode(json_payload)
            )
            json = simplejson.loads(urllib2.urlopen(http_post).read())
        except IOError as error:
            self.parent.show_error("Cannot connect to dataserver.")
        except JSONDecodeError as error:
            self.parent.show_error("The information from the server was invalid.")
        return [result for result in itersql(self._connection, query)]

    def commit(self):
        self._connection.commit()

    def __str__(self):
        return """Database Connection on: %s using %s
               Connected: %s
               Current Table: %s
               """ % (self.host,self.using_db,self.conncted,self.table)
