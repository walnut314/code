#!/usr/bin/perl

# 2020-10-07T18:21:54.5746912Z
$line = 0;
$start = 0;
$end = 0;
while (<>) {
    $line++;
    chomp;
    #if ($_ =~ /2020-10-07T(\d2:\d2:\d2\.[0-9A-Z]8)(.*)/) {
    if ($_ =~ /2020-10-07T(\d{2}.\d{2}.\d{2}\.[0-9A-Z]{8})(.*)/) {
        if ($start == 0) {
            $start = $line;
        } else {
            $end = $line;
        }
        print("$2\n");
   } else {
        print("$_\n");
   }
}
print("range: $start - $end\n");
