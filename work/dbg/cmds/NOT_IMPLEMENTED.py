from pykd import *
import re

def DumpArgs(Args):
    for i in range(len(Args)):
        dprintln("Arg{}: {}".format(i, Args[i]))
    
def INTERRUPT_EXCEPTION_NOT_HANDLED(Args):
    dprintln("INTERRUPT_EXCEPTION_NOT_HANDLED")
    pass
def DRIVER_POWER_STATE_FAILURE(Args):
    dprintln("DRIVER_POWER_STATE_FAILURE")
    pass
def INTERNAL_POWER_ERROR(Args):
    dprintln("INTERNAL_POWER_ERROR")
    dprintln(pykd.dbgCommand("!thread " + Args[3]))
    pass
def DRIVER_IRQL_NOT_LESS_OR_EQUAL(Args):
    dprintln("DRIVER_IRQL_NOT_LESS_OR_EQUAL")
    pass
def CLOCK_WATCHDOG_TIMEOUT(Args):
    dprintln("CLOCK_WATCHDOG_TIMEOUT")
    pass
def KERNEL_SECURITY_CHECK_FAILURE(Args):
    dprintln("KERNEL_SECURITY_CHECK_FAILURE")
    pass
def UCMUCSI_LIVEDUMP(Args):
    dprintln("UCMUCSI_LIVEDUMP")
    pass
def DPC_WATCHDOG_VIOLATION(Args):
    dprintln("DPC_WATCHDOG_VIOLATION")
    pass
def DRIVER_POWER_STATE_FAILURE(Args):
    dprintln("DRIVER_POWER_STATE_FAILURE 9F")
    DumpArgs(Args)
    pass

