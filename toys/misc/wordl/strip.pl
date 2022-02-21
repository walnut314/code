#!/usr/bin/perl

# e, t, a, i, o, n, s, h, r
# beg: t, a, o, d, w
# end: e, s, d, t
#
# taint
# tinea

# q w y u o s f j k l z x c v
# s . i l l 

while (<>) {
    chomp;
    if ($_ =~ /(^.....$)/) {
        print "$1\n";
    }
}

# slim down
while (<>) {
    chomp;
    if ($_ =~ /(^.....$)/) {
        print "$1\n";
    }
}

