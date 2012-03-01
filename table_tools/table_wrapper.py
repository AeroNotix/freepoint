from PyQt4 import QtCore

'''
Module which provides the wrapper which should be
applied to any method which is sensitive to the contents of the table
'''

def populate_wrapper(caller, *args, **kwargs):


    """
    Wrapper function to enable or disable editing signals, disable sorting and
    any further changes, during table population, other wise rewrite_db gets
    called on selection changed signal from the table, which the table calls
    on population as the pointer iterates through the rows.

    Imports functools, then wraps the argument function up in the inner
    function wrapper, checks if the UI's edit toggle is on,
    of so, disconnects the signal, updates the table, and then switches
    the signal back on.

    the functools module is avaliable, but it is a C library so good
    luck finding where that is.
    """

    import functools

    @functools.wraps(caller)
    def wrapper(obj, *args, **kwargs):

        """
        Real meat of the wrapper.

        obj is the instance of what calls this.

        Turns off SIGNALS, since would call the rewrite_db method,
        clears table, disables sorting and washes the dishes.

        """

        #----------------------------------------------------------
        # If the toggle is active, we deactivate the signals
        # associated with the toggle switch.
        #----------------------------------------------------------

        # if obj.gui.actionToolbarEdit.isChecked():
        #     obj.disconnect(obj.gui.tableWidget, QtCore.SIGNAL(
        #                                    "itemChanged(QTableWidgetItem*)"),
        #                                    obj.rewrite_db)
            # first we clear the table
        obj.gui.tableWidget.clear()                   # Clear items
        obj.gui.tableWidget.clearSpans()              # Clear table
        obj.gui.tableWidget.setSortingEnabled(False)  # Disable sorting

        #-------------------------------------------------------
        #
        # We delete everything row by row, going backwards.
        # You cannot iterate forwards because you're deleting
        # indexes for the rows, so if you delete i, i+1 becomes
        # i, and thus skipped when iter.next() is called.
        #-------------------------------------------------------

        if obj.second_pass == True:
            obj.row_count =  obj.gui.tableWidget.rowCount()

            for i in range(obj.gui.tableWidget.rowCount() + 1):
                obj.gui.tableWidget.removeRow(obj.row_count - i)

        #----------------------------------------------------------
        caller(obj, *args, **kwargs)    # This is where we call the passed in method
        #----------------------------------------------------------

        obj.gui.tableWidget.setWordWrap(True)      # Word wrapping
        obj.gui.tableWidget.setColumnWidth(2, 300)# Stop items spanning screen
        obj.gui.tableWidget.setColumnWidth(3, 250)# Stop items spanning screen
        obj.second_pass = True                     # Set dirty flag
        obj.gui.tableWidget.setSortingEnabled(True) # re-enable sorting
        # Alternate row colours
        obj.gui.tableWidget.setAlternatingRowColors(True)


        #----------------------------------------------------------
        # If the toggle is active, we reactivate the signals
        # associated with the toggle switch.
        #----------------------------------------------------------

        # if obj.gui.actionToolbarEdit.isChecked():
        #     obj.connect(obj.gui.tableWidget, QtCore.SIGNAL(
        #                                    "itemChanged(QTableWidgetItem*)"),
        #                                    obj.rewrite_db)


    return wrapper
