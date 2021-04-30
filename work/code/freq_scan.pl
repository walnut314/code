#!/usr/bin/perl
##!/usr/local/bin/perl

use Text::CSV_XS;


sub read_log {
    my $fh = shift; # file handle
    $count = 0;
    @frames;
    while (<$fh>) {
        chomp;
        if ($_ =~ /FA*B*A*B*(\d+)sinfreq = (\d+\.\d+)/) {
            my $item = {}; # need this for unique instance
            $count++;
            $item->{StartFrame} = $1;
            $item->{Frequency} = $2;
            push @frames, $item;
          }
    }
    return \@frames;
}

sub read_csv {
    my $fh = shift; # file handle
    my $csv = Text::CSV_XS->new;
    my @data;
    $hdr = $csv->getline($fh); # skip header
    #foreach (@$hdr) { printf("> %s\n", $_); }
    while (<$fh>) {
        $csv->parse($_);
        push @data, [$csv->fields];
    }
    return \@data;
}

sub write_csv {
    my $frames = shift;
    my $data = shift;

    foreach (@$frames) {
        printf("> %s => %s\n", $_->{StartFrame}, $_->{Frequency});
    }

    my $n_frames = scalar @$frames;
    my $n_data   = scalar @$data;
    my $i = 0; # frame index
    my $j = 0; # csv array index
    # prime the pump -- need first and next frames
    my $freq_frame = @$frames[0]->{StartFrame};
    my $next_frame = @$frames[1]->{StartFrame};
    $i++;
    $transition = 0;
    while ($j < $n_data) { # iterate of csv data
        my $cur_frame = @$data[$j]->[0]; # csv col refs
        my $amplitude = @$data[$j]->[3];
        if ($transition == 0 && $cur_frame == $freq_frame) {
            $transition = 1;
            print "going hi: $cur_frame => $freq_frame\n";
        } elsif ($transition != 0 && $cur_frame == $next_frame) {
            $transition = 0;
            my $freq_frame = $next_frame;
            my $next_frame = @$frames[i+1]->{StartFrame};
            print "going lo: $cur_frame => $freq_frame\n";
            $transition = 1;
        } elsif ($transition == 1) {
            print "Frame: $cur_frame, $amplitude\n";
        }
        $j++;        
    }
}

sub main {

    if (scalar @ARGV[0] < 1) {
        print "usage: freq_scan 20181116\n";
        exit;
    }

    my $file_prefix = @ARGV[0];
    my $file, $fh;

    $file = $file_prefix . "_MCPA_STDIO.log";
    open($fh, '<:encoding(UTF-8)', $file)
        or die "Could not open file '$file' $!";

    my $frames = read_log($fh); # returns array of frames
    close($fh);

    foreach (@$frames) {
        printf("> %s => %s\n", $_->{StartFrame}, $_->{Frequency});
    }

    exit;

    $file = $file_prefix . "_FreqSweep0_25Hz_A.csv";
    open($fh, '<:encoding(UTF-8)', $file)
        or die "Could not open file '$file' $!";

    my $data = read_csv($fh); # returns array of csv rows
    close($fh);

    write_csv(\@$frames, \@$data);

}

main();


