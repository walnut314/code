#!/usr/bin/env python

# http://stackoverflow.com/questions/145270/calling-c-c-from-python
# https://docs.python.org/2/library/ctypes.html

from ctypes import *
#from ctypes import cdll

#lib = cdll.LoadLibrary('./foofoo.so')
lib = cdll.LoadLibrary('./foofoo.dll')

foofoo = getattr(lib, 'foofoo')
foobar = getattr(lib, 'foobar')
foo    = getattr(lib, 'foo')
bar    = getattr(lib, 'bar')

#print globals()
#print id(lib)

foofoo()
x = foobar("dude")
print(x)

# http://www.ifnamemain.com/posts/2013/Dec/10/c_structs_python/
class POINT(Structure):
    _fields_ = [("x", c_int),
                ("y", c_int)]

foo.restype = POINTER(POINT)

p = foo(2,4)

ptr = p.contents
print("dude: {}".format(ptr.x))
print("dude: {}".format(ptr.y))

pp = pointer(ptr)

pt = POINT(1,7)
bar(byref(pt))
bar(pp)

