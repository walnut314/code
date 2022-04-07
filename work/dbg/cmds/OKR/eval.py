#!/usr/bin/env python3

#
# File: eval.py
#

# https://rayanfam.com/topics/pykd-tutorial-part1/
# https://githomelab.ru/pykd/pykd
# https://www.zerodayinitiative.com/blog/2018/7/19/mindshare-an-introduction-to-pykd
# https://python.hotexamples.com/examples/pykd/-/typedVar/python-typedvar-function-examples.html
# D:\Intel_Dev\Bugs\Largo\Take_Home\src\Tools\Display\DebugTools\pyKd\scripts\main.py

# 1: kd> .load pykd.dll
# 1: kd> !py eval.py

from pykd import *
import re

# external links
import CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP as CS
import NOT_IMPLEMENTED as NOT_IMPL

num_types   = 8 # number of supported bugcheck types
args_regex  = "Arg(\d): ([a-zA-Z0-9]{16})"
type_regex  = "(3D)|(9F)|(A0)|(D1)|(101)|(139)|(1D4)|(15F)|(133)"
grp         = None # this is the dump signature/type, e.g. (9F)
Args        = [0] * 4 # declare 4-element array for bugcheck arguments

def dumpargs(Args):
    for i in range(len(Args)):
        dprintln("Arg{}: {}".format(i, Args[i]))

# Note: the Bugcheck IDs for the hash map need to be UPPERCASE
dump_maps = dict()
dump_maps["3D"]   = NOT_IMPL.INTERRUPT_EXCEPTION_NOT_HANDLED;
dump_maps["9F"]   = NOT_IMPL.DRIVER_POWER_STATE_FAILURE;
dump_maps["A0"]   = NOT_IMPL.INTERNAL_POWER_ERROR;
dump_maps["D1"]   = NOT_IMPL.DRIVER_IRQL_NOT_LESS_OR_EQUAL;
dump_maps["101"]  = NOT_IMPL.CLOCK_WATCHDOG_TIMEOUT;
dump_maps["133"]  = NOT_IMPL.DPC_WATCHDOG_VIOLATION;
dump_maps["139"]  = NOT_IMPL.KERNEL_SECURITY_CHECK_FAILURE;
dump_maps["1D4"]  = NOT_IMPL.UCMUCSI_LIVEDUMP;
dump_maps["15F"]  = CS.CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP;

def get_dump_type(match):
    global num_types
    dprintln("dump match {}".format(match))
    for i in range(0,num_types):
        dprintln("grp {}={}".format(i, match.group(i)))
    if match.group(0) != None:
        dprintln("found {}".format(match.group(0)))
        return match.group(0)
    dprintln("no match found")

def main():
    global grp

    dprint(': {}' .format(float(0.27)) + ' Gbps\n')
    dprint("Intel dump analysis\n")
    lines = pykd.dbgCommand("!analyze -v").splitlines()
    for line in lines:
        if grp == None:
            typ = re.search(type_regex, line, re.IGNORECASE)
            if typ:
                print("type {}".format(typ))
                grp = get_dump_type(typ)
        arg = re.search(args_regex, line)
        if arg:
            print(arg)
            Args[int(arg.group(1))-1] = arg.group(0) #(2)

    if grp != None:
        grp = grp.upper()
        dprintln("Triaging bugchk {}".format(grp))
        dump_maps[grp](Args) ## call bugcode handler

    pykd.dbgCommand(".logclose")
    pykd.dbgCommand("$")

if __name__ == '__main__':
    dprintln("hello, dude")
    main()

