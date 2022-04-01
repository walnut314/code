from pykd import *
import re


# str hex addr, type str, subtype str
def GetType(addr, ptype, subtype, offset):
    ptr = pykd.ptrPtr(int(addr, 16))
    typeobject = pykd.typedVar(ptype, ptr) 
    dprintln('sub: {}'.format(subtype))
    #subtype = pykd.typedVar(subtype, typeobject.Irp)
    #offtype = subtype + '.' + offset
    #subtype = pykd.typedVar(subtype, typeobject.Irp)
    #subtype = pykd.typedVar(subtype, offtype)
    return subtype

def CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP(Args):
    for i in range(len(Args)):
        dprintln("Arg{}: {}".format(i, Args[i]))
    dprintln("CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP")
    dprintln("arg1: " + Args[1])
    NewIrp = GetType(Args[1], 'nt!_TRIAGE_POP_FX_DEVICE', 'nt!_IRP', 'Irp')
    dprintln('new irp:  {}' .format(NewIrp))
    #triage = pykd.dbgCommand("dt nt!_TRIAGE_POP_FX_DEVICE " + Args[1])
    #dprintln(triage)
    #ptr = pykd.ptrPtr(int(Args[1], 16))
    #typeobject = pykd.typedVar('nt!_TRIAGE_POP_FX_DEVICE', ptr) 
    #Irp = pykd.typedVar('nt!_IRP', typeobject.Irp)
    #dprintln('ptr:  {}' .format(ptr))
    #dprintln('type: {}' .format(typeobject))
    #dprintln('irp:  {}' .format(Irp))
    #devnode = pykd.dbgCommand("dt nt!_TRIAGE_DEVICE_NODE " + Args[3])
    #dprintln('devnode: {}' .format(devnode))

    return
