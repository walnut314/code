// C:\dev\Dumps\eval.js
"use strict";

// .scriptload C:\dev\Dumps\eval2.js
// .scriptunload eval2.js
// kd> dx Debugger.State.Scripts.eval2.Contents.EvalDump()

let logln = function (e) { host.diagnostics.debugLog(e + '\n'); }
let dumpargs = function(Args) { for(let i = 0; i < Args.length; i++) { logln('arg: ' + Args[i]); } }
let regex = /Arg(\d): ([a-zA-Z0-9]{16})/;
function initializeScript() {}

function DumpFactory(signature, code, handler) { // creates a struct
    this.signature = signature; // string
    this.code = code;           // hex
    this.handler = handler;     // dump parser
}

var dump_maps = new Map();
dump_maps.set("(3D)",  new DumpFactory("(3D)",  0x3d,  INTERRUPT_EXCEPTION_NOT_HANDLED));
dump_maps.set("(9F)",  new DumpFactory("(9F)",  0x9f,  DRIVER_POWER_STATE_FAILURE));
dump_maps.set("(A0)",  new DumpFactory("(A0)",  0xa0,  INTERNAL_POWER_ERROR));
dump_maps.set("(D1)",  new DumpFactory("(D1)",  0xd1,  DRIVER_IRQL_NOT_LESS_OR_EQUAL));
dump_maps.set("(101)", new DumpFactory("(101)", 0x101, CLOCK_WATCHDOG_TIMEOUT));
dump_maps.set("(139)", new DumpFactory("(139)", 0x139, KERNEL_SECURITY_CHECK_FAILURE));
dump_maps.set("(1D4)", new DumpFactory("(1D4)", 0x1d4, UCMUCSI_LIVEDUMP));

function Classify(line) {
    for (let [key, dumper] of dump_maps) {
        if (line.includes(dumper.signature)) {
            return dumper.signature;
        }
    }
}

// 2: kd> dx Debugger.State.Scripts.eval.Contents.EvalDump()
function EvalDump() {
    var Args = [];
    var index = null;
    let Exec = host.namespace.Debugger.Utility.Control.ExecuteCommand;
    logln("***> Hello Dump Analyzer! \n");
    for(let Line of Exec('!analyze -v')) {
        if (index == null) index = Classify(Line);
        if (Line.match(/Arg\d:/)) { 
            var matches = Line.match(regex);
            var idx = parseInt(matches[1]) - 1;
            Args[idx] = matches[2];
        }
    }
    if (index.length > 0) {
        dumpargs(Args);
        dump_maps.get(index).handler(Args);
    } else {
        logln("dump type not supported: " + index);
    }
}

function CLOCK_WATCHDOG_TIMEOUT(Args) {
    var clock_interrupt_timeout_interval_ticks = Args[0];
    var addr_of_prcb = Args[2];
    var processor = Args[3];
 
    logln("***> CLOCK_WATCHDOG_TIMEOUT <***");
    logln("clock interval: " + clock_interrupt_timeout_interval_ticks);
    logln("prcb:           " + addr_of_prcb);
    logln("cpu:            " + processor);

    let Exec = host.namespace.Debugger.Utility.Control.ExecuteCommand;
    for(let Line of Exec('!prcb ' + processor)) {
        logln(Line);
    }
}

function INTERRUPT_EXCEPTION_NOT_HANDLED(Args){ logln("not implemented"); }
function KERNEL_SECURITY_CHECK_FAILURE(Args){ logln("not implemented"); }
function UCMUCSI_LIVEDUMP(Args){ logln("not implemented"); }
function DRIVER_POWER_STATE_FAILURE(Args){ logln("not implemented"); }
function INTERNAL_POWER_ERROR(Args){ logln("not implemented"); }
function DRIVER_IRQL_NOT_LESS_OR_EQUAL(Args){ logln("not implemented"); }

