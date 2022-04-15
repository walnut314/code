"use strict";

// File: eval2.js
// .scriptload C:\dev\Dumps\eval2.js
// .scriptunload eval2.js
// kd> dx Debugger.State.Scripts.eval2.Contents.EvalDump()

// TODO: function IRP analysis

let regex       = /Arg(\d): ([a-zA-Z0-9]{16})/;
let irpex       = /.* = ([a-zA-Z0-9]{8}`[a-zA-Z0-9]{8})/;
let addrex      = /.* = ([a-zA-Z0-9]{8}`[a-zA-Z0-9]{8})/;
let addrex2     = /([a-zA-Z0-9]{8}`[a-zA-Z0-9]{8})/;
let addrex3     = /([a-zA-Z0-9]{16})/;
let bufex       = undefined; ///(\".*\")/;
let exec        = undefined;
let logln       = undefined;
let spew        = undefined;
let spew2       = undefined;
let grep        = undefined;
let find        = undefined;
let dumpargs    = undefined;

function initializeScript(){}

function init() {
    host.namespace.Debugger.State.Scripts.utils.Contents.utils_init();
    logln    = host.namespace.Debugger.State.Scripts.utils.Contents.logln;
    spew     = host.namespace.Debugger.State.Scripts.utils.Contents.spew;
    spew2    = host.namespace.Debugger.State.Scripts.utils.Contents.spew2;
    dumpargs = host.namespace.Debugger.State.Scripts.utils.Contents.dumpargs;
    find     = host.namespace.Debugger.State.Scripts.utils.Contents.find;
    grep     = host.namespace.Debugger.State.Scripts.utils.Contents.grep;
    bufex    = new RegExp('(\".*\")');
}

function IRP(irp_addr) {
    // add analysis of IRP and stack
    //    device, current stack, thread, system buffer, IRP_MJ_XXX (IOCTL)
    this.addr   = irp_addr;
    this.stack  = null;
    this.irp_mj = null;
    this.thread = null;
    this.buffer = null;
}

function DumpFactory(signature, handler) { // creates a struct
    this.signature = signature; // string
    this.handler = handler;     // dump parser
    this.bucket = null;
    this.module = null;
}

var dump_maps = new Map();
dump_maps.set("(3D)",  new DumpFactory("(3D)",  INTERRUPT_EXCEPTION_NOT_HANDLED));
dump_maps.set("(9F)",  new DumpFactory("(9F)",  DRIVER_POWER_STATE_FAILURE));
dump_maps.set("(A0)",  new DumpFactory("(A0)",  INTERNAL_POWER_ERROR));
dump_maps.set("(D1)",  new DumpFactory("(D1)",  DRIVER_IRQL_NOT_LESS_OR_EQUAL));
dump_maps.set("(101)", new DumpFactory("(101)", CLOCK_WATCHDOG_TIMEOUT));
dump_maps.set("(119)", new DumpFactory("(119)", VIDEO_SCHEDULER_INTERNAL_ERROR));
dump_maps.set("(133)", new DumpFactory("(133)", DPC_WATCHDOG_VIOLATION));
dump_maps.set("(139)", new DumpFactory("(139)", KERNEL_SECURITY_CHECK_FAILURE));
dump_maps.set("(1D4)", new DumpFactory("(1D4)", UCMUCSI_LIVEDUMP));
dump_maps.set("(15F)", new DumpFactory("(15F)", CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP));

function Classify(line) {
    for (let [key, dumper] of dump_maps) { // makes case insensitive
        if (line.toLowerCase().includes(dumper.signature.toLowerCase())) {
            return dumper.signature;
        }
    }
    return null;
}

function Dispatch(idx, args) {
    exec(".logopen report.log")
    logln("***> Intel Dump Analyzer <***\n");
    spew("||")
    logln("");
    dump_maps.get(idx).handler(args);
    exec(".logclose")
}

// 2: kd> dx Debugger.State.Scripts.eval.Contents.EvalDump()
function EvalDump() {
    var Args = [];
    var index = null;
    exec = host.namespace.Debugger.Utility.Control.ExecuteCommand;
    init();

    var bucket = null;
    var module = null;
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
    }
    if (index.length > 0) {
        dump_maps.get(index).bucket = bucket;
        dump_maps.get(index).module = module;
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

function VIDEO_SCHEDULER_INTERNAL_ERROR(Args){
    logln(this.signature + " ***> VIDEO_SCHEDULER_INTERNAL_ERROR <***");
    logln("bucket: " + this.bucket);
    logln("The video scheduler has detected that fatal violation has occurred. This resulted");
    logln("in a condition that video scheduler can no longer progress. Any other values after");
    logln("parameter 1 must be individually examined according to the subtype.");
    logln("Arguments:");
    var watchdog_subcode = parseInt(Args[0]);
    //logln("subcode: " + String(watchdog_subcode));
    if (watchdog_subcode == 2) {
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

    }
}

function DRIVER_POWER_STATE_FAILURE(Args){
    logln(this.signature + " ***> DRIVER_POWER_STATE_FAILURE <***");
    logln("bucket: " + this.bucket);
    var watchdog_subcode = parseInt(Args[0]);
    //logln("subcode: " + String(watchdog_subcode));
    if (watchdog_subcode == 3) {
        logln('Arg1: ' + Args[0] + ', A device object has been blocking an Irp for too long a time');
        logln('Arg2: ' + Args[1] + ', Physical Device Object of the stack');
        logln('Arg3: ' + Args[2] + ', nt!TRIAGE_9F_POWER on Win7 and higher, otherwise the Functional Device Object of the stack');
        logln('Arg4: ' + Args[3] + ', The blocked IRP');

        var pdo         = Args[1];
        var triage_9f   = Args[2];
        var irp         = Args[3];

        var is_pending = Get_Value(irp, "nt!_IRP", "PendingReturned");
        //logln("pending: " + is_pending);
        if (is_pending.includes("01")) {
            logln("the power IRP for device is pending - bucket: " + this.bucket);
            spew('!irp ' + irp);
            spew('!devstack ' + pdo);
        }
    }
}

function CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP(Args){ 
    logln(this.signature + " ***> CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP <***");
    logln("bucket: " + this.bucket);
    dumpargs(Args);
    var watchdog_subcode = parseInt(Args[0]);
    //logln("subcode: " + String(watchdog_subcode));
    if (watchdog_subcode == 2) {
        logln('Arg1: ' + Args[0]);
        logln("the resiliency phase of connected standby for too long without");
        logln("entering DRIPS (deepest runtime idle platform state) due to an");
        logln("unsatisfied device constraint with no activators active.");
        logln('Arg2: ' + Args[1] + ', nt!_TRIAGE_POP_FX_DEVICE Device');
        logln('Arg3: ' + Args[2] + ', Component index');
        logln('Arg4: ' + Args[3] + ', Reserved => _TRIAGE_DEVICE_NODE');

        var irp = Get_Pointer(Args[1], "nt!_TRIAGE_POP_FX_DEVICE", "Irp");

        var is_pending = Get_Value(irp, "nt!_IRP", "PendingReturned");
        logln("pending: " + is_pending);

        // Get the ServiceName -- this one is a bit odd
        var devnode = Get_Pointer(Args[1], "nt!_TRIAGE_POP_FX_DEVICE", "DeviceNode");
        spew2('nt!_TRIAGE_POP_FX_DEVICE', devnode);
        
        // make this into a function
        var field = Get_Field_Offset('0x'+devnode, 'nt!_TRIAGE_DEVICE_NODE', 'ServiceName');
        var driver_name = grep('dt _UNICODE_STRING ' + field, "Buffer", bufex);
        logln('name: ' + driver_name);

        // get the PDO -> _TRIAGE_DEVICE_NODE->PhysicalDeviceObject
        var pdo = Get_Pointer(Args[3], "nt!_TRIAGE_DEVICE_NODE", "PhysicalDeviceObject");

        if (is_pending.includes("01")) { // == true) {
            logln("*** START REPORT ***");
            logln("pending returned for irp: " + irp + " on driver: " + driver_name);
            spew('!irp ' + irp);
            logln("get the PDO stack");
            spew2('nt!_DEVICE_OBJECT', pdo);
            logln("*** END REPORT ***");
        } else {
            logln("unknown reason for bugcheck -- needs manual analysis");
        }
    }
}

function DPC_WATCHDOG_VIOLATION(Args){
    logln(this.signature + " ***> DPC_WATCHDOG_VIOLATION <***");
    logln("The DPC watchdog detected a prolonged run time at an IRQL of DISPATCH_LEVEL or above.");
    logln("bucket: " + this.bucket);
    var watchdog_subcode = parseInt(Args[0]);
    //logln("subcode: " + String(watchdog_subcode));
    if (watchdog_subcode == 1) {
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
        }
        logln("");
        // dump out and identify DPCs of interest
        for (let Line of exec("!dpcs")) {
            if (Line.includes(this.module)) { 
                logln("(!dpcs) DPC of interest: " + Line);
            }
        }
    }
}

function CLOCK_WATCHDOG_TIMEOUT(Args) {
    logln(this.signature + " ***> CLOCK_WATCHDOG_TIMEOUT <***");
    dumpargs(Args);
    var clock_interrupt_timeout_interval_ticks = Args[0];
    var addr_of_prcb = Args[2];
    var processor = Args[1];
    logln("clock interval: " + clock_interrupt_timeout_interval_ticks);
    logln("prcb:           " + addr_of_prcb);
    logln("cpu:            " + processor);
    spew('!prcb ' + processor);
}

function INTERRUPT_EXCEPTION_NOT_HANDLED(Args){ logln(this.signature + " not implemented"); }
function INTERNAL_POWER_ERROR(Args){            logln(this.signature + " not implemented"); }
function DRIVER_IRQL_NOT_LESS_OR_EQUAL(Args){   logln(this.signature + " not implemented"); }
function KERNEL_SECURITY_CHECK_FAILURE(Args){   logln(this.signature + " not implemented"); }
function UCMUCSI_LIVEDUMP(Args){                logln(this.signature + " not implemented"); }

