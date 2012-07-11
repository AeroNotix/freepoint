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
        "USERNAME": <username>,
        "PASSWORD": <password>
    }

    `json = {
        "USERNAME": <username>,
        "PASSWORD": <password>
    }`


Editing tabledata
-----------------

Creating a table
----------------
