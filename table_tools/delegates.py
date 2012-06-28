# TODO: Document this module

from PyQt4 import QtGui

class ComboDelegate(QtGui.QItemDelegate):
    def createEditor(self, parent, option, index):
        choiceEdit = QtGui.QComboBox(parent)
        choiceEdit.addItems(self.data)
        return choiceEdit

    def setModelData(self, editor, model, index):
        model.setData(index, editor.currentText())

class BooleanDelegate(ComboDelegate):
    def __init__(self, data, parent=None):
        if len(data) != 2:
            raise ValueError
        else:
            self.data = data
        super(BooleanDelegate, self).__init__(parent)

class ChoiceDelegate(ComboDelegate):
    def __init__(self, data, parent=None):
        self.data = data
        super(ChoiceDelegate, self).__init__(parent)

DELEGATES = {
    "BOOL": BooleanDelegate,
    "CHOICE": ChoiceDelegate
}


class Delegator(QtGui.QItemDelegate):
    def __init__(self, headers, metadata, parent=None):
        super(Delegator, self).__init__(parent)
        self.parent = parent
        self.metadata = metadata
        self.delegates = {}
        self.headers = headers
        self.parseMetadata()

    def createEditor(self, parent, option, index):
        try:
            delentry = self.delegates[self.headers[index.column()]]
            return delentry.createEditor(parent, option, index)
        except (AttributeError, KeyError):
            return super(Delegator, self).createEditor(parent, option, index)
        
    def setModelData(self, parent, option, index):
        try:
            delentry = self.delegates[self.headers[index.column()]]
            return delentry.setModelData(parent, option, index)
        except (AttributeError, KeyError):
            return super(Delegator, self).setModelData(parent, option, index)
        
    def parseMetadata(self):
        for item in self.headers:
            row_data = self.metadata.get(item)
            if row_data:
                dtype = DELEGATES.get(row_data["TYPE"])
                if dtype:
                    delinst = dtype(row_data["CHOICES"])
                    delinst.setParent(self)
                    self.delegates[item] = delinst
