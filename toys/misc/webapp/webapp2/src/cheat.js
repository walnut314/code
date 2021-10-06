#!/usr/bin/env node

'use strict'

const program = require('commander');
const readline = require('readline');
const fs = require('fs');
const Buffer = require('buffer').Buffer;
const printf = require('printf');
const csv = require('csv');
const colors = require('colors');
const crypto = require('crypto');


console.log("Hello, world!");

let listFunction = (directory,options) => {
    //some code here
    console.log("dude");
}

program
  .version('0.0.1')
  .command('list [directory]')
  .description('List files and folders')
  .option('-a, --all','List all files and folders')
  .option('-l, --long','')
  .action(listFunction);
program.parse(process.argv);


var rl = readline.createInterface({
  //input: process.stdin,
  input: fs.createReadStream('dude.txt'),
  output: process.stdout,
  terminal: false
});

rl.on('line', function(line){
    //console.log(line);
    parse_pkt(line);
})

var regex2 = new RegExp('typedef \\w+');



var str = 'For more information, see Chapter 3.4.5.1';
var re = /see (chapter \d+(\.\d)*)/i;
var found = str.match(re);
console.log(colors.yellow("look"));
//if (found == true) {
    console.log(colors.red(found));
//}
console.log(colors.yellow("over"));

function parse_pkt(pkt) {
    console.log(colors.red(pkt));
    var res = regex2.test(pkt);
    if (res == true) {
        console.log(colors.red("found typedef"));
        var str = printf("pkt: %s", pkt);
        console.log(colors.yellow(str));
        var re1 = /typedef (\w+) (\w+) (\d+)/;
        var found_it = pkt.match(re1);
        console.log(colors.red(found_it)); // dump whole array
        str = printf("found %d matches", found_it.length);
        console.log(str);
        console.log(colors.yellow(found_it[1]));
        console.log(colors.blue(found_it[2]));
        console.log(colors.green(found_it[3]));
        found_it.forEach(function(value){
            console.log(colors.red(value));
        });        
    } else {
        console.log("typedef not found");
    }
}

csv
// Generate 8 records
.generate({
  delimiter: '|',
  length: 8
})
// Parse the records
.pipe(csv.parse({
  delimiter: '|'
}))
// Transform each value into uppercase
.pipe(csv.transform(function(record){
   return record.map(function(value){
     return value.toUpperCase()
   });
}))
// Convert the object into a stream
.pipe(csv.stringify({
  quoted: true
}))
// Print the CSV stream to stdout
.pipe(process.stdout)



// crypto stuff
console.log(colors.red('crypto stuff'));
const secret = 'abcdefg';
const hash = crypto.createHmac('sha256', secret)
                   .update('I love cupcakes')
                   .digest('hex');
console.log(hash);
// Prints:
//   c0fa1bc00531bd78ef38c628449c5102aeabd49b5dc3a2a516ea6ea959d6658e


