#!/usr/bin/perl

use strict;
use warnings;

my $ip="216.108.225.236:60099";

# see below for explanation
if ($ip =~ /\b(\d{1,3}(?:\.\d{1,3}){3}:\d{1,5})\b/) {
    print "$ip - matches\n";
} else {
    print "$ip - does not match\n";
}



