"use strict";

// File: utils.js
let exec  = undefined;
let regex = /Arg(\d): ([a-zA-Z0-9]{16})/;
let bufex = /(\".*\")/;
let irpex = /.* = ([a-zA-Z0-9]{8}`[a-zA-Z0-9]{8})/;
let re    = undefined;

function utils_init() {
    host.diagnostics.debugLog('dude wusup\n');
    exec = host.namespace.Debugger.Utility.Control.ExecuteCommand;
    re = new RegExp('(\".*\")');
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

