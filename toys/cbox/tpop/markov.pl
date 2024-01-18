#!/usr/bin/perl

$MAXGEN = 10000;
$NONWORD = "douch\n";
$w1 = $w2 = $NONWORD;

while (<>) {
    foreach (split) {
        push(@{$statetab{$w1}{$w2}}, $_);
        ($w1, $w2) = ($w2, $_);
    }
}
push(@{$statetab{$w1}{$w2}}, $NONWORD);

$w1 = $w2 = $NONWORD;
for ($i = 0; $i < $MAXGEN; $i++) {
    $suf = $statetab{$w1}{$w2};
    $r = int(rand @$suf);
    exit if (($t = $suf->[$r]) eq $NONWORD);
    print "$t\n";
    ($w1, $w2) = ($w2, $t);
}

