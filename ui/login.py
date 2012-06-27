import sys
import urllib2, urllib
import simplejson
from PyQt4 import QtGui
from qtsqlviewer.ui.login_UI import Ui_frm_login

class Login(QtGui.QDialog):
    """
    Form which encapsulates the login 'screen'
    """

    def __init__(self, parent=None):
        """
        Form initializer
        """

        QtGui.QDialog.__init__(self, parent)
        self.gui = Ui_frm_login()
        self.gui.setupUi(self)
        self.parent = parent
        self.login_url = "http://localhost:12345/login/"

    def accept(self):
        while not self.login():
            self.parent.show_error("Login Failure!")
            return
        QtGui.QDialog.accept(self)

    def reject(self):
        QtGui.QDialog.reject(self)
        sys.exit()

    def login(self):
        json_payload = {
            'User': self.gui.txt_username.text(),
            'Password': self.gui.txt_password.text(),
            }
        http_post = urllib2.Request(
            self.login_url,
            urllib.urlencode(json_payload)
            )
        json = simplejson.loads(urllib2.urlopen(http_post).read())#
        try:
            return json['Success']
        except KeyError:
            return False
        
            
