#!/usr/bin/perl

use List::Util qw(sum);

$stage = 0; # current stage
$phase = 0; # current stage
$ptime = 0;
$stime = 0;
@ptimer = ();
@stimer = ();
@ptimer_samples = ();
@stimer_samples = ();
$stimer_count = 0;
$ptimer_count = 0;

while (<STDIN>) {
    chomp;
    if ($_ =~ m/S(\d+) (\d+): (.*)/i) {
        $stage  = $1;
        $phase  = $2;
        $ptime  = $3;
        $ptimer[$stage][$phase] += $ptime;
        $ptimer_samples[$stage][$phase][$ptimer_count] = $ptime;
        if ($stage == 0) {
            $ptimer_count++;
        }
    } elsif ($_ =~ m/Stage (\d+): (.*)/i) {
        $stage  = $1;
        $stime  = $2;
        $stimer[$stage] += $stime;
        $stimer_samples[$stage][$stimer_count] = $stime;
        if ($stage == 0) {
            $stimer_count++;
        }
    }

}

sub average{
        my($data) = @_;
        if (not @$data) {
                die("Empty arrayn");
        }
        my $total = 0;
        foreach (@$data) {
                $total += $_;
        }
        my $average = $total / @$data;
        return $average;
}

sub stdev{
        my($data) = @_;
        if(@$data == 1){
                return 0;
        }
        my $average = &average($data);
        my $sqtotal = 0;
        foreach(@$data) {
                $sqtotal += ($average-$_) ** 2;
        }
        my $std = ($sqtotal / (@$data-1)) ** 0.5;
        return $std;
}

$idx = 0;
$stotal = 0;
print "stage times: $sdiv\n";
foreach $st (@stimer) {
    my $a = sprintf("%06f", $st / $stimer_count);
    $stotal += $a;
    print "stage[$idx]: $a\n";
    $idx++;
}

print "frame time:==> $stotal\n\n";

$idx = 0;
print "phase times\n";
foreach $pt (@ptimer) {
    #$avg = sprintf("%0.6f", &average($pt));
    #$std = sprintf("%0.6f", &stdev($pt));
    #print "avg: $avg, std: $std\n";
    print "stage[$idx]: ";
    foreach $st (@$pt) {    
        my $a = sprintf("%06f", $st / $stimer_count);
        print "$a, ";
    }
    print "\n";
    $idx++;
}

print "\n\n";
print "*************************************\n";
print "**** raw sample data ****************\n";
print "*************************************\n";
print "\n\n";

print "**** stage data ****************\n";
my $sdx = 0;
foreach $st (@stimer_samples) {
    print "stage[$sdx]: ";
    $avg = sprintf("%0.6f", &average($st));
    $std = sprintf("%0.6f", &stdev($st));
    print " avg: $avg, std: $std\n";
    $sdx++;
}

print "\n";
my $sdx = 0;
my $pdx = 0;
print "**** phase data ****************\n";
foreach $pt (@ptimer_samples) {
    $pdx = 0;
    print "stage[$sdx]: \n";
    foreach $st (@$pt) {    
        $avg = sprintf("%0.6f", &average($st));
        $std = sprintf("%0.6f", &stdev($st));
        print "   p[$pdx]: avg: $avg, std: $std\n";
        $pdx++;
    }
    $sdx++;
}

exit;

