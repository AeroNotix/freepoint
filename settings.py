import os
import sys

SERVERURL = "http://16.55.133.115:12345/"

def module_path():
    """
    This will get us the program's directory,
    even if we are frozen using py2exe
    """

    if hasattr(sys, "frozen"):
        return os.path.dirname(unicode(sys.executable, sys.getfilesystemencoding( )))

    return os.path.dirname(unicode(__file__, sys.getfilesystemencoding( )))
