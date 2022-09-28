#!/usr/bin/perl

%pizzas = (
"_stl" => 0,
"AIQD" => 0,
"DCTX" => 0,
"DFCI" => 0,
"DVMM" => 0,
"GNRM" => 0,
"HRTH" => 0,
"ISPp" => 0,
"ISYS" => 0,
"KNEW" => 0,
"MUTE" => 0,
"ISYS" => 0,
"SCFG" => 0,
"SNOD" => 0,
"SYSI" => 0,
"UMM" => 0,
"USTR" => 0,
"VMdl" => 0,
"VStr" => 0,
"WITM" => 0
);

#$counter = 0;
#while (($key, $value) = each (%pizzas))
#{
#  #$value = $pizzas{$key};
#  print "  $key costs $value\n";
#  $counter++;
#}
#print("#hashes: $counter\n");
#exit;
#
#foreach $pizza (@pizzas) {
#  print "$pizza => $pizzas{$pizza}\n";
#}
#exit;

#$tag = "AIQD";
#$tag = "_stl";
#$tag = "DCTX";
#$tag = "DFCI";
#$tag = "DVMM";
#$tag = "GNRM";
#$tag = "HRTH";
#$tag = "ISPp";
#$tag = "ISYS";
#$tag = "KNEW";
#$tag = "MUTE";
#$tag = "ISYS";
#$tag = "SCFG";
#$tag = "SNOD";
#$tag = "SYSI";
#$tag = "UMM";
#$tag = "USTR";
#$tag = "VMdl";
#$tag = "VStr";
#$tag = "WITM";
#$tag = "WITM";

#      0xffffae82bb7f4fe0,  0x00000018,  DVMM,  0xfffff80783d91253,  iacamera64!Dvmm_Allocate+0x28f

# /(?:0[xX][\da-fA-F]+)
$count      = 0;
$num        = 0;
while (<>) {
    chomp;
    $line++;
    $addr = 0;
    $size = 0;
    #if ($_ =~ /\+0x010 host_addr/) {
    #if ($_ =~ /\+0x050 actual_size/) {

    while (($key, $value) = each (%pizzas))
    {
        $value = $pizzas{$key};
        #print "  $key costs $value\n";
        $tag = $key;
        #foreach $pizza (@pizzas) {
        if ($_ =~ /$tag/) {
            $num++;
            #print("$line: $_\n");
            #$_ =~ /0x([\da-fA-F]{16}),  0x([\da-fA-F]{8}),  (\s{4}),  0x([\da-fA-F]{16}),  (.*)/;
            $_ =~ /0x([\da-fA-F]{16})  0x([\da-fA-F]{8})(.*)/;
            $sz = hex($2);
            $count += $sz;
            $pizzas{$key}++;
            #print("$line: $1, $sz, $3\n"); #, $2, $3 $4, $5\n");
        }
    }
}
print("$tag => $num => total size: $count (bytes)\n");

while (($key, $value) = each (%pizzas))
{
  $value = $pizzas{$key};
  print "  $key count $value\n";
  $counter++;
}
print("#hashes: $counter\n");

