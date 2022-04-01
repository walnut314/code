

def Get_Irp_Info (
    addr : str,     # str hex address of ptype
    ptype : str,    # str struct typedef
    subtype : str,  # str struct typedef
    output: STRUCT_IRP_DATA
):
    status : KD_STATUS = {}
    ptr = pykd.ptrPtr(int(addr, 16)) # convert str hex address to pointer
    typeobject = pykd.typedVar(ptype, ptr) # get object for type = nt!_TRIAGE_POP_FX_DEVICE at address ptr
    output['Irp'] = pykd.typedVar(subtype, typeobject.Irp) # subtype = nt!_IRP
    output['Stack'] = pykd.typedVar('nt!_IO_STACK_LOCATION', typeobject.Irp.Tail.Overlay.CurrentStackLocation)
    status = {'Status': 0, 'Reason': 'success', 'Function' : inspect.stack()[0][3]}
    return status



    dprintln("arg1: " + Args[1])
    status = Get_Irp_Info(Args[1], 'nt!_TRIAGE_POP_FX_DEVICE', 'nt!_IRP', irpst)
    if (status['Status'] == 0):
        dprintln('new irp:  {}'.format(irpst['Irp']))
        dprintln('new stk:  {}'.format(irpst['Stack']))
    else:
        dprintln('error: {} in:  {}'.format(status['Reason'], status['Function']))
    #triage = pykd.dbgCommand("dt nt!_TRIAGE_POP_FX_DEVICE " + Args[1])
    #dprintln(triage)
    #ptr = pykd.ptrPtr(int(Args[1], 16))
    #typeobject = pykd.typedVar('nt!_TRIAGE_POP_FX_DEVICE', ptr) 
    #Irp = pykd.typedVar('nt!_IRP', typeobject.Irp)
    #devnode = pykd.dbgCommand("dt nt!_TRIAGE_DEVICE_NODE " + Args[3])
    #dprintln('devnode: {}' .format(devnode))
    return


# https://www.stl-tec.de/tutorials/WinReverseEng/pyKD/
def firstNumberInString(string):
    # meant to get the middle number out of Windbgs: "Evaluate expression: 1104 = 0000000000000450"
    return [int(s) for s in string.split() if s.isdigit()][0]


def dump_irp_threads():
    pThreadList = pykd.getOffset("nt!PopIrpThreadList")
    #pThreadNext = pykd.typedVarList(pThreadList, "nt!_ETHREAD", "ThreadListEntry" )
    dprintln(hex(pThreadList))
    #print(pThreadNext)

def dump_procs2():
    pActiveProcessList = pykd.getOffset("nt!PsActiveProcessHead")
    #pActiveProcessList = pykd.module("nt").PsActiveProcessHead
    processList = pykd.typedVarList(pActiveProcessList, "nt!_EPROCESS", "ActiveProcessLinks" )
    for i, process in enumerate(processList):
        pykd.dprint("Process " + str(i)+":")
        name=pykd.loadCStr(process.ImageFileName)
        dprintln(name)

def dump_procs():
    pActiveProcessHead = pykd.getOffset("nt!PsActiveProcessHead")
    currentEntry  = pykd.ptrPtr(pActiveProcessHead)
    dprintln(hex(pActiveProcessHead))
    dprintln(hex(currentEntry))

    s = pykd.dbgCommand("? @@(#FIELD_OFFSET(nt!_EPROCESS,ActiveProcessLinks))")
    offset_eprocess_activeProcesslinks = firstNumberInString(s)
    #pykd.dprintln("Offset EPROCESS-ActiveProcessLinks: " + str(offset_eprocess_activeProcesslinks))
    s = pykd.dbgCommand("? @@(#FIELD_OFFSET(nt!_EPROCESS, ImageFileName))")
    offset_eprocess_imageName = firstNumberInString(s)
    #pykd.dprintln("Offset EPROCESS-ImageName: " + str(offset_eprocess_imageName))

    while (currentEntry != pActiveProcessHead):
        #pykd.dprintln("Current Flink:  @ 0x%08x"%currentEntry)
        pCurrentEProcessObject = currentEntry - offset_eprocess_activeProcesslinks
        #pykd.dprintln("Current EPROCESS address: @ 0x%08x"%pCurrentEProcessObject)
        currentImageName = pykd.loadCStr( pCurrentEProcessObject + offset_eprocess_imageName )
        pykd.dprintln("Current ImageName: "  + currentImageName)
        if (currentImageName == "csrss.exe"):
            dprintln("found csrss")
        currentEntry = pykd.ptrPtr(pCurrentEProcessObject + offset_eprocess_activeProcesslinks)
    pykd.dprintln("Done")

