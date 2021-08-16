#!/usr/bin/env node

const WebSocket = require('ws');

var ws = new WebSocket('ws://10.0.0.138:8080/');
    
ws.onopen = function() {
    console.log('open');
}

ws.onmessage = function(event) {
  console.log('Count is: ' + event.data);
};

