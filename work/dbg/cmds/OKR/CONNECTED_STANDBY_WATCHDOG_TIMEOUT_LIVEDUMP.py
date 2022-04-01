from pykd import *
import re
from typing import Dict, Union, Set
import inspect

STRUCT_IRP_DATA = {'Irp': 0, 'Stack': 0}
KD_STATUS = {'Status': 0, 'Reason' : '', 'Function' : ''}

class CON_STANDBY():
    def __init__(self, Args):
        dprintln("CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP")
        for i in range(len(Args)):
            dprintln("Arg{}: {}".format(i, Args[i]))
        self.arg1 = Args[0]
        self.arg2 = Args[1]
        self.arg3 = Args[2]
        self.arg4 = Args[3]
        self.triage_ptr = pykd.ptrPtr(int(self.arg2, 16))
        self.triage_obj = pykd.typedVar('nt!_TRIAGE_POP_FX_DEVICE', self.triage_ptr)
        self.output : STRUCT_IRP_DATA = {}

    def parse(self):
        status : KD_STATUS = {}
        self.output['Irp'] = pykd.typedVar('nt!_IRP', self.triage_obj.Irp) # subtype = nt!_IRP
        self.output['Stack'] = pykd.typedVar('nt!_IO_STACK_LOCATION', self.triage_obj.Irp.Tail.Overlay.CurrentStackLocation)
        status = {'Status': 0, 'Reason': 'success', 'Function' : inspect.stack()[0][3]}
        return status

    def show(self):
        dprintln('new irp:  {}'.format(self.output['Irp']))
        dprintln('new stk:  {}'.format(self.output['Stack']))

def CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP(Args):
    status : KD_STATUS = {}

    cstdby = CON_STANDBY(Args)
    status = cstdby.parse()
    if (status['Status'] == 0):
        cstdby.show()
    else:
        dprintln('error: {} in:  {}'.format(status['Reason'], status['Function']))

    return


