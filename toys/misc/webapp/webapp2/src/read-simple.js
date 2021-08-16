#!/usr/bin/env node

'use strict'

/*
 * https://blog.tojicode.com/2011/08/jsstruct-c-style-struct-reading-in.html
 */

const fs = require('fs');
var hexdump = require('hexdump-nodejs');
const Buffer = require('buffer').Buffer;

fs.readFile('helicopter.jpg', (err, data) => {
//fs.readFile('mcpa.bin', (err, data) => {
    if (err) {
        throw err;
    }
    //console.log(Number(data.slice(1,4)));
    //var digit = parseInt(data.slice(0,3), 2);
    //console.log(digit);
    //console.log(bin2dec(data.slice(0,3)));
    //console.log(hexdump(data));

    console.log("frames");
    var f;
    for (f = 0; f < 8; f++) {
        var start = f*0x82;
        var end = start + 0x82;
        var pkt = Buffer.from(data.slice(start,end));
        parse_pkt(pkt);
    }

    //console.log(bin2hex(data.slice(0,1)));
    //console.log(bin2hex(data.slice(4,5)));
    //console.log(bin2hex(data.slice(8,9)));
    //console.log(bin2hex(data.slice(12,13)));
    //console.log(data.slice(0,3).toString("hex"));
    //console.log(data.slice(4,7).toString());
    //console.log(data.slice(8,11).toString());
    //console.log(data.slice(12,15).toString());
});

function dec2bin(dec) {
    return (dec >>> 0).toString(2);
}

function bin2dec(num) {
    return num.split('').reverse().reduce(function(x, y, i){
      return (y === '1') ? x + Math.pow(2, i) : x;
    }, 0);
}

function bin2hex(bin) {
    return new Buffer(bin).toString("hex");
}


var TypeEnum = {
    U32  : 0,
    F32  : 1,
    BOOL : 2
};

var Columns = [
    {'start': 1, 'end': 5, 'name': "frame", 'type': TypeEnum.U32},

    {'start': 5, 'end': 9, 'name': "phi", 'type': TypeEnum.F32},
    {'start': 9, 'end': 13, 'name': "theta", 'type': TypeEnum.F32},
    {'start': 13, 'end': 17, 'name': "psi", 'type': TypeEnum.F32},

    {'start': 17, 'end': 21, 'name': "magX", 'type': TypeEnum.F32},
    {'start': 21, 'end': 25, 'name': "magY", 'type': TypeEnum.F32},
    {'start': 25, 'end': 29, 'name': "magZ", 'type': TypeEnum.F32},

    {'start': 29, 'end': 33, 'name': "p", 'type': TypeEnum.F32},
    {'start': 33, 'end': 37, 'name': "q", 'type': TypeEnum.F32},
    {'start': 37, 'end': 41, 'name': "r", 'type': TypeEnum.F32},

    {'start': 41, 'end': 45, 'name': "accelX", 'type': TypeEnum.F32},
    {'start': 45, 'end': 49, 'name': "accelY", 'type': TypeEnum.F32},
    {'start': 49, 'end': 53, 'name': "accelZ", 'type': TypeEnum.F32},

];
var numColumns = 13;

