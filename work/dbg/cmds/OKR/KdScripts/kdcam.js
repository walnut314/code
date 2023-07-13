// File: kdcam.js
// .scriptload C:\dev\Dumps\kdcam.js
// .scriptunload kdcam.js
// kd> dx Debugger.State.Scripts.eval.Contents.KdCam()

let exec    = undefined;
let addrex  = /([a-fA-F0-9]{8}`[a-fA-F0-9]{8})/;

function initializeScript(){}
function utils_init() {
    exec = host.namespace.Debugger.Utility.Control.ExecuteCommand;
}

function KdCam() {
    utils_init();

    logln("dude, wusup");
    var val = undefined;
    for (let Line of exec('x iacamera64!used_mem_list')) { 
        //logln("Line: " + Line); 
        val = Line.match(addrex)[1]; 
        //logln("val: " + val); 
        for (let Line of exec('dt nt!_LIST_ENTRY ' + val)) { 
            //logln("Line: " + Line);
            if (Line.match('Flink')) {
                val = Line.match(addrex)[1];
                logln("Flink: " + val);
                //spew("!list -t nt!_LIST_ENTRY.Flink -e " + val); -- works
                //spew("!list -t nt!_LIST_ENTRY.Flink -e -x \"dt iacamera64!_memory_range\"" + val); -- works
                spew("!list -t nt!_LIST_ENTRY.Flink -e -x \"dt iacamera64!_memory_range @$extret host_addr page_addr actual_size\"" + val);

            }
        }
    }
    
}

function logln (e) { host.diagnostics.debugLog(e + '\n'); }
function spew (e) { for(let Line of exec(e)) { logln(Line) } }

