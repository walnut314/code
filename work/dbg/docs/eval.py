#!/usr/bin/env python3

#
# File: eval.py
#

# https://rayanfam.com/topics/pykd-tutorial-part1/
# https://githomelab.ru/pykd/pykd
# https://www.zerodayinitiative.com/blog/2018/7/19/mindshare-an-introduction-to-pykd

# 1: kd> .load C:\dev\Dumps\pykd\pykd.dll
# 1: kd> !py C:\dev\Dumps\eval.py

from pykd import *
import re


#print(pykd.dbgCommand("r"))
#line = print(pykd.dbgCommand("!analyze -v"))
#def EvalDump():
num_types = 7
regex = "Arg(\d): ([a-zA-Z0-9]{16})"
regex2 = "(\(3D\))|(\(9F\))|(\(A0\))|(\(D1\))|(\(101\))|(\(139\))|(\(1D4\))"

def dumpargs(Args):
    for i in range(len(Args)):
        print("Arg{}: {}".format(i, Args[i]))

def INTERRUPT_EXCEPTION_NOT_HANDLED(Args):
    print("INTERRUPT_EXCEPTION_NOT_HANDLED")
    pass
def DRIVER_POWER_STATE_FAILURE(Args):
    print("DRIVER_POWER_STATE_FAILURE")
    pass
def INTERNAL_POWER_ERROR(Args):
    print("INTERNAL_POWER_ERROR")
    pass
def DRIVER_IRQL_NOT_LESS_OR_EQUAL(Args):
    print("DRIVER_IRQL_NOT_LESS_OR_EQUAL")
    pass
def CLOCK_WATCHDOG_TIMEOUT(Args):
    print("CLOCK_WATCHDOG_TIMEOUT")
    pass
def KERNEL_SECURITY_CHECK_FAILURE(Args):
    print("KERNEL_SECURITY_CHECK_FAILURE")
    pass
def UCMUCSI_LIVEDUMP(Args):
    print("UCMUCSI_LIVEDUMP")
    pass

dump_maps = dict()
dump_maps["(3D)"]   = ("(3D)",  0x3d,  INTERRUPT_EXCEPTION_NOT_HANDLED);
dump_maps["(9F)"]   = ("(9F)",  0x9f,  DRIVER_POWER_STATE_FAILURE);
dump_maps["(A0)"]   = ("(A0)",  0xa0,  INTERNAL_POWER_ERROR);
dump_maps["(D1)"]   = ("(D1)",  0xd1,  DRIVER_IRQL_NOT_LESS_OR_EQUAL);
dump_maps["(101)"]  = ("(101)", 0x101, CLOCK_WATCHDOG_TIMEOUT);
dump_maps["(139)"]  = ("(139)", 0x139, KERNEL_SECURITY_CHECK_FAILURE);
dump_maps["(1D4)"]  = ("(1D4)", 0x1d4, UCMUCSI_LIVEDUMP);

def which_group(match):
    for i in range(1,7):
        if match.group(i) != None:
            return match.group(i)

grp = None
Args = [1,2,3,4]
pykd.dbgCommand(".logopen /d")
print("triage")
pykd.dbgCommand("||")

lines = pykd.dbgCommand("!analyze -v").splitlines()
for line in lines:
    if grp == None:
        typ = re.search(regex2, line)
        if typ:
            grp = which_group(typ)
    arg = re.search(regex, line)
    if arg:
        Args[int(arg.group(1))-1] = arg.group(2)

dumpargs(Args)
dump_maps[grp][2](Args)

pykd.dbgCommand(".logclose")
pykd.dbgCommand("$")