/*
	parser->Set_Column(++column, "Frame Number", TELEMETRY_LOG_STRUCT, framenumber, U32_Type);

	parser->Set_Column(++column, "Phi", TELEMETRY_LOG_STRUCT, eulers.phi, F32_Type);
	parser->Set_Column(++column, "Theta", TELEMETRY_LOG_STRUCT, eulers.theta, F32_Type);
	parser->Set_Column(++column, "Psi", TELEMETRY_LOG_STRUCT, eulers.psi, F32_Type);

	parser->Set_Column(++column, "Mag X", TELEMETRY_LOG_STRUCT, mags.X, F32_Type);
	parser->Set_Column(++column, "Mag Y", TELEMETRY_LOG_STRUCT, mags.Y, F32_Type);
	parser->Set_Column(++column, "Mag Z", TELEMETRY_LOG_STRUCT, mags.Z, F32_Type);

	parser->Set_Column(++column, "p", TELEMETRY_LOG_STRUCT, rates.X, F32_Type);
	parser->Set_Column(++column, "q", TELEMETRY_LOG_STRUCT, rates.Y, F32_Type);
	parser->Set_Column(++column, "r", TELEMETRY_LOG_STRUCT, rates.Z, F32_Type);

	parser->Set_Column(++column, "Accel X", TELEMETRY_LOG_STRUCT, accels.X, F32_Type);
	parser->Set_Column(++column, "Accel Y", TELEMETRY_LOG_STRUCT, accels.Y, F32_Type);
	parser->Set_Column(++column, "Accel Z", TELEMETRY_LOG_STRUCT, accels.Z, F32_Type);

	parser->Set_Column(++column, "phi dot", TELEMETRY_LOG_STRUCT, euler_rates.phi_dot, F32_Type);
	parser->Set_Column(++column, "theta dot", TELEMETRY_LOG_STRUCT, euler_rates.theta_dot, F32_Type);
	parser->Set_Column(++column, "psi dot", TELEMETRY_LOG_STRUCT, euler_rates.psi_dot, F32_Type);

	parser->Set_Column(++column, "Roll Actr Posn", TELEMETRY_LOG_STRUCT, actuator_positions.roll, F32_Type);
	parser->Set_Column(++column, "Pitch Actr Posn", TELEMETRY_LOG_STRUCT, actuator_positions.pitch, F32_Type);

	parser->Set_Column(++column, "Roll Actr Cmd", TELEMETRY_LOG_STRUCT, actuator_commands.roll, F32_Type);
	parser->Set_Column(++column, "Pitch Actr Cmd", TELEMETRY_LOG_STRUCT, actuator_commands.pitch, F32_Type);

	parser->Set_Column(++column, "Roll Motor A Amps", TELEMETRY_LOG_STRUCT, motor_A_current.roll, F32_Type);
	parser->Set_Column(++column, "Pitch Motor A Amps", TELEMETRY_LOG_STRUCT, motor_A_current.pitch, F32_Type);
	parser->Set_Column(++column, "Roll Motor B Amps", TELEMETRY_LOG_STRUCT, motor_B_current.roll, F32_Type);
	parser->Set_Column(++column, "Pitch Motor B Amps", TELEMETRY_LOG_STRUCT, motor_B_current.pitch, F32_Type);

	parser->Set_Column(++column, "IAS", TELEMETRY_LOG_STRUCT, IAS, F32_Type);
	parser->Set_Column(++column, "TAS", TELEMETRY_LOG_STRUCT, TAS, F32_Type);
	parser->Set_Column(++column, "Baro altitude", TELEMETRY_LOG_STRUCT, baro_altitude, F32_Type);
	parser->Set_Column(++column, "Outside air temp", TELEMETRY_LOG_STRUCT, oat, F32_Type);

	parser->Set_Column(++column, "Latest Fault", TELEMETRY_LOG_STRUCT, latest_fault, U32_Type);
	parser->Set_Column(++column, "Button", TELEMETRY_LOG_STRUCT, button_states, U8_Type);
	parser->Set_Column(++column, "AP states", TELEMETRY_LOG_STRUCT, flags, U32_Type);
*/

function parse_pkt(pkt) {
/*
    var c0 = Columns[0].start;
    var c1 = Columns[0].end;
    var c2 = Columns[0].name;

    console.log(c0);
    console.log(c1);
    console.log(c2);

    var frame = bin2hex(Buffer.from(pkt.slice(1,5)));
    var digit = parseInt(frame, 16);
    console.log(digit);

    var phi   = Buffer.from(pkt.slice(5,9));
    var theta = Buffer.from(pkt.slice(9,13));
    var psi   = Buffer.from(pkt.slice(9,13));

    var magx = Buffer.from(pkt.slice(5,9));
    var magy = Buffer.from(pkt.slice(9,13));
    var magz = Buffer.from(pkt.slice(9,13));
    
    console.log(phi);
    console.log(theta);
    console.log(psi);
    //console.log(phi.writeFloatBE(0.1,0));
    //console.log(theta.writeFloatBE(0.1,0));
    //console.log(psi.writeFloatBE(0.1,0));
    //console.log(psi.getFloat32(0, true));
*/
    //console.log(hexdump(pkt));
    var col;
    for (col = 0; col < numColumns; col++) {
        var type = Columns[col].type;
        var value = bin2hex(Buffer.from(pkt.slice(Columns[col].start,Columns[col].end)));
        var buffer = Buffer.from(pkt.slice(Columns[col].start,Columns[col].end));
        //var int32View = new Int32Array(buffer);
        //var float32View = new Float32Array(buffer);

        console.log(Columns[col].name, ": ", value);
        //console.log(int32View);
        //console.log(float32View);
        //console.log(type);
        //console.log(value);
    }
    console.log("");
}


