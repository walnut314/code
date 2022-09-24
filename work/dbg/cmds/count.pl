#!/usr/bin/perl

# /(?:0[xX][\da-fA-F]+)
$count = 0;
$num   = 0;
while (<>) {
    $line++;
    chomp;
    #$_ =~ /page_addr   : (.*)/;
    #$_ =~ /host_addr        : (.*)/;
    $_ =~ /vied_addr        : (.*)/;
    $addr = $1;
    #if ($addr !~ /\(null\)/) {
    if (hex($addr) != 0) {
        #if ($_ =~ /actual_size : 0x([\da-fA-F]+)/) {
        #    $count += hex($1);
        #    $num++;
        #}
       ($hi, $lo) = $addr =~ /([\da-fA-F]{8})`([\da-fA-F]{8})/;
       print("$addr => $hi$lo\n");
   } elsif ($addr =~ /\(_MDL\)/) {
       $num++;
       #print("addr $addr\n");
   }
}
print("count: $num, memsz: $count bytes \n");

