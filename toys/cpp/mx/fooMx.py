#!/usr/bin/env python

from ctypes import cdll
lib = cdll.LoadLibrary('./libmx.so')

#class Foo(object):
#    def __init__(self):
#        self.obj = lib.Foo_new()
#
#    def bar(self):
#        lib.Foo_bar(self.obj)
#
#f = Foo()
#f.bar() #and you will see "Hello" on the screen

# parse file
with open("test.mx") as f:
    for line in f:
        lib.xrefpy(line.rstrip());

