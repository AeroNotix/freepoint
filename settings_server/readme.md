API Outline
===========

Logging in
----------

Authentication is achieved by sending a request to `domain:port/login/` with
the appropriate content-headers for JSON along with the content-length set
properly for the length of the JSON string.

A typical JSON request for logging in will look like this:

```javascript
    json = {
        "USERNAME": <username>,
        "PASSWORD": <password>
    }
```

Password hashing will be employed server-side so you may send it clear
text. This is subject to change and this document should be consulted if you
cannot log in.

A typical return for a successful request looks like:

```javascript
    json = {
        "Success": true
    }
```

Eventually the authentication will move to providing you with a cookie and/or
a session ID so that real authentication and session management can take
place. This will work for now whilst the prototype is being finalized.

Viewing a table
---------------

Viewing a table is as simple as ensuring that you are authenticated using the
above method and then sending a request to `domain:port/getdb/` with the below
JSON:

```javascript
    json = {
        "DATABASE": <database>,
        "TABLE": <table>
    }
```

You will then receive a response such as:

```javascript

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
```

The data fully supports unicode so setting data to Unicode-code points will
cause no problems.

* Rows

Rows is an array of string arrays which constitute of the raw data in the
table. Each table will contain an ID and the subsequent columns for that
table.

* Headings

Headings is an array of strings which constitutes the column names for the
table.

* Metadata

Metadata is the interesting piece of information sent back from the
server. This data is made up of the various extra information which make
displaying, editing and validating the changes to the data easy.

Each metadata entry for a given row will contain a ROWNUM which is the
column's horizontal position and it will also contain the ROWDATA which is
broken down as follows:

TYPE
----

Type shows the column type of the row. In the provided client this will change
which Item Delegate editor widget is used to edit the data.

UNIQUE
------

Unique determines whether the given data should be unique among that
column. This is currently only enforced at the database level, but nothing is
stopping the client from enforcing this and validating it on the client side.

NULL
----

Null displays whether the given data is allowed to be NULL, or contain no
value.

LEN
---

When the column type is TEXT or VARCHAR this is used to show the maximum
length of the string in the column.

CHOICES
-------

When the column type is CHOICE then this will contain the options that should
be available for choice in any UI that will be used. This is obviously not
enforced on the database level so client side validation is a must. Having
choices also sets the database column type to VARCHAR with a maximum length to
the maximum length of all the strings in the choices.

Inserting a row
---------------

Inserting a row should happen with a full row of data, the results are
undefined if supplied data is partial or does not contain all the fields for
the table. The first element of the data array should be NULL. This is because
each table has an ID column which we want to automatically increment.

A typical insert request looks like this:

```javascript

    json = {
        "DATABASE": <database>,
        "TABLE": <table>,
        "DATA": [NULL, array, of, strings]
    }


Editing tabledata
-----------------

Editing the data for a column should invoke a change *per cell*. This ensures
that the user has feedback to know *which* piece of data failed to validate
and they will be able to fix their data rather than guessing which piece of
data failed to validate in the group of data they sent.

A typical edit request will look like this:

```javascript

    json = {
        "DATABASE": <database>,
        "TABLE": <table>,
        "COLUMN": <column-name>,
        "DATA": <new-data>,
        "ID": <row-id>
    }
```

To which a typical response will be:

```javascript

    json = {
        "Success": true
    }

Creating a table
----------------

Creating a table is the about creating the necessary metadata for the database
you are constructing.

The JSON metadata serves as both the data which gets interpreted into raw SQL
and what gets stored in the metadata table as what will get served when the
relevant tables get accessed.

Each table should be described with at least:

```javascript

    json = {
        "DATABASE": <database>,
        "TABLE": <table>,
        "HEADINGS": <headings map>,
        "PAYLOAD": <string encoded headings map>,
    }
```

This is the message which the server will receive which contains the needed
fields to create a new table.

Database
--------

Type: string.

This represents the logical database in which the table will be created
under. If this table does not exist then an error will be returned and the
table will not be created.

Table
-----

Type: string

This represents the table name under which the new table will be referred to
as. Usual table naming restrictions apply. See your database manual for
allowed names. If this is null then the request will return an error.

Headings
--------

Type: Map/Associative Array

This represents the metadata associated with the new table and makes up the
meat of this request. This call will contain at least:

```javascript

   HEADINGS = {
       "RowA" : {
          "RowNum": <col number, integer>  // Real programming languages implement Maps as hashtables
          "RowData": {
              "TYPE": <type name, string>,     // see supported types
              "NULL": <allow NULL, bool>,
              "UNIQUE": <is unique, bool>
          }
       }
```

The above is what all request will have since it is the most basic data
required to make a column. What makes the UNIQUE and NULL interesting is that
they have validators both server-side (database level) and are implemented in
the reference client as client-side validation rules. Below is a deeper
explanation of this.

Specific datatypes will have their own extra data associations and are
required to insert them as so:

```javascript
    json = {
        "DIRECTIVE": <type>
    }

    // in the case of a CHOICE column this is implemented as so:

    {
        "CHOICES": ["Choice1", "Choice2", "Choice3"]
    }
```

Herein lies an interesting property. Consider that we know that a column is a
choice type, based on the metadata that we received saying as much. We can
build in validation rules which are enforced at the client level. Rather than
server-side (where they are also implemented). Since this will allow a rich
set of data manipulation functions not normally allowed in direct database
connections.  Consider again that specifying extra data about what kinds of
validation is required could be attached to the database metadata at database
creation time in arbitrary json fields then the design is quite extensible
providing that the implementation and execution of validation rules is carried
out.

A reference metadata decoder and dynamic application of validation rules is
supplied in the reference client implementation, please see the code in
qtsqlviewer/table_tools/tools.py and qtsqlviewer/ui/createDatabase.py for more
details in how the validation rules are enforced and how new validation
functions can be written.

Payload
-------

Payload is a json string encoded version of the headings map. This may not be
required in future versions as this could be implemented server-side. As for
now just encode JSON into a string as you normally would prior to sending the
JSON data and insert that into what will be the final JSON message.
