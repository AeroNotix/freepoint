"""
Set of tools that will come in handy when dealing with SQL
databases and Qt applications
"""

import contextlib
import functools

import MySQLdb
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
    cursor.execute("%s" % query)

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
