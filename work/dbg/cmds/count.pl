#!/usr/bin/perl

# /(?:0[xX][\da-fA-F]+)
$count      = 0;
$num        = 0;
$vied_addr  = "";
$host_addr  = "";
$found = 0;
while (<>) {
    chomp;
    $line++;
    $addr = 0;
    $size = 0;
    #if ($_ =~ /\+0x010 host_addr/) {
    #if ($_ =~ /\+0x050 actual_size/) {
    if ($_ =~ /\+0x010 host_addr/) {
        $_ =~ /host_addr        : 0x([\da-fA-F]{8})`([\da-fA-F]{8})/;
        $host_addr = "$1$2";
        print("host $line: $host_addr\n");
        if ($1 =~ /ffff800/) {
            print("found: $host_addr\n");
            $found = 1;
        }
    }
    if ($_ =~ /\+0x018 vied_addr/) {
        $_ =~ /vied_addr        : 0x([\da-fA-F]{8})`([\da-fA-F]{8})/;
        $vied_addr = "$1$2";
        print("vied $line: $vied_addr\n");
    }
    if ($_ =~ /\+0x050 actual_size/ and ((length($vied_addr)) or (length($host_addr)))) {
        #print("$_\n");
        $_ =~ /actual_size      : 0x([\da-fA-F]+)/;
        #($hi, $lo) = $_ =~ /page_addr        : 0x([\da-fA-F]{8})`([\da-fA-F]{8})/;
        #print("dt nt!_MDL $hi$lo\n");
        #$_ =~ /\+0x010 host_addr        : (.*)/;
        #$addr = $1;
        #$_ =~ /vied_addr        : (.*)/;
        #$_ =~ /page_addr   : (.*)/;
        $size = hex($1);
        print("size $line: $size found: $found\n");
        #if ($addr !~ /\(null\)/) {
        if ($size != 0) { # and $found == 0) {
            $count += $size;
            $num++;
        #    #($hi, $lo) = $addr =~ /([\da-fA-F]{8})`([\da-fA-F]{8})/;
        #} elsif ($addr =~ /\(_MDL\)/) {
        #    #$num++;
        #    #print("addr $addr\n");
        }
    }
    if ($_ =~ /\+0x050 actual_size/) {
        $vied_addr = "";
        $host_addr = "";
        $found = 0;
    }
}
print("count: $num, memsz: $count bytes \n");

