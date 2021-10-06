#!/usr/bin/env node

// or we could use fs.watchFile

// To find these log files, look in the following locations:

// System Log Folder: /var/log
// System Log: /var/log/system.log
// Mac Analytics Data: /var/log/DiagnosticMessages
// System Application Logs: /Library/Logs
// System Reports: /Library/Logs/DiagnosticReports
// User Application Logs: ~/Library/Logs (in other words, /Users/NAME/Library/Logs)
// User Reports: ~/Library/Logs/DiagnosticReports (in other words, /Users/NAME/Library/Logs/DiagnosticReports)

Tail = require('tail').Tail;

tail = new Tail("/var/log/system.log");

tail.on("line", function(data) {
  console.log(data);
});

