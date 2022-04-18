"use strict";

// File: eval2.js
// .scriptload C:\dev\Dumps\eval2.js
// .scriptunload eval2.js
// kd> dx Debugger.State.Scripts.eval2.Contents.EvalDump()

// TODO: function IRP analysis
// have each analyzer return bool success or not-implemented

let regex       = /Arg(\d): ([a-fA-F0-9]{16})/;
let irpex       = /.* = ([a-fA-F0-9]{8}`[a-fA-F0-9]{8})/;
let addrex      = /.* = ([a-fA-F0-9]{8}`[a-fA-F0-9]{8})/;
let addrex2     = /([a-fA-F0-9]{8}`[a-fA-F0-9]{8})/;
let addrex3     = /([a-fA-F0-9]{16})/;
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
    for(let Line of exec('!irp ' + irp_addr)) {
        logln("irp: " + Line);
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
    logln("irp.addr:    " + this.addr);
    logln("irp.current: " + this.current);
    logln("irp.irp_mj:  " + this.irp_mj);
    logln("irp.thread:  " + this.thread);
    logln("irp.buffer:  " + this.buffer);
    logln("irp.pending: " + this.pending);
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
        
    }
    if (index.length > 0) {
        dump_maps.get(index).bucket = bucket;
        dump_maps.get(index).module = module;
        dump_maps.get(index).thread = thread;
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
        var irp         = new IRP(Args[3]);

        if (irp.pending.includes("01")) {
            logln("the power IRP for device is pending - bucket: " + this.bucket);
            spew('!irp ' + irp.addr);
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

        var irp = new IRP(Get_Pointer(Args[1], "nt!_TRIAGE_POP_FX_DEVICE", "Irp"));

        // Get the ServiceName -- this one is a bit odd
        var devnode = Get_Pointer(Args[1], "nt!_TRIAGE_POP_FX_DEVICE", "DeviceNode");
        spew2('nt!_TRIAGE_POP_FX_DEVICE', devnode);

        // make this into a function
        var field = Get_Field_Offset('0x'+devnode, 'nt!_TRIAGE_DEVICE_NODE', 'ServiceName');
        var driver_name = grep('dt _UNICODE_STRING ' + field, "Buffer", bufex);
        logln('name: ' + driver_name);

        // get the PDO -> _TRIAGE_DEVICE_NODE->PhysicalDeviceObject
        var pdo = Get_Pointer(Args[3], "nt!_TRIAGE_DEVICE_NODE", "PhysicalDeviceObject");

        if (irp.pending.includes("01")) { // == true) {
            logln("*** START REPORT ***");
            logln("pending returned for irp: " + irp.addr + " on driver: " + driver_name);
            spew('!irp ' + irp.addr);
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
}

// $DumpFile = "D:\Intel_Dev\Dumps\BC_101\MEMORY_ZAC214703EV1_2022-01-31_12-33-21\MEMORY_ZAC214703EV1_2022-01-31_12-33-21.DMP"
// 004   !t ffffd086`c6c93040     0000    0    0     922917      1:01:56.062                0           12.218  R------ RUNNING  Exec                  (SpinLock)            nt!KeBugCheckEx+0x0
//
// 2: kd> !thread ffffd086`c6c93040
// THREAD ffffd086c6c93040  Cid 0000.0000  Teb: 0000000000000000 Win32Thread: 0000000000000000 RUNNING on processor 2
// Not impersonating
// DeviceMap                 ffffbf01b5a67c30
// Owning Process            fffff8033a947f40       Image:         Idle
// Attached Process          ffffd086c6ce8040       Image:         System
// Wait Start TickCount      309971         Ticks: 782 (0:00:00:12.218)
// Context Switch Count      922917         IdealProcessor: 2             
// UserTime                  00:00:00.000
// KernelTime                01:01:56.062
// Win32 Start Address nt!KiIdleLoop (0xfffff8033a0284b0)
// Stack Init ffff808978457bb0 Current ffff808978457b40
// Base ffff808978458000 Limit ffff808978451000 Call 0000000000000000
// Priority 0 BasePriority 0 PriorityDecrement 0 IoPriority 0 PagePriority 0
// Child-SP          RetAddr               : Args to Child                                                           : Call Site
// ffff8380`3abdac08 fffff803`3a080620     : 00000000`00000101 00000000`0000000c 00000000`00000000 ffff8380`3b458180 : nt!KeBugCheckEx
// ffff8380`3abdac10 fffff803`39e31ff6     : 00000000`00000000 00000000`00000000 ffff8380`3abc0180 00000000`00000000 : nt!KeAccumulateTicks+0x24cb90
// ffff8380`3abdac80 fffff803`39e6fd8b     : fffff780`00000000 fffff803`3a80b7c0 fffff7c6`0001ed00 00000000`00000010 : nt!KiUpdateRunTime+0x66
// ffff8380`3abdace0 fffff803`39e31da5     : fffff803`3a8408a8 00000000`00000000 ffff8380`3abc0180 00000000`00000000 : nt!KiUpdateTime+0x60b
// ffff8380`3abdaea0 fffff803`39e2842a     : fffff803`3a85f6f0 ffffd086`c6d23f50 ffffd086`c6d23f50 00000000`00000000 : nt!KeClockInterruptNotify+0x235
// ffff8380`3abdaf40 fffff803`39f2212b     : 0000000b`4f6fdfe6 ffffd086`c6d23ea0 00000000`00000001 00000000`00000000 : nt!HalpTimerClockInterrupt+0x10a
// ffff8380`3abdaf70 fffff803`3a0261ea     : ffff8089`784574d0 ffffd086`c6d23ea0 ffffd086`c6c93040 00000000`00000000 : nt!KiCallInterruptServiceRoutine+0x18b
// ffff8380`3abdafb0 fffff803`3a0267b7     : 00000000`00000000 00000000`00000000 ffff8089`78457650 fffff803`3a0267c4 : nt!KiInterruptSubDispatchNoLockNoEtw+0xfa (TrapFrame @ ffff8380`3abdae70)
// ffff8089`78457450 fffff803`39e795ac     : 00000000`00000010 00000000`00000286 ffff8089`78457600 00000000`00000018 : nt!KiInterruptDispatchNoLockNoEtw+0x37 (TrapFrame @ ffff8089`78457450)
// ffff8089`784575e0 fffff803`3a063228     : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000000 : nt!KeYieldProcessorEx+0x1c
// ffff8089`78457610 fffff803`3a063110     : 00000009`52d98539 fffff803`39fadb19 ffff8089`09576147 00000000`00000000 : nt!KxWaitForLockOwnerShipWithIrql+0x24
// ffff8089`78457640 fffff803`3a046d5d     : ffffd086`d1d99000 ffff8089`784577b9 fffff803`37e0df70 ffff8089`78457b00 : nt!KiAcquireQueuedSpinLockInstrumented+0x66
// ffff8089`78457680 fffff803`37e0dfc5     : fffff803`37e0df70 ffff8089`78457b00 ffffd086`c6c93040 ffffd086`c6c93040 : nt!KeAcquireInStackQueuedSpinLockAtDpcLevel+0x21ad9d
// ffff8089`784576b0 fffff803`39e6aada     : ffffd086`d15ac560 fffff803`00000001 00000000`00989680 ffff8089`784577b9 : dxgmms2!VidSchiWorkerThreadTimerCallback+0x55
// ffff8089`78457710 fffff803`39e6a504     : 00000000`00000003 ffffd086`d15a6ab8 ffffd086`d15ac560 ffffd086`c6c93040 : nt!KiExpireTimer2+0x42a
// ffff8089`78457820 fffff803`39e7b036     : 00000000`00000008 ffff8089`784579e0 00000000`00000021 ffff8380`3abc0180 : nt!KiTimer2Expiration+0x164
// ffff8089`784578e0 fffff803`3a02854e     : 00000000`00000000 ffff8380`3abc0180 ffffd086`c6c93040 ffffd086`d86e60c0 : nt!KiRetireDpcList+0xa76
// ffff8089`78457b80 00000000`00000000     : ffff8089`78458000 ffff8089`78451000 00000000`00000000 00000000`00000000 : nt!KiIdleLoop+0x9e
//
//     Active Threads on each processor:
//                                                              Cur  Bas   zContext      Kernel Time        User Time    Elapsed Ticks   Q-TSA   Thread    Wait                                      Waiting  | Overview | Start     
// PROC  Image Name        No.   Thread                    Id   Pri  Pri   Switches   d:hh:mm:ss.ttt   d:hh:mm:ss.ttt   d:hh:mm:ss.ttt  RrBLsrl   State  Reason  Notes        Waiting On             Function | Function | Function  
// 
// 0000  Idle              000   !t fffff8033a94b040     0000    0    0     920414      1:02:25.546                0         7:23.078  R------ RUNNING  Exec                  (CPU)                 nt!PpmIdleGuestExecute+0x1d
// 0000  Idle              001   !t ffffd086c6ca8040     0000    0    0     437982      1:04:30.421                0         9:34.468  R------ RUNNING  Exec                  (CPU)                 nt!PpmIdleGuestExecute+0x1d
// 0000  Idle              002   !t ffffd086c6c93040     0000    0    0     922917      1:01:56.062                0           12.218  R------ RUNNING  Exec                  (SpinLock)            nt!KeBugCheckEx+0x0
// 0000  Idle              003   !t ffffd086c6d1b040     0000    0    0     362866      1:04:27.140                0         9:31.765  R------ RUNNING  Exec                  (CPU)                 nt!PpmIdleGuestExecute+0x1d
// 0000  Idle              004   !t ffffd086c6d20040     0000    0    0     824559      1:02:21.718                0         7:17.375  R------ RUNNING  Exec                  (CPU)                 nt!PpmIdleGuestExecute+0x1d
// 0000  Idle              005   !t ffffd086c6d47080     0000    0    0     444634      1:04:16.515                0        14:12.625  R------ RUNNING  Exec                  (CPU)                 nt!PpmIdleGuestExecute+0x1d
// 0000  Idle              006   !t ffffd086c6d21040     0000    0    0     957990      1:01:32.890                0         9:40.796  R------ RUNNING  Exec                  (CPU)                 nt!PpmIdleGuestExecute+0x1d
// 0000  Idle              007   !t ffffd086c6d54040     0000    0    0     403097      1:04:49.375                0        13:12.343  R------ RUNNING  Exec                  (CPU)                 nt!PpmIdleGuestExecute+0x1d
// 1130  MsMpEng.exe         8   !t ffffd086d6248100     2ed0    9    8      14875             .843            1.515           12.359  R----r- RUNNING  PageIn.               (CPU)                 igdkmdn64+0x2513e9
// 10A0  svchost.exe         9   !t ffffd086d4e27080     3450    8    8       5958             .625             .906            4.562  R------ RUNNING  Exec                  (CPU)                 IntcAudioBus+0xf054
// 10E0  SurfaceMLServi     10   !t ffffd086d7e97080     3460    8    8        429            7.765             .250            5.812  R----r- RUNNING  Yield                 (CPU)                 nt!KeFlushProcessWriteBuffers+0xdc
// 0000  Idle              011   !t ffffd086c6d69080     0000    0    0     130545      1:05:51.140                0        13:04.125  R------ RUNNING  Exec                  (CPU)                 nt!PpmIdleGuestExecute+0x1d
// Unable to load image c:\windows\system32\driverstore\filerepository\surfaceoempanel.inf_amd64_a8a57c8e12e207d2\surfaceoempanel.dll, Win32 error 0n2
// 1570  WUDFHost.exe       12   !t ffffd086d63020c0     15b4    8    8       1142           12.359             .015            5.609  R----r- RUNNING  Quantum               (CPU)                 igdkmdn64+0x37368e
// 2ACC  SgrmBroker.exe     13   !t ffffd086d97f3080     3b64    8    8        798            6.531                0            6.125  R----r- RUNNING  Quantum               (CPU)                     0xfffff803357b001c
// 0004  System             14   !t ffffd086d1d9c100     0360   16   16     364275           21.171                0           12.359  R------ RUNNING  Exec                  (SpinLock)            nt!KxWaitForSpinLockAndAcquire+0x1c
// 10E0  SurfaceMLServi     15   !t ffffd086d48ce080     26c0    8    8        516            5.875             .093            5.812  R----r- RUNNING  Yield                 (CPU)                 nt!MiLockPageTableInternal+0x270
// 
//
// \\\   
//  >>>  Activity Analysis B - Blocking Locks Summary
// ///   
// An unexpected circular linked list was detected on the shared waiters list for lock 0xffffd086d1972010.
// An unexpected circular linked list was detected on the exclusive waiters list for lock 0xffffd086d1972010.
// Invalid Thread(2) 00060001 Type = 0, should be 6.
// 
//
// Resource @ 0xffffd086d1972010    Shared 1 owning threads
//     Contention Count = 2285996
//     NumberOfSharedWaiters = 1
//     NumberOfExclusiveWaiters = 8
//      Threads: ffffd086d74d5080-01<*> 
// 
//      THREAD ffffd086d74d5080  Cid 23e4.2280  Teb: 000000ae78b79000 Win32Thread: ffffd086d70dd5b0 STANDBY
//      Not impersonating
//      DeviceMap                 ffffbf01bc9ec220
//      Owning Process            ffffd086d6809080       Image:         StartMenuExperienceHost.exe
//      Attached Process          N/A            Image:         N/A
//      Wait Start TickCount      310361         Ticks: 392 (0:00:00:06.125)
//      Context Switch Count      5707           IdealProcessor: 12             
//      UserTime                  00:00:00.593
//      KernelTime                00:00:00.343
//      Win32 Start Address 0x00007ffd94497480
//      Stack Init ffff80897ce37bb0 Current ffff80897ce372f0
//      Base ffff80897ce38000 Limit ffff80897ce31000 Call 0000000000000000
//      Priority 13 BasePriority 8 PriorityDecrement 80 IoPriority 2 PagePriority 5
//      Scheduling Group: ffffd086d76e7080
//      Child-SP          RetAddr               Call Site
//      ffff8089`7ce37330 fffff803`39e83255     nt!KiSwapContext+0x76
//      ffff8089`7ce37470 fffff803`39e813df     nt!KiSwapThread+0xa65
//      ffff8089`7ce375c0 fffff803`39e86e9d     nt!KiCommitThreadWait+0x11f
//      ffff8089`7ce37660 fffff803`39f35530     nt!KeWaitForSingleObject+0x1fd
//      ffff8089`7ce37760 fffff803`39e85f90     nt!ExpWaitForResource+0x60
//      ffff8089`7ce377e0 fffff803`39f2aee9     nt!ExpAcquireResourceSharedLite+0x590
//      ffff8089`7ce37880 ffffe0d5`be65970b     nt!ExEnterCriticalRegionAndAcquireResourceShared+0x39
//      ffff8089`7ce378c0 ffffe0d5`bf7ffe16     win32kbase!EnterSharedCrit+0xab
//      ffff8089`7ce37960 ffffe0d5`bebd49ea     win32kfull!NtUserGetQueueStatus+0x36
//      ffff8089`7ce379f0 fffff803`3a036e65     win32k!NtUserGetQueueStatus+0x16
//      ffff8089`7ce37a20 00007ffd`93a3a054     nt!KiSystemServiceCopyEnd+0x25 (TrapFrame @ ffff8089`7ce37a20)
//      000000ae`793ff458 00000000`00000000     0x00007ffd`93a3a054
// 
// ffffd086d7031080-01    
// 
//      THREAD ffffd086d7031080  Cid 20c4.2184  Teb: 0000000000f18000 Win32Thread: ffffd086d70aa2b0 WAIT: (WrResource) KernelMode Non-Alertable
//          ffff80897eb77ba0  SynchronizationEvent
//      Not impersonating
//      DeviceMap                 ffffbf01bc9ec220
//      Owning Process            ffffd086d6ecd080       Image:         explorer.exe
//      Attached Process          N/A            Image:         N/A
//      Wait Start TickCount      310365         Ticks: 388 (0:00:00:06.062)
//      Context Switch Count      47515          IdealProcessor: 6             
//      UserTime                  00:00:01.453
//      KernelTime                00:00:01.984
//      Win32 Start Address 0x00007ffd94497480
//      Stack Init ffff80897eb77fb0 Current ffff80897eb77660
//      Base ffff80897eb78000 Limit ffff80897eb71000 Call 0000000000000000
//      Priority 12 BasePriority 9 PriorityDecrement 2 IoPriority 2 PagePriority 5
//      Child-SP          RetAddr               Call Site
//      ffff8089`7eb776a0 fffff803`39e83255     nt!KiSwapContext+0x76
//      ffff8089`7eb777e0 fffff803`39e813df     nt!KiSwapThread+0xa65
//      ffff8089`7eb77930 fffff803`39e86e9d     nt!KiCommitThreadWait+0x11f
//      ffff8089`7eb779d0 fffff803`39f35530     nt!KeWaitForSingleObject+0x1fd
//      ffff8089`7eb77ad0 fffff803`39e85f90     nt!ExpWaitForResource+0x60
//      ffff8089`7eb77b50 fffff803`39f2aee9     nt!ExpAcquireResourceSharedLite+0x590
//      ffff8089`7eb77bf0 ffffe0d5`be65970b     nt!ExEnterCriticalRegionAndAcquireResourceShared+0x39
//      ffff8089`7eb77c30 ffffe0d5`bf7ffc7f     win32kbase!EnterSharedCrit+0xab
//      ffff8089`7eb77cd0 ffffe0d5`bebd73f9     win32kfull!NtUserMessageCall+0x22f
//      ffff8089`7eb77d60 fffff803`3a036e65     win32k!NtUserMessageCall+0x3d
//      ffff8089`7eb77db0 00007ffd`93a31554     nt!KiSystemServiceCopyEnd+0x25 (TrapFrame @ ffff8089`7eb77e20)
//      00000000`0410e308 00000000`00000000     0x00007ffd`93a31554
// 
// 
//      Threads Waiting On Exclusive Access:
//               ffffd086d7059080       ffffd086d721d080       ffffd086d7e6d080       ffffd086d4ecf0c0       
//               ffffd086d75664c0       ffffd086d86df080       ffffd086d496b080       ffffd086d83020c0       
// 
// 

function CLOCK_WATCHDOG_TIMEOUT(Args) {
    logln(this.signature + " ***> CLOCK_WATCHDOG_TIMEOUT <***");
    logln("bucket: " + this.bucket);

    logln("The CLOCK_WATCHDOG_TIMEOUT bug check has a value of 0x00000101. This indicates");
    logln("that an expected clock interrupt on a secondary processor, in a multi-processor");
    logln("system, was not received within the allocated interval.");

    logln('Arg1: ' + Args[0] + ', Clock interrupt time-out interval, in nominal clock ticks');
    logln('Arg2: ' + Args[1] + ', 0');
    logln('Arg3: ' + Args[2] + ', The address of the processor control block (PRCB) for the unresponsive processor');
    logln('Arg4: ' + Args[3] + ', faulting processor');

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

}

function INTERRUPT_EXCEPTION_NOT_HANDLED(Args){ logln(this.signature + " not implemented"); }
function INTERNAL_POWER_ERROR(Args){            logln(this.signature + " not implemented"); }
function DRIVER_IRQL_NOT_LESS_OR_EQUAL(Args){   logln(this.signature + " not implemented"); }
function KERNEL_SECURITY_CHECK_FAILURE(Args){   logln(this.signature + " not implemented"); }
function UCMUCSI_LIVEDUMP(Args){                logln(this.signature + " not implemented"); }

