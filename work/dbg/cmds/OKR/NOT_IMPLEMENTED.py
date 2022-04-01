from pykd import *
import re

def INTERRUPT_EXCEPTION_NOT_HANDLED(Args):
    dprintln("INTERRUPT_EXCEPTION_NOT_HANDLED {}".format(grp))
    pass
def DRIVER_POWER_STATE_FAILURE(Args):
    dprintln("DRIVER_POWER_STATE_FAILURE {}".format(grp))
    pass
def INTERNAL_POWER_ERROR(Args):
    dprintln("INTERNAL_POWER_ERROR {}".format(grp))
    dprintln(pykd.dbgCommand("!thread " + Args[3]))
    pass
def DRIVER_IRQL_NOT_LESS_OR_EQUAL(Args):
    dprintln("DRIVER_IRQL_NOT_LESS_OR_EQUAL {}".format(grp))
    pass
def CLOCK_WATCHDOG_TIMEOUT(Args):
    dprintln("CLOCK_WATCHDOG_TIMEOUT {}".format(grp))
    pass
def KERNEL_SECURITY_CHECK_FAILURE(Args):
    dprintln("KERNEL_SECURITY_CHECK_FAILURE {}".format(grp))
    pass
def UCMUCSI_LIVEDUMP(Args):
    dprintln("UCMUCSI_LIVEDUMP {}".format(grp))
    pass

