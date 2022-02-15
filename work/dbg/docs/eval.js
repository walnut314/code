// C:\dev\Dumps\eval.js
"use strict";

// .scriptload C:\dev\Dumps\eval.js
// .scriptunload eval.js
// kd> dx Debugger.State.Scripts.eval.Contents.EvalDump()

let logln = function (e) { host.diagnostics.debugLog(e + '\n'); }
let dumpargs = function(Args) { for(let i = 0; i < Args.length; i++) { logln('arg: ' + Args[i]); } }
var DumpIndex = -1;
function initializeScript() {}

function DumpFactory(signature, code, handler) { // creates a struct
    this.signature = signature; // string
    this.code = code;           // hex
    this.handler = handler;     // dump parser
}

var dumps = [new DumpFactory("(3D)", 0x3d, INTERRUPT_EXCEPTION_NOT_HANDLED),
             new DumpFactory("(101)", 0x101, CLOCK_WATCHDOG_TIMEOUT) ];

function Classify(sline) {
    for (let i = 0; i < dumps.length; i++) {
        if (sline.includes(dumps[i].signature)) {
            DumpIndex = i;
            return i;
        }
    }
    return -1;
}

// 2: kd> dx Debugger.State.Scripts.eval.Contents.EvalDump()
function EvalDump() {
    var Args = [];
    let Control = host.namespace.Debugger.Utility.Control;
    logln("***> Hello Dump Analyzer! \n");
    for(let Line of Control.ExecuteCommand('!analyze -v')) {
        var sline = new String(Line);
        var index = Classify(sline);
        if      (sline.includes('Arg1')) { Args[0] = sline; }
        else if (sline.includes('Arg2')) { Args[1] = sline; }
        else if (sline.includes('Arg3')) { Args[2] = sline; }
        else if (sline.includes('Arg4')) { Args[3] = sline; }
    }
    if (DumpIndex >= 0) {
        dumpargs(Args);
        dumps[DumpIndex].handler(Args);
    } else {
        logln("dump type not supported: " + String(DumpIndex));
    }
}

function CLOCK_WATCHDOG_TIMEOUT(Args) {
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/bug-check-0x101---clock-watchdog-timeout
    var regex = /([a-zA-Z0-9]{16})/;
    var clock_interrupt_timeout_interval_ticks = Args[0].match(regex)[0];
    var addr_of_prcb = Args[2].match(regex)[0];
    var processor = Args[3].match(regex)[0];;

    logln("***> CLOCK_WATCHDOG_TIMEOUT <***");
    logln("clock interval: " + clock_interrupt_timeout_interval_ticks);
    logln("prcb:           " + addr_of_prcb);
    logln("cpu:            " + processor);
   
    let Control = host.namespace.Debugger.Utility.Control;
    for(let Line of Control.ExecuteCommand('!prcb ' + processor)) {
        logln(Line);
    }
}

function INTERRUPT_EXCEPTION_NOT_HANDLED(Args) {
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/bug-check-0x3d--interrupt-exception-not-handled
}

