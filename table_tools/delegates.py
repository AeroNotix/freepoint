from PyQt4 import QtGui


class ComboDelegate(QtGui.QItemDelegate):
    """
    Base class for all delegates which are combo boxes.

    The only method which is different is the constructor so the subclasses
    will only override that method and the base class' methods will suffice
    just fine for everything else.
    """

    def __init__(self, parent=None):
        """
        Our base class methods use a reference to self.data for their items so
        we give ourselves something to use. Subclasses will override this
        anyway.
        """

        self.data = list()
        super(ComboDelegate, self).__init__(parent)

    def createEditor(self, parent, option, index):
        """
        Returns an instance of the delegated widget with which to interact
        with the model's data. This is a ComboDelegate so we return a
        ComboBox. In our initializer we took the data to use for the items so
        we attach those items to the widget.
        """

        choiceEdit = QtGui.QComboBox(parent)
        choiceEdit.addItems(self.data)
        return choiceEdit

    def setModelData(self, editor, model, index):
        """
        Method to insert the correct text from the choice box into the
        tableWidget's viewport.
        """

        model.setData(index, editor.currentText())


class BooleanDelegate(ComboDelegate):
    """
    This class throws a value error if len(data) != 2. This is to signify that
    the delegate returned by this is a boolean or binary switch.
    """

    def __init__(self, data, parent=None):
        if len(data) != 2:
            raise ValueError
        else:
            self.data = data
        super(BooleanDelegate, self).__init__(parent)


class ChoiceDelegate(ComboDelegate):
    """
    This class takes any number of items to display as the choices. This could
    be for example a group table, or a list of options.
    """
    def __init__(self, data, parent=None):
        self.data = data
        super(ChoiceDelegate, self).__init__(parent)

DELEGATES = {
    "BOOL": BooleanDelegate,
    "CHOICE": ChoiceDelegate
}


class Delegator(QtGui.QItemDelegate):
    """
    Considering that tables and other data view items take a single
    QItemDelegate subclass. We need to bring together several QItemDelegate
    classes and dispatch calls to other instances in order to dynamically
    create our display. First of all, we take a reference to the headers and
    the metadata we're using for this table. We then parse the metadata and
    create a map of columns to the delegate that we want to use for that
    column. Upon having the overridden methods called we will lookup the
    delegate instance for that column and return the method from that instance
    instead. This is so we can be as generic as possible in here instead of
    conditionally checking the columns or other such nonsense.
    """
    def __init__(self, headers, metadata, parent=None):
        super(Delegator, self).__init__(parent)
        self.parent = parent
        self.metadata = metadata
        self.delegates = {}
        self.headers = headers
        self.parseMetadata()

    def createEditor(self, parent, option, index):
        """
        Find the name of the column and return the special delegate instance
        if we've got one, otherwise we defer to the base class' method for
        creating the editor.
        """
        try:
            delentry = self.delegates[self.headers[index.column()]]
            return delentry.createEditor(parent, option, index)
        except (AttributeError, KeyError):
            return super(Delegator, self).createEditor(parent, option, index)

    def setModelData(self, parent, option, index):
        """
        Similar to creating the editor, we check which column and dispatch the
        method to the instance of the delegate attached to the columns, firing
        off the base class if there is no special instance for that column.
        """
        try:
            delentry = self.delegates[self.headers[index.column()]]
            return delentry.setModelData(parent, option, index)
        except (AttributeError, KeyError):
            return super(Delegator, self).setModelData(parent, option, index)

    def parseMetadata(self):
        """
        The metadata contains the types of rows and the data associated with
        each row. We figure out from that which columns should have which
        QItemDelegate associated with it and put an instance in our map with
        the column name as the key.
        """
        for item in self.headers:
            row_data = self.metadata.get(item)
            if row_data:
                dtype = DELEGATES.get(row_data["TYPE"])
                if dtype:
                    delinst = dtype(row_data["CHOICES"])
                    delinst.setParent(self)
                    self.delegates[item] = delinst
