"use strict";

// File: eval2.js
// .scriptload C:\dev\Dumps\eval2.js
// .scriptunload eval2.js
// kd> dx Debugger.State.Scripts.eval2.Contents.EvalDump()

let regex       = /Arg(\d): ([a-zA-Z0-9]{16})/;
let irpex       = /.* = ([a-zA-Z0-9]{8}`[a-zA-Z0-9]{8})/;
let addrex      = /.* = ([a-zA-Z0-9]{8}`[a-zA-Z0-9]{8})/;
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

function DumpFactory(signature, handler) { // creates a struct
    this.signature = signature; // string
    this.handler = handler;     // dump parser
}

var dump_maps = new Map();
dump_maps.set("(3D)",  new DumpFactory("(3D)",  INTERRUPT_EXCEPTION_NOT_HANDLED));
dump_maps.set("(9F)",  new DumpFactory("(9F)",  DRIVER_POWER_STATE_FAILURE));
dump_maps.set("(A0)",  new DumpFactory("(A0)",  INTERNAL_POWER_ERROR));
dump_maps.set("(D1)",  new DumpFactory("(D1)",  DRIVER_IRQL_NOT_LESS_OR_EQUAL));
dump_maps.set("(101)", new DumpFactory("(101)", CLOCK_WATCHDOG_TIMEOUT));
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

// 2: kd> dx Debugger.State.Scripts.eval.Contents.EvalDump()
function EvalDump() {
    var Args = [];
    var index = null;
    exec = host.namespace.Debugger.Utility.Control.ExecuteCommand;
    exec(".logopen report.log")
    init();
    logln("***> Intel Dump Analyzer! \n");

    spew("||")
    for(let Line of exec('!analyze -v')) {
        if (index == null) index = Classify(Line);
        if (Line.match(/Arg\d:/)) { 
            var matches = Line.match(regex);
            var idx = parseInt(matches[1]) - 1;
            Args[idx] = matches[2];
        }
    }
    if (index.length > 0) {
        dump_maps.get(index).handler(Args);
    } else {
        logln("dump type not supported: " + index);
    }
    exec(".logclose")
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

function CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP(Args){ 
    logln(this.signature + " ***> CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP <***");
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
        logln("irp: " + irp);

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

function INTERRUPT_EXCEPTION_NOT_HANDLED(Args){ logln(this.signature + " not implemented"); }
function DRIVER_POWER_STATE_FAILURE(Args){      logln(this.signature + " not implemented"); }
function INTERNAL_POWER_ERROR(Args){            logln(this.signature + " not implemented"); }
function DRIVER_IRQL_NOT_LESS_OR_EQUAL(Args){   logln(this.signature + " not implemented"); }
function DPC_WATCHDOG_VIOLATION(Args){          logln(this.signature + " not implemented"); }
function KERNEL_SECURITY_CHECK_FAILURE(Args){   logln(this.signature + " not implemented"); }
function UCMUCSI_LIVEDUMP(Args){                logln(this.signature + " not implemented"); }

