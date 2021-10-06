#!/usr/bin/env node

"use strict";

const peg = require('pegjs');

var parser = peg.generate(
  'start\n' +
  '  = additive\n' +
  '\n' +
  'additive\n' +
  '  = left:multiplicative "+" right:additive { return left + right; }\n' +
  '  / multiplicative\n' +
  '\n' +
  'multiplicative\n' +
  '  = left:primary "*" right:multiplicative { return left * right; }\n' +
  '  / primary\n' +
  '\n' +
  'primary\n' +
  '  = integer\n' +
  '  / "(" additive:additive ")" { return additive; }\n' +
  '\n' +
  'integer "integer"\n' +
  '  = digits:[0-9]+ { return parseInt(digits.join(""), 10); }'
);

console.log(parser.parse('(1+2)*3'));
