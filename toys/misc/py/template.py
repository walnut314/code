#!/usr/bin/env python3

#-----------------------------------------------------------------------
# Name:         template.py
# Purpose:      Template playground.
#-----------------------------------------------------------------------

__author__          = "the_dude"
__version__         = "0.0.1"
__date__            = "2020/01/01"
__title__           = "replay"
__email__           = "the_dudet@gmail.com"
__release_notes__   = """
    FREE text
"""

version_string = "%s v%s (%s) %s (%s)" % (__title__, __version__, __date__, __author__, __email__)
introduction_string = """

    --------------------------------------------------------------------

    This code bla bla bla

"""

import sys
print("py ver ", sys.version_info[0])

if sys.version_info[0] != 3:
    sys.stdout.write("""
    %s
    This code expects Python v.
    You are using %s.
    Recommend at least v.
    Find an installer at https://www.python.org/downloads/release.
    Multiple versions can coexist. Use the search path to select.
    """ % (version_string, sys.version))
    exit()

g_win32 = sys.platform == 'win32'
g_posix = 'posix' in sys.builtin_module_names
#if g_win32:
#    import _winreg

import os
import collections
import struct
import re
import time
import binascii
import optparse
import glob
import sqlite3
import traceback
import fnmatch

#-----------------------------------------------------------------------
class OPTIONS:
    """
    """
    def __repr__(self):
        return '<Options: %s>' % (", ".join(map(lambda x: "%s=%s" % (x[0], repr(x[1])), self.__dict__.items())))

options = OPTIONS()

def set_options(*args, **kwds):
    global options
    for k, v in kwds.items():
        options.__dict__[k] = v

def get_options():
    return options.__dict__

set_options(debug=False, error=True, spew=False, exports=None, rraw=False, wraw=False, terse=False, progress=False, nocrc=False)

def hexdump(prefix, address, data, offset=0):
    if prefix is not None: print(prefix)
    if type(data) is str: data = map(ord, data)
    length = len(data)
    while length > 0:
        size = min(length, 8)
        line = '%08X:  ' % (address) + ' '.join(map(lambda x: "%02X" % x, data[offset:offset+size]))
        address += size
        mark = offset
        offset += size
        length -= size
        if length > 0:
            size = min(length, 8)
            line += '  ' + ' '.join(map(lambda x: "%02X" % x, data[offset:offset+size]))
            address += size
            offset += size
            length -= size
        line += ' ' * (61 - len(line)) + '"'
        for c in data[mark:offset]:
            line += (c < 32 or c > 126) and '?' or chr(c)
        print(line + ' ' * (78 - len(line)) + '"')

def globber(path, subdirs=None):
    result = [[]]
    def _gg(path):
        for p in glob.glob(path):
            if os.path.isdir(p):
                if subdirs:
                    _gg(os.path.join(p, '*'))
            else:
                result[0].append(p)
    def _g(path):
        if type(path) in (list, tuple, set):
            for p in path:
                _g(p)
        elif path:
            _gg(os.path.isdir(path) and os.path.join(path, '*') or path)
    _g(path)
    return result[0]

def _spew(spew):
    if options.spew and spew:
        hexdump("SPEW:", 0, spew)

def _debug(text='', spew=None):
    if options.debug:
        print("DBG:", text)
    _spew(spew)

def _error(text='', spew=None):
    if options.error:
        print("ERR: %s, %s" % (sys.exc_value, text))
    _spew(spew)

def crc_16(src):
    return binascii.crc_hqx(src, 0xFFFF)

def root_dirs_files(path):
      root, dirs, files = None, [], []
      for root, dirs, files in os.walk(path): break
      return (root, dirs, files)


class Console:
    """
    """
    def __init__(self):
        try: # windows
            import msvcrt
            self._getch = msvcrt.getch
            self._kbhit = msvcrt.kbhit
        except ImportError: # unix
            import tty, termios
            if 0:
                self.stdinfd = sys.stdin.fileno()
                self.old_settings = termios.tcgetattr(self.stdinfd)
                # termios.tcsetattr(self.stdinfd, termios.TCSADRAIN, self.old_settings)
                tty.setraw(self.stdinfd)
                def __getch():
                    return sys.stdin.read(1)
                def __kbhit():
                    print("dude")
                    pass #TODO
            else:
                def __getch():
                    fd = sys.stdin.fileno()
                    old_settings = termios.tcgetattr(fd)
                    try:
                        tty.setraw(sys.stdin.fileno())
                        ch = sys.stdin.read(1)
                    finally:
                        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
                    return ch
                def __kbhit():
                    pass #TODO
            self._getch = __getch
            self._kbhit = __kbhit

    def getch(self):
        return ord(self._getch())

    def kbhit(self):
        return self._kbhit()

    def putch(self, c):
        sys.stdout.write(chr(c & 0xFF))


