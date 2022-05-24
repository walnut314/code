#!/usr/bin/perl

# from PowerShell
# gc .\ADLP-PSTS3_DripsWatchdog-20220502-1339.log | perl .\scan.pl > dude
#

$in_irp = 0;
while (<>) {
    chomp;
    if ($_ =~ /Not impersonating.*/) {
        #print "ok got out of irps: $_\n";
        $in_irp = 0;
    }
    elsif ($in_irp == 1) {
        $irp = $_ =~ /([a-zA-Z0-9]{16}):/;
        print "!irp $1\n";
    } elsif ($_ =~ /IRP List:.*/) {
        #print "ok got in irp: $_\n";
        $in_irp = 1;
    }
}
