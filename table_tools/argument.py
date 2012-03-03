#!/usr/bin/env python2
# -*- coding: utf-8 -*-

"""
Module for making creating arguments cleaner for the argparse module
A way to create nice **args for the add_argument function

ArgumentParser.add_argument(name or flags...[, action][, nargs][, const][,
                            default][, type][, choices][, required][, help]
                            [, metavar][, dest])
"""

class Argument(object):

    """
    Argument class
    """

    def __init__(self, name, prefix='-', choices=None, required=False,
                 help='',type=str):

        """
        Just creates a dict that we can access for kwarg style
        unpacking

        i.e. (parser.add_argument(Argument.names, **Argument.data)
        
        """
        
        self.name = prefix+name
        self.data = dict(type=type,
                         choices=choices,
                         required=required,
                         help=help)