def Main():

    import msvcrt
    console = Console()

    while True:
        c = msvcrt.getch()
        print("sup: ", c)
        if c == '.':
            console.putch(data[dptr])
        elif c == ',':
            data[dptr] = console.getch()
        elif c == 'q':
            return

    usage = """
    %prog [-h] [options] <path> ... 
    
    --------------------------------------------------------------------

    To display the contents of a log:
    %prog --dump SomeWonderfulLog.bin

    To see the log blocks in raw binary form:
    %prog --dump --wraw SomeWonderfulLog.bin

    A folder may be specified, and all logs in the folder will be read:
    %prog --dump folder

    To descend to all sub-folders, specify --subdirs:
    %prog --dump --subdirs folder

    Full Unix-style globbing is performed on the arguments, and multiple
    mixed file and folder paths may be specified:
    %prog --dump --subdirs folder file[ab]*.bin logs

    CSV export supports (HEADER, FAULT, BATTERY, BLADE).
    CSV export columns are;
        path, offset, size, type, type_str, <block elements>

    """

    parser = optparse.OptionParser(usage=usage)
    parser.add_option('',   '--block_defs', action="store_true",                    dest="block_defs",      default=False,  help="dump the supported block definitions" )
    parser.add_option('',   '--csv',        action="store",         type="string",  dest="csv_path",        default=None,   help="export in csv format to this path" )
    parser.add_option('',   '--csv_defs',   action="store_true",                    dest="csv_defs",        default=False,  help="dump the supported block csv field list" )
    parser.add_option('',   '--diagnose',   action="store_true",                    dest="diagnose",        default=False,  help="enable diagnostics" )
    parser.add_option('',   '--dump',       action="store_true",                    dest="dump",            default=False,  help="dump the logs" )
    parser.add_option('',   '--exports',    action="append",        type="string",  dest="exports_path",    default=None,   help="IN PROGRESS: exports search path" )
    parser.add_option('',   '--intro',      action="store_true",                    dest="intro",           default=False,  help="display an introduction to logs" )
    parser.add_option('',   '--noerrors',   action="store_true",                    dest="noerrors",        default=False,  help="limit error reporting - almost quiet" )
    parser.add_option('',   '--nocrc',      action="store_true",                    dest="nocrc",           default=False,  help="disregard CRC errors" )
    #parser.add_option('',   '--pattern',    action="append",        type="string",  dest="pattern",         default=None,   help="specify file name pattern" )
    parser.add_option('',   '--progress',   action="store_true",                    dest="progress",        default=False,  help="display a progress indicator to stderr" )
    parser.add_option('',   '--rraw',       action="store_true",                    dest="rraw",            default=False,  help="read data as raw data (no log context)" )
    parser.add_option('',   '--registry',   action="store_true",                    dest="registry",        default=False,  help="search registry for telemetry (log formatted only)" )
    parser.add_option('',   '--release',    action="store_true",                    dest="release",         default=False,  help="display the release notes" )
    parser.add_option('',   '--subdirs',    action="store_true",                    dest="subdirs",         default=False,  help="include subdirectories" )
    parser.add_option('',   '--sqlite',     action="store",         type="string",  dest="sqlite_path",     default=None,   help="EXPERIMENTAL: export to a sqlite3 database" )
    parser.add_option('',   '--terse',      action="store_true",                    dest="terse",           default=False,  help="resist the urge to interpret field values" )
    parser.add_option('',   '--verbose',    action="store_true",                    dest="verbose",         default=False,  help="debug and error messages set to spew-o-matic" )
    parser.add_option('',   '--version',    action="store_true",                    dest="version",         default=False,  help="display the code version information" )
    parser.add_option('',   '--wraw',       action="store_true",                    dest="wraw",            default=False,  help="dump data in raw format (hexdump)" )
    opts, args = parser.parse_args()

    okay = False
    if opts.version:
        print(version_string)
        okay = True
    if opts.release:
        print(__release_notes__)
        okay = True
    if opts.intro:
        print(introduction_string)
        okay = True
    if opts.block_defs:
        #BLOCK.dump_defs()
        okay = True
    if opts.csv_defs:
        #BLOCK.dump_csv_defs()
        okay = True

    print(options)
    #_error("PRE: ", spew=__release_notes__)

    if len(args) < 1 and not opts.registry:
        print("dude")
        return not okay and parser.print_help() or None

    return

if __name__ == '__main__':
    sys.exit(Main())

if 0:
    print("zero")


