API Outline
===========

Logging in
----------

Authentication is achieved by sending a request to `domain:port/login/` with the appropriate
content-headers for JSON along with the content-length set properly for the length of the
JSON string.

A typical JSON request for logging in will look like this:

.. code-block:: javascript

    json = {
        "USERNAME": <username>,
        "PASSWORD": <password>
    }

Password hashing will be employed server-side so you may send it clear text. This is subject
to change and this document should be consulted if you cannot log in.

A typical return for a successful request looks like:

.. code-block:: javascript

    json = {
        "Success": true
    }

Eventually the authentication will move to providing you with a cookie and/or a session ID
so that real authentication and session management can take place. This will work for now
whilst the prototype is being finalized.

Viewing a table
---------------

Viewing a table is as simple as ensuring that you are authenticated using the above method
and then sending a request to `domain:port/getdb/` with the below JSON:

.. code-block:: javascript

    json = {
        "DATABASE": <database>,
        "TABLE": <table>
    }

You will then receive a response such as:

.. code-block:: javascript

    {
    'Rows': [
            ['1', 'Data', 'Pending', '2000-01-01'],
            ['2', 'DATA', 'Open', '2000-01-01']
          ],
    'Headings': ['id', 'RowA', 'Status', 'EntryDate'],
    'Metadata': {
             'HEADINGS': {
                  'Status': {
                         'ROWNUM': 1,
                         'ROWDATA': {
                             'UNIQUE': False,
                             'NULL': False,
                             'TYPE': u'CHOICE',
                             'CHOICES': ['Open', 'Closed', 'Pending']
                         }
                  },
                  'EntryDate': {
                        'ROWNUM': 2,
                        'ROWDATA':
                                {
                                'UNIQUE': False,
                                'NULL': True,
                                'TYPE': 'DATE'
                        }
                  },
                  'RowA': {
                       'ROWNUM': 0,
                       'ROWDATA': {
                               'UNIQUE': False,
                               'NULL': False,
                               'TYPE': 'VARCHAR',
                               'LEN': 255
                       }
                  }
            }
    }
    }

The data fully supports unicode so setting data to Unicode-code points will cause
no problems.

* Rows

Rows is an array of string arrays which constitute of the raw data in the table. Each table will contain
an ID and the subsequent columns for that table.

* Headings

Headings is an array of strings which constitutes the column names for the table.

* Metadata

Metadata is the interesting piece of information sent back from the server. This data is made up of the
various extra information which make displaying, editing and validating the changes to the data easy.

Each metadata entry for a given row will contain a ROWNUM which is the column's horizontal position
and it will also contain the ROWDATA which is broken down as follows:

TYPE
####

Type shows the column type of the row. In the provided client this will change which Item Delegate editor
widget is used to edit the data.

UNIQUE
######
Unique determines whether the given data should be unique among that column. This is currently only enforced
at the database level, but nothing is stopping the client from enforcing this and validating it on the client
side.

NULL
####

Null displays whether the given data is allowed to be NULL, or contain no value.

LEN
###

When the column type is TEXT or VARCHAR this is used to show the maximum length of the string in the
column.

CHOICES
#######

When the column type is CHOICE then this will contain the options that should be available for choice
in any UI that will be used. This is obviously not enforced on the database level so client side validation
is a must. Having choices also sets the database column type to VARCHAR with a maximum length to the maximum
length of all the strings in the choices.

Editing tabledata
-----------------

Editing the data for a column should invoke a change *per cell*. This ensures that the user has feedback
to know *which* piece of data failed to validate and they will be able to fix their data rather than 
guessing which piece of data failed to validate in the group of data they sent.

A typical edit request will look like this:

.. code-block:: javascript

    json = {
        "DATABASE": <database>,
        "TABLE": <table>,
        "COLUMN": <column-name>,
        "DATA": <new-data>,
        "ID": <row-id>
    }

To which a typical response will be:

.. code-block:: javascript

    json = {
        "Success": true
    }

Creating a table
----------------
