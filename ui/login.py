"""
Module which brings together the UI widgets and places them onto a class which allows us
to easily instantiate the login form from another dialog/widget/window and authenticate
from there without caring *how* the authentication happens.
"""

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
        # this is hardcoded we should/could put this in a local
        # config. But how to make that config the first time?
        self.login_url = "http://localhost:12345/login/"

    def accept(self):
        """
        This accept method constantly looks to see if the login
        details provided were correct, if so, carry on and accept
        the form so that the action that instantiated us can carry
        on. Otherwise, loop back and offer the dialog box again.
        """
        while not self.login():
            self.parent.show_error("Login Failure!")
            return
        self.parent.username = self.gui.txt_username.text()
        self.parent.password = self.gui.txt_password.text()
        QtGui.QDialog.accept(self)

    def reject(self):
        """
        If the dialog gets rejected then we exit the whole application
        """

        QtGui.QDialog.reject(self)
        sys.exit()

    def login(self):
        """
        Method which returns a boolean value base on whether we managed
        to get a positive response from logging in or not.
        """

        # construct the user/pass from the form
        json_payload = {
            'User': self.gui.txt_username.text(),
            'Password': self.gui.txt_password.text(),
            }
        # ping the server
        http_post = urllib2.Request(
            self.login_url,
            urllib.urlencode(json_payload)
            )
        # no error checking because if the json is malformed
        # we're fucked anyway.
        json = simplejson.loads(urllib2.urlopen(http_post).read())
        try:
            return json['Success']
        except KeyError:
            return False
        
            
