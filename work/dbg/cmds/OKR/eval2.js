"use strict";

// File: eval2.js
// .scriptload C:\dev\Dumps\eval2.js
// .scriptunload eval2.js
// kd> dx Debugger.State.Scripts.eval2.Contents.EvalDump()

let regex = /Arg(\d): ([a-zA-Z0-9]{16})/;
let irpex = /.* = ([a-zA-Z0-9]{8}`[a-zA-Z0-9]{8})/;
let bufex = undefined; ///(\".*\")/;

let exec = undefined
let logln = undefined
let spew = undefined
let grep = undefined
let find = undefined
let dumpargs = undefined

function initializeScript(){}
function init() {
    host.namespace.Debugger.State.Scripts.utils.Contents.utils_init();
    logln    = host.namespace.Debugger.State.Scripts.utils.Contents.logln;
    spew     = host.namespace.Debugger.State.Scripts.utils.Contents.spew;
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
    //exec('.scriptload C:\\sandbox\\dump\\utils.js');
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

function CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP(Args){ 
    logln(this.signature + " ***> CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP <***");
    dumpargs(Args);
    var pending = false;
    var driver_name;
    var irp;
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

        exec("r $t0 = " + Args[1]);
        exec("r $t0 = @@c++(((nt!_TRIAGE_POP_FX_DEVICE *)@$t0))");
        exec("r $t1 = @@c++(((nt!_TRIAGE_POP_FX_DEVICE *)@$t0)->Irp)");
        exec("r $t7 = $t1"); // save Irp in $t7
        for (let Line of exec("? @$t1")) {
            irp = Line.match(irpex)[1];
        }
        logln(irp);
        exec("r $t2 = @@c++(((nt!_IRP *)@$t1)->PendingReturned)");
        if (find("? @$t2", "00000001")) {
            pending = true;
        }
        //if (exec("? @$t2") 
        //fx_dev = host.createPointerObject(Args[1], "ntoskrnl.exe", "nt!_TRIAGE_POP_FX_DEVICE");
        //
        spew('dt _TRIAGE_POP_FX_DEVICE @$t0');
        exec("r $t1 = @@c++(((nt!_TRIAGE_POP_FX_DEVICE *)@$t0)->DeviceNode)");
        exec("r $t2 = @@c++(@$t1+@@c++(#FIELD_OFFSET(_TRIAGE_DEVICE_NODE, ServiceName)))");
        driver_name = grep("dt _UNICODE_STRING @$t2", "Buffer", bufex);
        // get the PDO -> _TRIAGE_DEVICE_NODE->PhysicalDeviceObject
        exec("r $t0 = " + Args[3]);
        exec("r $t0 = @@c++(((nt!_TRIAGE_DEVICE_NODE *)@$t0))");
        exec("r $t1 = @@c++(((nt!_TRIAGE_DEVICE_NODE *)@$t0)->PhysicalDeviceObject)");
        if (pending == true) {
            logln("*** START REPORT ***");
            logln("pending returned for irp: " + irp + " on driver: " + driver_name);
            spew("!irp @$t7")
            logln("get the PDO stack");
            spew("!devstack @$t1");
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

