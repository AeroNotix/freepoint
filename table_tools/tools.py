"""
Set of tools that will come in handy when dealing with SQL
databases and Qt applications
"""

import contextlib
import MySQLdb

def get_headings(db, table):
    '''
    get the headings from a table in the specified database

    Takes a reference to a database, and a string for which
    table you want to query, returns the headers.

    MySQL database syntax.
    '''

    cursor = db.cursor()
    cursor.execute('''SHOW COLUMNS FROM {}'''.format(table))
    
    headings = cursor.fetchall()
    ioheadings = []
    [ioheadings.append(item[0]) for item in headings]
    return ioheadings

@contextlib.contextmanager
def sql_query(db, query):
    cursor = db.cursor()
    cursor.execute(query)
    yield cursor.fetchall()
    
    
