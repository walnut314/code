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

