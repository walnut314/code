"use strict";

// File: eval2.js
// .scriptload C:\dev\Dumps\eval2.js
// .scriptunload eval2.js
// kd> dx Debugger.State.Scripts.eval2.Contents.EvalDump()

// TODO: CXR analysis
//   replace all let line(s) with spew(s)
// 22ww2.4_Zaca_21ww50_SAM_Fail_to_resume_from_CS <-- needs digging into, manually dumped with power button
// create database of analyses
// cleanup regex's

let regex       = /Arg(\d): ([a-fA-F0-9]{16})/;
let irpex       = /.* = ([a-fA-F0-9]{8}`[a-fA-F0-9]{8})/;
let addrex      = /.* = ([a-fA-F0-9]{8}`[a-fA-F0-9]{8})/;
let addrex2     = /([a-fA-F0-9]{8}`[a-fA-F0-9]{8})/;
let addrex3     = /([a-fA-F0-9]{16})/;
let hex8ex3     = /([a-fA-F0-9]{8})/;
let addrex4     = /([a-fA-F0-9]{8})`([a-fA-F0-9]{8})/;
let bufex       = /(\".*\")/;
let exec        = undefined;
//let logln       = undefined;
//let spew        = undefined;
//let spew2       = undefined;
//let grep        = undefined;
//let find        = undefined;
//let dumpargs    = undefined;
let logpath     = "C:\\sandbox\\dump\\dogfood\\";

function initializeScript(){}

function init() {
    utils_init();
//    host.namespace.Debugger.State.Scripts.utils.Contents.utils_init();
//    logln    = host.namespace.Debugger.State.Scripts.utils.Contents.logln;
//    spew     = host.namespace.Debugger.State.Scripts.utils.Contents.spew;
//    spew2    = host.namespace.Debugger.State.Scripts.utils.Contents.spew2;
//    dumpargs = host.namespace.Debugger.State.Scripts.utils.Contents.dumpargs;
//    find     = host.namespace.Debugger.State.Scripts.utils.Contents.find;
//    grep     = host.namespace.Debugger.State.Scripts.utils.Contents.grep;
//    bufex    = new RegExp('(\".*\")');
}

function CXR(cxr_addr) {
    for (let Line of exec('.cxr ' + cxr_addr)) {
        logln("cxr: " + Line);
    }
}

function EXR(exr_addr) {
    for (let Line of exec('.exr ' + exr_addr)) {
        logln("exr: " + Line);
        // ExceptionAddress: fffff80670be5cab (intelppm!HwpInterruptService+0x000000000000001b)
        if (Line.includes("ExceptionAddress: ")) { 
            var matches = Line.match(addrex3);
            var addr = matches[1];
            var matches = Line.match(/ \((.*)\+/);
            var module = matches[1];
        }
        if (Line.includes("ExceptionCode: ")) { 
            var matches = Line.match(hex8ex3);
            var code = matches[1];
        }
    }
    this.code = code;
    this.addr = addr;
    this.module = module;
    //logln("exr code:   " + this.code);
    //logln("exr addr:   " + this.addr);
    //logln("exr module: " + this.module);
}

function IRP(irp_addr) {
    for(let Line of exec('!irp ' + irp_addr)) {
        //logln("irp: " + Line);
        if (Line.includes("current (=")) { 
            var matches = Line.match(addrex3);
            var current = matches[1];
        }
        if (Line.includes("buffer=")) { 
            var matches = Line.match(/buffer=([a-fA-F0-9]{16}):/);
            var buffer = matches[1];
        }
        if (Line.includes(": Thread ")) { 
            var matches = Line.match(/: Thread ([a-fA-F0-9]{16}):/);
            if (matches != null) {
                logln("thread matches: " + matches);
                var thread = matches[1];
            }
        }
    }
    this.addr    = irp_addr;
    this.current = current;
    this.irp_mj  = null;
    this.thread  = thread;
    this.buffer  = buffer;
    this.pending = Get_Value(irp_addr, "nt!_IRP", "PendingReturned");
    //logln("irp.addr:    " + this.addr);
    //logln("irp.current: " + this.current);
    //logln("irp.irp_mj:  " + this.irp_mj);
    //logln("irp.thread:  " + this.thread);
    //logln("irp.buffer:  " + this.buffer);
    //logln("irp.pending: " + this.pending);
}

function DumpFactory(signature, handler) { // creates a struct
    this.signature = signature; // string
    this.handler = handler;     // dump parser
    this.bucket = null;
    this.module = null;
    this.thread = null;
    this.trap = null;
    this.file = null;
    this.cxr = null;
    this.lock = null;
    this.symbol_name = null;
}

var dump_maps = new Map();
dump_maps.set("(A)",   new DumpFactory("(A)",   IRQL_NOT_LESS_OR_EQUAL_A));
dump_maps.set("(1A)",  new DumpFactory("(1A)",  MEMORY_MANAGEMENT_1A));
dump_maps.set("(1E)",  new DumpFactory("(1E)",  KMODE_EXCEPTION_NOT_HANDLED_1E));
dump_maps.set("(3B)",  new DumpFactory("(3B)",  SYSTEM_SERVICE_EXCEPTION_3B));
dump_maps.set("(3D)",  new DumpFactory("(3D)",  INTERRUPT_EXCEPTION_NOT_HANDLED_3D));
dump_maps.set("(50)",  new DumpFactory("(50)",  PAGE_FAULT_IN_NONPAGED_AREA_50));
dump_maps.set("(7E)",  new DumpFactory("(7E)",  SYSTEM_THREAD_EXCEPTION_NOT_HANDLED_7E));
dump_maps.set("(9F)",  new DumpFactory("(9F)",  DRIVER_POWER_STATE_FAILURE_9F));
dump_maps.set("(A0)",  new DumpFactory("(A0)",  INTERNAL_POWER_ERROR_A0));
dump_maps.set("(D1)",  new DumpFactory("(D1)",  DRIVER_IRQL_NOT_LESS_OR_EQUAL_D1));
dump_maps.set("(101)", new DumpFactory("(101)", CLOCK_WATCHDOG_TIMEOUT_101));
dump_maps.set("(116)", new DumpFactory("(116)", VIDEO_TDR_FAILURE_116));
dump_maps.set("(117)", new DumpFactory("(117)", VIDEO_TDR_TIMEOUT_DETECTED_117));
dump_maps.set("(119)", new DumpFactory("(119)", VIDEO_SCHEDULER_INTERNAL_ERROR_119));
dump_maps.set("(124)", new DumpFactory("(124)", LKR_WHEA_UNCORRECTABLE_ERROR_124));
dump_maps.set("(133)", new DumpFactory("(133)", DPC_WATCHDOG_VIOLATION_133));
dump_maps.set("(139)", new DumpFactory("(139)", KERNEL_SECURITY_CHECK_FAILURE_139));
dump_maps.set("(13A)", new DumpFactory("(13A)", KERNEL_MODE_HEAP_CORRUPTION_13A));
dump_maps.set("(141)", new DumpFactory("(141)", VIDEO_ENGINE_TIMEOUT_DETECTED_141));
dump_maps.set("(15E)", new DumpFactory("(15E)", LKR_BUGCODE_NDIS_DRIVER_LIVE_DUMP_15E));
dump_maps.set("(15F)", new DumpFactory("(15F)", CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP_15F));
dump_maps.set("(164)", new DumpFactory("(164)", LKR_WIN32K_CRITICAL_FAILURE_164));
dump_maps.set("(193)", new DumpFactory("(193)", VIDEO_DXGKRNL_LIVEDUMP_193));
dump_maps.set("(19C)", new DumpFactory("(19C)", WIN32K_POWER_WATCHDOG_TIMEOUT_19C));
dump_maps.set("(1A1)", new DumpFactory("(1A1)", LKR_WIN32K_CALLOUT_WATCHDOG_LIVEDUMP_1A1));
dump_maps.set("(1C8)", new DumpFactory("(1C8)", MANUALLY_INITIATED_POWER_BUTTON_HOLD_1C8));
dump_maps.set("(1CC)", new DumpFactory("(1CC)", LKR_EXRESOURCE_TIMEOUT_LIVEDUMP_1CC));
dump_maps.set("(1D4)", new DumpFactory("(1D4)", UCMUCSI_LIVEDUMP_1D4));


function Classify(line) {
    for (let [key, dumper] of dump_maps) { // makes case insensitive
        if (line.toLowerCase().includes(dumper.signature.toLowerCase())) {
            return dumper.signature;
        }
    }
    return null;
}

function CreateDebugStack() {
    exec(".logappend " + logpath + "DebugStack.txt")
    // create DebugStack.txt with: "!sym prompts;.reload;!analyze -v;.ecxr;!for_each_frame dv /t;q"
    spew("||");
    spew("!sym prompts");
    spew(".reload");
    spew("!analyze -v");
    spew(".ecxr");
    spew("!for_each_frame dv /t");
    exec(".logclose")
}

function Dispatch(idx, args) {
    CreateDebugStack();
    exec(".logappend " + logpath + "DrillDown.txt")
    logln("***> Intel Dump Analyzer <***\n");
    spew("||")
    logln("");
    if (!dump_maps.get(idx).handler(args)) {
        logln(dump_maps.get(idx).bucket + " bucket handler NOT IMPLEMENTED");
    }
    exec(".logclose")
}

// 2: kd> dx Debugger.State.Scripts.eval.Contents.EvalDump()
function EvalDump() {
    var Args = [];
    var index = null;
    //exec = host.namespace.Debugger.Utility.Control.ExecuteCommand;
    init();

    var bucket = null;
    var module = null;
    var thread = null;
    for(let Line of exec('!analyze -v')) {
        if (index == null) index = Classify(Line);
        if (Line.match(/Arg\d:/)) { 
            var matches = Line.match(regex);
            var idx = parseInt(matches[1]) - 1;
            Args[idx] = matches[2];
        }
        if (Line.match(/FAILURE_BUCKET_ID: (.*)/)) {
            var matches = Line.match(/FAILURE_BUCKET_ID: (.*)/);
            bucket = matches[1];
            logln("bucket: " + bucket);
        }
        if (Line.match(/MODULE_NAME: (.*)/)) {
            var matches = Line.match(/MODULE_NAME: (.*)/);
            module = matches[1];
            logln("module: " + module);
        }
        if (Line.match(/FAULTING_THREAD: (.*)/)) {
            var matches = Line.match(/FAULTING_THREAD: (.*)/);
            thread = matches[1];
            logln("thread: " + thread);
        }
        if (Line.includes("TRAP_FRAME: ")) {
            var matches = Line.match(/TRAP_FRAME:.*([a-fA-F0-9]{16}) /);
            var trap = matches[1];
            logln("trap: " + trap);
        }
        if (Line.includes("CONTEXT: ")) {
            var matches = Line.match(/CONTEXT:.*([a-fA-F0-9]{16}) /);
            var cxr = matches[1];
            logln("cxr: " + cxr);
        }
        if (Line.includes("LOCK_ADDRESS: ")) {
            var matches = Line.match(/LOCK_ADDRESS:.*([a-fA-F0-9]{16}) /);
            var lock = matches[1];
            logln("lock: " + lock);
        }
        if (Line.includes("SYMBOL_NAME: ")) {
            //var matches = Line.match(/SYMBOL_NAME:.*([a-fA-F0-9!+]{8})/);
            var matches = Line.match(/SYMBOL_NAME:\s*(.*)/);
            var symbol_name = matches[1];
            logln("symbol_name: " + symbol_name);
        }
        
        //if (Line.includes("Loading Dump File")) {
        //    //Loading Dump File [D:\Intel_Dev\Dumps\BC_A\MEMORY_TIN194703ES277_2020-02-03_17-41-15.DMP]
        //    var matches = Line.match(/Loading Dump File [(.*)]/);
        //    var file = matches[1];
        //    logln("dump file: " + file);
        //    var logfile = path + file;
        //    logln("log file: " logfile);
        //}
        
        
    }
    if (index != null && index.length > 0) {
        dump_maps.get(index).bucket         = bucket;
        dump_maps.get(index).module         = module;
        dump_maps.get(index).thread         = thread;
        dump_maps.get(index).trap           = trap;
        dump_maps.get(index).cxr            = cxr;
        dump_maps.get(index).lock           = lock;
        dump_maps.get(index).symbol_name    = symbol_name;
        Dispatch(index, Args);
    } else {
        logln("dump type not supported: " + index);
    }
}

function Get_Pointer(addr, struct, member) {
    var ptr = undefined;
    exec("r $t0 = " + addr);
    exec("r $t0 = @@c++(((" + struct + " *)@$t0))");
    exec("r $t1 = @@c++(((" + struct + " *)@$t0)->" + member + ")");
    for (let Line of exec("? @$t1")) { ptr = Line.match(addrex)[1]; }
    var matches = ptr.match(addrex4); // normalize pointer - remove `
    if (matches) {
        ptr = matches[1] + matches[2];
    }
    return ptr;
}

function Get_Value(addr, struct, member) {
    var val = undefined;
    exec("r $t0 = " + addr);
    exec("r $t0 = @@c++(((" + struct + " *)@$t0))");
    exec("r $t1 = @@c++(((" + struct + " *)@$t0)->" + member + ")");
    for (let Line of exec("? @$t1")) { 
        val = Line.match(addrex)[1]; 
    }
    return val;
}

function Get_Field_Offset(addr, struct, member) {
    var field_ptr = undefined;
    exec("r $t2 = @@c++(" + addr + "+@@c++(#FIELD_OFFSET(" + struct + ", " + member + ")))");
    for (let Line of exec("? @$t2")) { 
        field_ptr = Line.match(addrex)[1]; 
    }
    return field_ptr;
}

function VIDEO_SCHEDULER_INTERNAL_ERROR_119(Args){
    logln(this.signature + " ***> VIDEO_SCHEDULER_INTERNAL_ERROR <***");
    logln("bucket:      " + this.bucket);
    logln("symbol name: " + "\'" + this.symbol_name + "\'");
    logln("The video scheduler has detected that fatal violation has occurred. This resulted");
    logln("in a condition that video scheduler can no longer progress. Any other values after");
    logln("parameter 1 must be individually examined according to the subtype.");
    logln("Arguments:");
    var retval = false;
    var watchdog_subcode = parseInt(Args[0]);
    //logln("subcode: " + String(watchdog_subcode));
    if (watchdog_subcode == 2) {
        retval = true;

        logln('Arg1: ' + Args[0] + ', The driver failed upon the submission of a command.');
        logln('Arg2: ' + Args[1] + ', Error status');
        logln('Arg3: ' + Args[2] + ', ???');
        logln('Arg4: ' + Args[3] + ', memory causing the error');

        var error_status = Args[1];
        var mem          = Args[3];

        var non_paged = false;
        for (let Line of exec("!pool " + mem + " 1")) {
            if (Line.includes('Nonpaged pool')) {
                non_paged = true;
            }
        }

        for (let Line of exec("!pte " + mem)) {
            if (Line.includes('contains')) {
                var matches = Line.match(/contains .* contains .* contains .* contains (.*)/);
                var pte = matches[1];
                if ((pte === "0000000000000000") && (non_paged == true)){
                    logln('');
                    logln("memory " + mem + " is corrupt - marked as non_paged and pte = " + pte);
                    logln('');
                    spew("!pool " + mem + " 1");
                    spew("!pte " + mem);
                    logln('');
                }
            }
        }

        spew("r");
        spew("u " + this.symbol_name);
        spew("ub " + this.symbol_name);

        var thread;
        for (let Line of exec(".thread")) { 
            thread = Line.match(addrex2)[1]; 
        }
        spew("!thread " + thread);
    }
    return retval;
}

function DRIVER_POWER_STATE_FAILURE_9F(Args){
    logln(this.signature + " ***> DRIVER_POWER_STATE_FAILURE <***");
    logln("bucket: " + this.bucket);
    var watchdog_subcode = parseInt(Args[0]);
    var retval = false;
    //logln("subcode: " + String(watchdog_subcode));
    if (watchdog_subcode == 3) {
        retval = true;
        logln('Arg1: ' + Args[0] + ', A device object has been blocking an Irp for too long a time');
        logln('Arg2: ' + Args[1] + ', Physical Device Object of the stack');
        logln('Arg3: ' + Args[2] + ', nt!TRIAGE_9F_POWER on Win7 and higher, otherwise the Functional Device Object of the stack');
        logln('Arg4: ' + Args[3] + ', The blocked IRP');

        var pdo         = Args[1];
        var triage_9f   = Args[2];
        var irp         = new IRP(Args[3]);

        if (irp.pending.includes("01")) {
            logln("the power IRP for device is pending - bucket: " + this.bucket);
        }
        spew('!irp ' + irp.addr);
        spew('!devstack ' + pdo);
    }
    return retval;
}

function CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP_15F(Args){ 
    logln(this.signature + " ***> CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP <***");
    logln("bucket: " + this.bucket);
    dumpargs(Args);
    var watchdog_subcode = parseInt(Args[0]);
    var retval = false;
    //logln("subcode: " + String(watchdog_subcode));
    if (watchdog_subcode == 2) {
        retval = true;
        logln('Arg1: ' + Args[0]);
        logln("the resiliency phase of connected standby for too long without");
        logln("entering DRIPS (deepest runtime idle platform state) due to an");
        logln("unsatisfied device constraint with no activators active.");
        logln('Arg2: ' + Args[1] + ', nt!_TRIAGE_POP_FX_DEVICE Device');
        logln('Arg3: ' + Args[2] + ', Component index');
        logln('Arg4: ' + Args[3] + ', Reserved => _TRIAGE_DEVICE_NODE');

        var irp = new IRP(Get_Pointer(Args[1], "nt!_TRIAGE_POP_FX_DEVICE", "Irp"));

        // Get the ServiceName -- this one is a bit odd
        var devnode = Get_Pointer(Args[1], "nt!_TRIAGE_POP_FX_DEVICE", "DeviceNode");
        spew2('nt!_TRIAGE_POP_FX_DEVICE', devnode);

        // make this into a function
        var field = Get_Field_Offset('0x'+devnode, 'nt!_TRIAGE_DEVICE_NODE', 'ServiceName');
        var driver_name = grep('dt _UNICODE_STRING ' + field, "Buffer", bufex);
        //logln('name: ' + driver_name);

        // get the PDO -> _TRIAGE_DEVICE_NODE->PhysicalDeviceObject
        var pdo = Get_Pointer(Args[3], "nt!_TRIAGE_DEVICE_NODE", "PhysicalDeviceObject");

        if (irp.pending.includes("01")) { // == true) {
            logln("pending returned for irp: " + irp.addr + " on driver: " + driver_name + ", pdo: " + pdo);
        }
        spew('!irp ' + irp.addr);
        logln("get the PDO stack");
        spew('!devstack ' + pdo);
        spew2('nt!_DEVICE_OBJECT', pdo);
    }
    return retval;
}

function DPC_WATCHDOG_VIOLATION_133(Args){
    logln(this.signature + " ***> DPC_WATCHDOG_VIOLATION <***");
    logln("The DPC watchdog detected a prolonged run time at an IRQL of DISPATCH_LEVEL or above.");
    logln("bucket: " + this.bucket);
    var watchdog_subcode = parseInt(Args[0]);
    var retval = false;
    //logln("subcode: " + String(watchdog_subcode));
    if (watchdog_subcode == 1) {
        retval = true;
        logln('Arg1: ' + Args[0] + ', The system cumulatively spent an extended period of time at');
        logln('                        DISPATCH_LEVEL or above. The offending component can usually be');
        logln('                        identified with a stack trace.');
        logln('Arg2: ' + Args[1] + ', The watchdog period.');
        logln('Arg3: ' + Args[2] + ', cast to nt!DPC_WATCHDOG_GLOBAL_TRIAGE_BLOCK, more info');
        logln('Arg4: ' + Args[3] + ', ???');

        var watchdog_triage = Args[2];

        //for (let Line of exec("dt " + watchdog_triage + " nt!_DPC_WATCHDOG_GLOBAL_TRIAGE_BLOCK")) {
        //    logln(Line);
        //}
        
        // get the main thread
        var thread;
        for (let Line of exec(".thread")) { 
            thread = Line.match(addrex2)[1]; 
        }
        // Look at thread and find any IRPs and module matches
        var has_irps = false;
        var irp_array = new Array(); // array of IRP structs
        for (let Line of exec("!thread " + thread)) {
            if (has_irps == true) {
                if (Line.includes("Mdl:")) {
                    var irp_addr = Line.match(addrex3)[1];
                    var irp = new IRP(irp_addr);
                    irp_array.push(irp); //(irp_addr);
                    continue;
                } else {
                    has_irps = false;
                }
            }
            if (Line.includes("IRP List:")) {
                has_irps = true;
                continue;
            }
            if (Line.includes(this.module)) {
                logln("(kv) line of interest" + Line);
            }
        }
        for (var irp_entry of irp_array) {
            for (let Line of exec("!irp " + irp_entry.addr)) { 
                logln(Line);
            }
            for (let Line of exec("dt nt!_IO_STACK_LOCATION " + irp_entry.current)) { 
                logln(Line);
            }
        }
        logln("");
        // dump out and identify DPCs of interest
        for (let Line of exec("!dpcs")) {
            if (Line.includes(this.module)) { 
                logln("(!dpcs) DPC of interest: " + Line);
            }
        }
    }
    return retval;
}

// $DumpFile = "D:\Intel_Dev\Dumps\BC_101\MEMORY_ZAC214703EV1_2022-01-31_12-33-21\MEMORY_ZAC214703EV1_2022-01-31_12-33-21.DMP"
// PROCESS_NAME:  SgrmBroker.exe
// System Guard Runtime Monitor Broker (SgrmBroker) is a Windows Service running and part of the Windows Defender System Guard.
// FAULTING_THREAD:  ffffd086d97f3080
// 
// STACK_TEXT:  
// ffff8089`7e36f598 fffff803`3a0207db     : ffffd086`c6d83000 ffff8089`7e36f830 00000000`00000000 fffff803`3a868b40 : 0xfffff803`357b001c
// ffff8089`7e36f5a0 fffff803`39f3734c     : 00000000`00000001 ffff8089`7e36f830 ffffd086`d97f3080 ffffd086`d97f4080 : nt!HvlSwitchToVsmVtl1+0xab
// ffff8089`7e36f6e0 fffff803`3a487472     : ffff8089`7e36f970 ffffd086`d97f3080 ffff8089`7e36f940 00000000`00000000 : nt!VslpEnterIumSecureMode+0x168
// ffff8089`7e36f7b0 fffff803`3a5d1f8a     : ffffffff`ffffffff 00000000`00000000 ffff8089`7e36faa0 ffffffff`ffffffff : nt!VslCallEnclave+0x168
// ffff8089`7e36f8f0 fffff803`3a02dac8     : 00000000`00000000 00000000`00000001 00000000`00000000 00000094`020ff5f0 : nt!PsCallEnclave+0x48a
// ffff8089`7e36f9f0 fffff803`3a036e65     : 0000022c`083536c0 ffffd086`c6d25400 0000022c`0835b110 ffffd086`00000000 : nt!NtCallEnclave+0x38
// ffff8089`7e36fa20 00007ffd`96266a91     : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000000 : nt!KiSystemServiceCopyEnd+0x25
// 00000094`020ff5c0 00000000`00000000     : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000000 : 0x00007ffd`96266a91
// 
// STACK_COMMAND:  .thread 0xffffd086d97f3080 ; kb
// SYMBOL_NAME:  nt!HvlSwitchToVsmVtl1+ab
//
// Check out: vsm_communication_signed.pdf
// MSR?? = KiSystemCall64
// Dump MSRS? -> rMFF dumps all registers
//

function CLOCK_WATCHDOG_TIMEOUT_101(Args) {
    logln(this.signature + " ***> CLOCK_WATCHDOG_TIMEOUT <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The CLOCK_WATCHDOG_TIMEOUT bug check has a value of 0x00000101. This indicates");
    logln("that an expected clock interrupt on a secondary processor, in a multi-processor");
    logln("system, was not received within the allocated interval.");
    logln("");
    logln('Arg1: ' + Args[0] + ', Clock interrupt time-out interval, in nominal clock ticks');
    logln('Arg2: ' + Args[1] + ', 0');
    logln('Arg3: ' + Args[2] + ', The address of the processor control block (PRCB) for the unresponsive processor');
    logln('Arg4: ' + Args[3] + ', faulting processor');
    logln("");

    var retval = true;
    var clock_ticks = Args[0];
    var prcb        = Args[2];
    var cpu         = Args[3];

    for (let Line of exec("!pcr " + cpu)) {
        logln("pcr: " + Line);
    }

    for (let Line of exec("!prcb " + cpu)) {
        logln("prcb: " + Line);
    }
    
    for (let Line of exec("!thread " + this.thread)) {
        logln("thread: " + Line);
    }
    return retval;
}

function INTERRUPT_EXCEPTION_NOT_HANDLED_3D(Args){ 
    logln(this.signature + " ***> INTERRUPT_EXCEPTION_NOT_HANDLED <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The INTERRUPT_EXCEPTION_NOT_HANDLED bug check has a value of 0x0000003D.");
    logln("This indicates that the exception handler for the kernel interrupt object");
    logln("interrupt management was not able to handle the generated exception.");
    logln("");
    logln('Arg1: ' + Args[0] + ', Exception Record (When Available)');
    logln('Arg2: ' + Args[1] + ', Context Record (When Available)');
    logln('Arg3: ' + Args[2] + ', 0');
    logln('Arg4: ' + Args[3] + ', 0');
    logln("");

    var retval = false;
    var exr = new EXR(Args[0]);
    var cxr = Args[1];
    var thread = null;
    var ExceptionCode = null;
    var ExceptionAddress = null;

    retval = true;
    for (let Line of exec("u " + exr.addr)) { 
        logln("u: " + Line);
    }

    for (let Line of exec(".thread")) { 
        thread = Line.match(addrex2)[1]; 
    }

    for (let Line of exec('.cxr ' + cxr)) {
        logln("cxr: " + Line);
    }

    for (let Line of exec('!thread ' + thread)) {
        logln("thread: " + Line);
        if (Line.includes(exr.module)) {
            logln("(kv) line of interest" + Line);
        }
    }
    return retval;
}

function IRQL_NOT_LESS_OR_EQUAL_A(Args){
    logln(this.signature + " ***> IRQL_NOT_LESS_OR_EQUAL <***");
    logln("bucket: " + this.bucket);
    logln("trap frame: " + this.trap);
    logln("");
    logln("An attempt was made to access a pageable (or completely invalid) address at an");
    logln("interrupt request level (IRQL) that is too high.  This is usually");
    logln("caused by drivers using improper addresses.");
    logln("If a kernel debugger is available get the stack backtrace");
    logln("");
    logln('Arg1: ' + Args[0] + ', memory referenced');
    logln('Arg2: ' + Args[1] + ', IRQL');
    logln('Arg3: ' + Args[2] + ', bitfield :');
    logln("    bit 0 : value 0 = read operation, 1 = write operation");
    logln("    bit 3 : value 0 = not an execute operation, 1 = execute operation (only on chips which support this level of status)");
    logln('Arg4: ' + Args[3] + ', address which referenced memory');
    logln("");
    
    for (let Line of exec('.trap ' + this.trap)) {
        logln("trap: " + Line);
    }

    var thread;
    for (let Line of exec(".thread")) { 
        thread = Line.match(addrex2)[1]; 
    }

    // look at the handle of the invalid memory
    for (let Line of exec('!handle ' + Args[0])) {
        logln("handle: " + Line);
    }
    
    for (let Line of exec('!thread ' + thread)) {
        logln("thread: " + Line);
    }

    return true;
}

function MEMORY_MANAGEMENT_1A(Args){
    logln(this.signature + " ***> MEMORY_MANAGEMENT <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The MEMORY_MANAGEMENT bug check has a value of 0x0000001A. This indicates that");
    logln("a severe memory management error occurred.");
    logln("");
    logln('Arg1: ' + Args[0] + ', Subtype of bugcheck.');
    logln(" refer to: https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/bug-check-0x1a--memory-management");
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}
function KMODE_EXCEPTION_NOT_HANDLED_1E(Args){
    logln(this.signature + " ***> KMODE_EXCEPTION_NOT_HANDLED <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The KMODE_EXCEPTION_NOT_HANDLED bug check has a value of 0x0000001E. This indicates that");
    logln("a kernel-mode program generated an exception that the error handler did not catch.");
    logln("");
    logln('Arg1: ' + Args[0] + ', The exception code that was not handled.');
    logln('Arg2: ' + Args[1] + ', The address at which the exception occurred.');
    logln('Arg3: ' + Args[2] + ', Parameter 0 of the exception.');
    logln('Arg4: ' + Args[3] + ', Parameter 1 of the exception.');
    logln("");

    return true;
}
function SYSTEM_SERVICE_EXCEPTION_3B(Args){              
    logln(this.signature + " ***> SYSTEM_SERVICE_EXCEPTION <***");
    logln("bucket: " + this.bucket);
    logln("context record: " + this.cxr);

    logln("The SYSTEM_SERVICE_EXCEPTION bug check has a value of 0x0000003B. This indicates");
    logln("that an exception happened while executing a routine that transitions from");
    logln("non-privileged code to privileged code.");
    logln("");
    logln('Arg1: ' + Args[0] + ', The exception that caused the bug check.');
    logln('Arg2: ' + Args[1] + ', The address of the instruction that caused the bug check');
    logln('Arg3: ' + Args[2] + ', The address of the context record for the exception that caused the bug check');
    logln('Arg4: ' + Args[3] + ', 0');
    logln("");
    var cxr = new CXR(this.cxr);
    var thread;
    for (let Line of exec(".thread")) { 
        thread = Line.match(addrex2)[1]; 
    }
    spew('!thread ' + thread);

    return true;
}

function PAGE_FAULT_IN_NONPAGED_AREA_50(Args){
    logln(this.signature + " ***> PAGE_FAULT_IN_NONPAGED_AREA <***");
    logln("bucket: " + this.bucket);
    logln("trap: " + this.trap);
    logln("lock: " + this.lock);

    logln("The PAGE_FAULT_IN_NONPAGED_AREA bug check has a value of 0x00000050. This indicates");
    logln("that invalid system memory has been referenced. Typically the memory address is");
    logln("wrong or the memory address is pointing at freed memory.");
    logln("");
    logln('Arg1: ' + Args[0] + ', Memory address referenced');
    logln('Arg2: ' + Args[1] + ', Operation');
    logln('Arg3: ' + Args[2] + ', Address that referenced the memory (if known)');
    logln('Arg4: ' + Args[3] + ', Type of fault');
    logln("");
    if (this.trap) { spew('.trap ' + this.trap); }
    if (this.lock) { spew('!locks ' + this.lock); }
    var thread;
    for (let Line of exec(".thread")) {
        thread = Line.match(addrex2)[1]; 
    }
    spew('!thread ' + thread);
    spew('!address ' + Args[2]);

    return true;
}
function SYSTEM_THREAD_EXCEPTION_NOT_HANDLED_7E(Args){   
    logln(this.signature + " ***> SYSTEM_THREAD_EXCEPTION_NOT_HANDLED <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("This is a very common bugcheck.  Usually the exception address pinpoints");
    logln("the driver/function that caused the problem.  Always note this address");
    logln("as well as the link date of the driver/image that contains this address.");
    logln("");
    logln('Arg1: ' + Args[0] + ', The exception code that was not handled');
    logln('Arg2: ' + Args[1] + ', The address that the exception occurred at');
    logln('Arg3: ' + Args[2] + ', Exception Record Address');
    logln('Arg4: ' + Args[3] + ', Context Record Address');
    logln("");
    var exr = new EXR(Args[2]);
    var cxr = new CXR(Args[3]);

    var thread;
    for (let Line of exec(".thread")) { 
        thread = Line.match(addrex2)[1]; 
    }
    spew('!thread ' + thread);

    return true;
}
function INTERNAL_POWER_ERROR_A0(Args){
    logln(this.signature + " ***> INTERNAL_POWER_ERROR <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The INTERNAL_POWER_ERROR bug check has a value of 0x000000A0. This bug check");
    logln("indicates that the power policy manager experienced a fatal error.");
    logln("");
    logln('Arg1: ' + Args[0] + ', ');
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}
function DRIVER_IRQL_NOT_LESS_OR_EQUAL_D1(Args){
    logln(this.signature + " ***> DRIVER_IRQL_NOT_LESS_OR_EQUAL <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The DRIVER_IRQL_NOT_LESS_OR_EQUAL bug check has a value of 0x000000D1. This");
    logln("indicates that a kernel-mode driver attempted to access pageable memory while");
    logln("the process IRQL that was too high.");
    logln("");
    logln('Arg1: ' + Args[0] + ', ');
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}
function VIDEO_TDR_FAILURE_116(Args){
    logln(this.signature + " ***> VIDEO_TDR_FAILURE <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The VIDEO_TDR_FAILURE bug check has a value of 0x00000116. This indicates that");
    logln("an attempt to reset the display driver and recover from a timeout failed.");
    logln("");
    logln('Arg1: ' + Args[0] + ', ');
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}
function VIDEO_TDR_TIMEOUT_DETECTED_117(Args){
    logln(this.signature + " ***> VIDEO_TDR_TIMEOUT_DETECTED <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The VIDEO_TDR_TIMEOUT_DETECTED live dump has a value of 0x00000117. This");
    logln("indicates that the display driver failed to respond in a timely fashion.");
    logln("");
    logln('Arg1: ' + Args[0] + ', ');
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}
function LKR_WHEA_UNCORRECTABLE_ERROR_124(Args){
    logln(this.signature + " ***> LKR_WHEA_UNCORRECTABLE_ERROR <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The WHEA_UNCORRECTABLE_ERROR means that a hardware error has occurred.");
    logln("");
    logln('Arg1: ' + Args[0] + ', ');
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}
function KERNEL_SECURITY_CHECK_FAILURE_139(Args){
    logln(this.signature + " ***> KERNEL_SECURITY_CHECK_FAILURE <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The KERNEL_SECURITY_CHECK_FAILURE bug check has a value of 0x00000139.");
    logln("This bug check indicates that the kernel has detected the corruption of");
    logln("a critical data structure.");
    logln("");
    logln('Arg1: ' + Args[0] + ', ');
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}
function KERNEL_MODE_HEAP_CORRUPTION_13A(Args){
    logln(this.signature + " ***> KERNEL_MODE_HEAP_CORRUPTION <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The KERNEL_MODE_HEAP_CORRUPTION bug check has a value of 0x0000013A. This");
    logln("indicates that the kernel mode heap manager has detected corruption in a heap.");
    logln("");
    logln('Arg1: ' + Args[0] + ', ');
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}
function VIDEO_ENGINE_TIMEOUT_DETECTED_141(Args){
    logln(this.signature + " ***> VIDEO_ENGINE_TIMEOUT_DETECTED <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The VIDEO_ENGINE_TIMEOUT_DETECTED live dump has a value of 0x00000141. This");
    logln("indicates that one of the display engines failed to respond in timely fashion.");
    logln("");
    logln('Arg1: ' + Args[0] + ', ');
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}
function LKR_BUGCODE_NDIS_DRIVER_LIVE_DUMP_15E(Args){
    logln(this.signature + " ***> LKR_BUGCODE_NDIS_DRIVER_LIVE_DUMP <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The BUGCODE_NDIS_DRIVER_LIVE_DUMP bug code has a value of 0x0000015E. This bug");
    logln("code indicates that NDIS has captured a live kernel dump. NDIS does not generate");
    logln("a bug check in this situation.");
    logln("");
    logln('Arg1: ' + Args[0] + ', ');
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}
function LKR_WIN32K_CRITICAL_FAILURE_164(Args){
    logln(this.signature + " ***> LKR_WIN32K_CRITICAL_FAILURE <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The WIN32K_CRITICAL_FAILURE bug check has a value of 0x00000164. This indicates");
    logln("that Win32k has encountered a critical failure.");
    logln("");
    logln('Arg1: ' + Args[0] + ', ');
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}
function VIDEO_DXGKRNL_LIVEDUMP_193(Args){
    logln(this.signature + " ***> VIDEO_DXGKRNL_LIVEDUMP <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The VIDEO_DXGKRNL_LIVEDUMP live dump has a value of 0x00000193. This indicates");
    logln("a livedump triggered by dxgkrnl occurred.");
    logln("");
    logln('Arg1: ' + Args[0] + ', Reason Code 0x100 Internal');
    logln('Arg2: ' + Args[1] + ', Reserved');
    logln('Arg3: ' + Args[2] + ', Reserved');
    logln('Arg4: ' + Args[3] + ', Reserved');
    logln("");

    return true;
}
function WIN32K_POWER_WATCHDOG_TIMEOUT_19C(Args){
    logln(this.signature + " ***> WIN32K_POWER_WATCHDOG_TIMEOUT <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The WIN32K_POWER_WATCHDOG_TIMEOUT bug check has a value of 0x0000019C. This indicates");
    logln("that Win32k did not turn the monitor on in a timely manner.");
    logln("");
    logln('Arg1: ' + Args[0] + ', ');
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}
function LKR_WIN32K_CALLOUT_WATCHDOG_LIVEDUMP_1A1(Args){
    logln(this.signature + " ***> LKR_WIN32K_CALLOUT_WATCHDOG_LIVEDUMP <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The WIN32K_CALLOUT_WATCHDOG_LIVEDUMP live dump has a value of 0x000001A1.");
    logln("A callout to Win32k did not return promptly.");
    logln("");
    logln('Arg1: ' + Args[0] + ', ');
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}
function MANUALLY_INITIATED_POWER_BUTTON_HOLD_1C8(Args){
    logln(this.signature + " ***> MANUALLY_INITIATED_POWER_BUTTON_HOLD <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The system was configured to initiate a bugcheck when the user holds the");
    logln("power button for a specified length of time. This is a diagnostic bugcheck");
    logln("used to capture a dump when the system is about to be hard reset with a");
    logln("long power button hold.");
    logln("");
    logln('Arg1: ' + Args[0] + ', ');
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}
function LKR_EXRESOURCE_TIMEOUT_LIVEDUMP_1CC(Args){
    logln(this.signature + " ***> LKR_EXRESOURCE_TIMEOUT_LIVEDUMP <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The EXRESOURCE_TIMEOUT_LIVEDUMP live dump has a value of 0x000001CC.");
    logln("A kernel ERESOURCE has timed out. This can indicate a deadlock condition");
    logln("or heavy contention which can cause performance issues.");
    logln("");
    logln('Arg1: ' + Args[0] + ', ');
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}
function UCMUCSI_LIVEDUMP_1D4(Args){
    logln(this.signature + " ***> UCMUCSI_LIVEDUMP <***");
    logln("bucket: " + this.bucket);
    logln("");
    logln("The UCMUCSI_LIVEDUMP live dump has a value of 0x000001D4. This indicates that");
    logln("the UcmUcsi class extension has encountered an error. For example this can be");
    logln("because a UCSI command has timed out, or because a UCSI command execution");
    logln("failed because the client driver returned failure.");
    logln("The UcmUcsiCx.sys is the included UCSI Class Extension. For more information,");
    logln("see USB Type-C Connector System Software Interface (UCSI) driver.");
    logln("");
    logln('Arg1: ' + Args[0] + ', ');
    logln('Arg2: ' + Args[1] + ', ');
    logln('Arg3: ' + Args[2] + ', ');
    logln('Arg4: ' + Args[3] + ', ');
    logln("");

    return true;
}

//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~ utilities ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//

function utils_init() {
    exec = host.namespace.Debugger.Utility.Control.ExecuteCommand;
}
 
function logln (e) { host.diagnostics.debugLog(e + '\n'); }
function spew (e) { for(let Line of exec(e)) { logln(Line) } }

function spew2 (a, b) { 
    logln("spew2: " + a + ' ' + b);
    for(let Line of exec('dt ' + a + ' ' + b)) { logln(Line) } 
}

function dumpargs (Args) { for(let i = 0; i < Args.length; i++) { logln('arg: ' + Args[i]); } }

function find(cmd, pat) { 
    for(let Line of exec(cmd)) { 
        if (Line.includes(pat)) { 
            return true; 
        } 
    } return false; 
}

function grep(cmd, pat, rex) {
    for(let Line of exec(cmd)) {
        if (Line.includes(pat)) { 
            var found = Line.match(bufex)[1];
            return found;
        }
    }
    return undefined; 
}

