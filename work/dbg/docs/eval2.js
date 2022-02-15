// C:\dev\Dumps\eval.js
"use strict";

// .scriptload C:\dev\Dumps\eval.js
// .scriptunload eval.js
// kd> dx Debugger.State.Scripts.eval.Contents.EvalDump()

let logln = function (e) { host.diagnostics.debugLog(e + '\n'); }
let dumpargs = function(Args) { for(let i = 0; i < Args.length; i++) { logln('arg: ' + Args[i]); } }
var regex = /Arg(\d): ([a-zA-Z0-9]{16})/;
function initializeScript() {}

function DumpFactory(signature, code, handler) { // creates a struct
    this.signature = signature; // string
    this.code = code;           // hex
    this.handler = handler;     // dump parser
}

var DumpIndex = -1;
var dumps2 = new Map();
dumps2.set("(3D)", new DumpFactory("(3D)", 0x3d, INTERRUPT_EXCEPTION_NOT_HANDLED));
dumps2.set("(101)", new DumpFactory("(101)", 0x101, CLOCK_WATCHDOG_TIMEOUT));

function Classify(sline) {
    for (let [key, dumper] of dumps2) {
        if (sline.includes(dumper.signature)) {
            DumpIndex = dumper.signature;
        }
    }
}

// 2: kd> dx Debugger.State.Scripts.eval.Contents.EvalDump()
function EvalDump() {
    var Args = [];
    let Control = host.namespace.Debugger.Utility.Control;
    logln("***> Hello Dump Analyzer! \n");
    for(let Line of Control.ExecuteCommand('!analyze -v')) {
        var sline = new String(Line);
        var index = Classify(sline);
        if (sline.match(/Arg\d:/)) { 
            var matches = sline.match(regex);
            var idx = parseInt(matches[1]);
            Args[idx-1] = matches[2];
        }
    }
    if (DumpIndex.length > 0) {
        var dumper = dumps2.get(DumpIndex);
        dumpargs(Args);
        dumper.handler(Args);
    } else {
        logln("dump type not supported: " + String(DumpIndex));
    }
}

function CLOCK_WATCHDOG_TIMEOUT(Args) {
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/bug-check-0x101---clock-watchdog-timeout

    var clock_interrupt_timeout_interval_ticks = Args[0];
    var addr_of_prcb = Args[2];
    var processor = Args[3];
    
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

