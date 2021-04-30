#!/usr/bin/perl

my $cur_frame= 0;
my $in = 0;
while (<>) {
    if ($_ =~ /^(\d+), /) {
        if ($1 != $cur_frame) {
            print $_;
            $cur_frame = $1;
        }
    }
}
