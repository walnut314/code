#!/usr/bin/perl

# from PowerShell
# gc .\ADLP-PSTS3_DripsWatchdog-20220502-1339.log | perl .\scan.pl > dude
#

while (<>) {
    chomp;
    if ($_ =~ /BT: (.*)/) {
        print "BT: $1\n";
    }
    if ($_ =~ /BAT: (.*)/) {
        print "BAT: $1\n";
    }
}
