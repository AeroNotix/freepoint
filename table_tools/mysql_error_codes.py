"""
Module contains a map of MySQL error codes.

This module is defunct but kept as a reference.
"""

MYSQL_ERRORS = {
    1062: "Duplicate values in column",
    1146: """Table does not exist""",
    1366: """Invalid number for decimal column""",
    2000: """Unknown MySQL error""",
    2001: """Can't create UNIX socket""",
    2002: """Can't connect to local MySQL server through socket""",
    2003: """Can't connect to MySQL server""",
    2004: """Can't create TCP/IP socket""",
    2005: """Unknown MySQL server host""",
    2006: """MySQL server has gone away""",
    2007: """Protocol mismatch""",
    2008: """MySQL client ran out of memory""",
    2009: """Wrong host info""",
    2010: """Localhost via UNIX socket""",
    2011: """via TCP/IP""",
    2012: """Error in server handshake""",
    2013: """Lost connection to MySQL server during query""",
    2014: """Commands out of sync; you can't run this command now""",
    2015: """Named pipe:""",
    2016: """Can't wait for named pipe to host:pipe:""",
    2017: """Can't open named pipe to host:pipe:""",
    2018: """Can't set state of named pipe to host:pipe:""",
    2019: """Can't initialize character set(path: )""",
    2020: """Got packet bigger than 'max_allowed_packet' bytes""",
    2021: """Embedded server""",
    2022: """Error on SHOW SLAVE STATUS:""",
    2023: """Error on SHOW SLAVE HOSTS:""",
    2024: """Error connecting to slave:""",
    2025: """Error connecting to master:""",
    2026: """SSL connection error:""",
    2027: """Malformed packet""",
    2028: """This client library
 is licensed only for use with MySQL servers having '' license""",
    2029: """Invalid use of null pointer""",
    2030: """Statement not prepared""",
    2031: """No data supplied for parameters in prepared statement""",
    2032: """Data truncated""",
    2033: """No parameters exist in the statement""",
    2034: """Invalid parameter number""",
    2035: """Can't send long data for
non-string/non-binary data types (parameter: )""",
    2036: """Using unsupported buffer type""",
    2037: """Shared memory:""",
    2038: """Can't open shared memory;
 client could not create request event""",
    2039: """Can't open shared memory;
 no answer event received from server""",
    2040: """Can't open shared memory;
 server could not allocate file mapping""",
    2041: """Can't open shared memory;
 server could not get pointer to file mapping""",
    2042: """Can't open shared memory;
 client could not allocate file mapping""",
    2043: """Can't open shared memory;
 client could not get pointer to file mapping""",
    2044: """Can't open shared memory;
 client could not createevent""",
    2045: """Can't open shared memory;
 no answer from server""",
    2046: """Can't open shared memory;
 cannot send request event to server""",
    2047: """Wrong or unknown protocol""",
    2048: """Invalid connection handle""",
    2049: """Connection using old (pre-4.1.1)
authentication protocol refused (client option 'secure_auth' enabled)""",
    2050: """Row retrieval was canceled by mysql_stmt_close() call""",
    2051: """Attempt to read column without prior row fetch""",
    2052: """Prepared statement contains no metadata""",
    2053: """Attempt to read a row while there
is no result set associated with the statement""",
    2054: """This feature is not implemented yet""",
    2055: """Lost connection to MySQL server. System error.""",
    2056: """Statement closed indirectly because of a preceeding call""",
    2057: """The number of columns in the
result set differs from the number of bound buffers.
You must reset the statement, rebind the result set
columns, and execute the statement again""",
    2058: """This handle is already connected.
Use a separate handle for each connection.""",
    2059: """Authentication plugin cannot be loaded""",
}

def mysqlerror(errorstring):
    '''
    Returns the Error string associated with the code
    '''
    return MYSQL_ERRORS[errorstring[0]]

def mysqlerr(errorstring):
    '''
    Returns the Error string associated with the code
    '''
    return MYSQL_ERRORS[errorstring]
