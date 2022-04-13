"use strict";

// File: eval2.js
// .scriptload C:\dev\Dumps\eval2.js
// .scriptunload eval2.js
// kd> dx Debugger.State.Scripts.eval2.Contents.EvalDump()

let logln = function (e) { host.diagnostics.debugLog(e + '\n'); }
let dumpargs = function(Args) { for(let i = 0; i < Args.length; i++) { logln('arg: ' + Args[i]); } }
let regex = /Arg(\d): ([a-zA-Z0-9]{16})/;
function initializeScript() {}

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
    for (let [key, dumper] of dump_maps) {
        logln("dumper: " + dumper.signature + " -> line: " + line);
        if (line.includes(dumper.signature.toLowerCase())) {
            logln("signature: " + dumper.signature + " -> line: " + line);
            return dumper.signature;
        }
    }
    return null;
}

// 2: kd> dx Debugger.State.Scripts.eval.Contents.EvalDump()
function EvalDump() {
    var Args = [];
    var index = null;
    let Exec = host.namespace.Debugger.Utility.Control.ExecuteCommand;
    logln("***> Intel Dump Analyzer! \n");
    Exec(".logappend analysis.log")
    Exec("||")
    for(let Line of Exec('!analyze -v')) {
        //logln(Line);
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
    Exec(".logclose")
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
    let Exec = host.namespace.Debugger.Utility.Control.ExecuteCommand;
    for(let Line of Exec('!prcb ' + processor)) {
        logln(Line);
    }
}

function CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP(Args){ 
    logln(this.signature + " ***> CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP <***");
    dumpargs(Args);
    var watchdog_subcode = parseInt(Args[0]);
    logln("subcode: " + String(watchdog_subcode));
    let Exec = host.namespace.Debugger.Utility.Control.ExecuteCommand;
    if (watchdog_subcode == 2) {
        Exec("r $t0 = " + Args[1]);
        Exec("r $t0 = @@c++(((nt!_TRIAGE_POP_FX_DEVICE *)@$t0))");
        Exec("r $t1 = @@c++(((nt!_TRIAGE_POP_FX_DEVICE *)@$t0)->Irp)");
        for(let Line of Exec("!irp @$t1")) {
            logln(Line)
        }
        //fx_dev = host.createPointerObject(Args[1], "ntoskrnl.exe", "nt!_TRIAGE_POP_FX_DEVICE");
        //for(let Line of Exec('dt _TRIAGE_POP_FX_DEVICE ' + Args[1])) {
        //    logln(Line);
        //}
    }
}

function INTERRUPT_EXCEPTION_NOT_HANDLED(Args){             logln(this.signature + " not implemented"); }
function DRIVER_POWER_STATE_FAILURE(Args){                  logln(this.signature + " not implemented"); }
function INTERNAL_POWER_ERROR(Args){                        logln(this.signature + " not implemented"); }
function DRIVER_IRQL_NOT_LESS_OR_EQUAL(Args){               logln(this.signature + " not implemented"); }
function DPC_WATCHDOG_VIOLATION(Args){                      logln(this.signature + " not implemented"); }
function KERNEL_SECURITY_CHECK_FAILURE(Args){               logln(this.signature + " not implemented"); }
function UCMUCSI_LIVEDUMP(Args){                            logln(this.signature + " not implemented"); }

