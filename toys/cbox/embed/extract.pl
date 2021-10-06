#!/usr/bin/perl

$start_time = 283.3;   #10.05;
$stop_time  = 283.4; #19.01;
$collect = 0;

#while (<STDIN>) {
#    chomp;
#    if (($_ =~ m/Expected/i) || ($_ =~ m/Actual/i)) {
#        #print "yo: $_\n";
#    } else {
#        print "$_\n";
#    }
#
#}

# pattern: psi_dot_target 0.000174533 => 9.13852e-006
$time = 0;
$data = 0;
$psi_target_sim = 0;
$psi_target_sw = 0;
$psi_target_delta = 0;
$psi_dot_target_sim = 0;
$psi_dot_target_sw = 0;
$psi_dot_target_delta = 0;
$psi_s = 0;
while (<STDIN>) {
    chomp;
    if ($_ =~ m/Time=(\d+\.*\d{0,2})/i) {
        $data = $_;
        $time = $1;
        if (($time > $start_time) && ($time < $stop_time)) {
            $collect = 1;
        } else {
            $collect = 0;
        }
        #print "time: $time\n";
    }
    #elsif (($_ =~ m/psi_dot_target (.*) => (.*)/i)) {
    elsif (($_ =~ m/psi_dot_target (.*) => (.*)/i)) {
        $psi_dot_target_sim = $1;
        $psi_dot_target_sw = $2;
        #if ($collect == 1) {
        #    print "$time, $psi_target_sim, $psi_target_sw\n";
        #}
    }
    elsif (($_ =~ m/psi_target (.*) => (.*)/i)) {
        $psi_target_sim = $1;
        $psi_target_sw = $2;
        if ($collect == 1) {
            #print "$time, $psi_target_sim, $psi_target_sw\n";
            print "$data $psi_dot_target_sim, $psi_dot_target_sw, $psi_target_sim, $psi_target_sw\n";
        }
    }
    #elsif (($_ =~ m/psi_s=(.*),/i)) {
    #    $psi_s = $1;
    #     print "psi_s: $psi_s\n";        
    #    if ($collect == 1) {
    #        print "psi_s: $psi_s\n";
    #    }
    #}

}

