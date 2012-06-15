
Main GUI
========

The main GUI module which brings together most of the functionality

.. class:: MainGui

   .. method:: __init__(parent=None)

               The initialization method which creates the instance of the main window

               :param parent: A Qt object which is the 'owner' of this object.
                              This is used if the parent is closed/killed therefore
                              this object must also be killed.
   
   .. method:: populate_table(self, host='localhost', user=None, password=None, using_db=None, table=None, query=None)

                Opens and displays a MySQL table.

                Mutates the table's headings and the data in the table with
                the data returned from the select on the passed in database.

                :param host: :class:`string` which holds the hostname for the remote/local MySQL instance
                :param user: :class:`string` which is the user to connect as.
                :param using_db: :class:`string` which holds the database to connect using.
                :param table: :class:`string` which is the table to query. Optional.
                :param query: :class:`string` This parameter can override the default query and run a
                              different query in it's place.

   .. method:: clear_table(self)

        Method to clear the table.

        We iterate forwards but we delete the opposite side of the table because
        we can be safe knowing that we aren't iterating over the iterable of rows
        whilst changing the size of the iterable.

        :returns: :class:`None`
