#!/usr/bin/perl

# from PowerShell
# gc .\ADLP-PSTS3_DripsWatchdog-20220502-1339.log | perl .\scan.pl > dude
#
while (<>) {
    chomp;
    if ($_ =~ /PDO (.*)/) {
        print "!podev $1\n";
        print "!devstack $1\n";
        print "\$\$\n";
    }
}
