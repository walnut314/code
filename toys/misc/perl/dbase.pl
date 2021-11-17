#!/usr/bin/perl

#
#dbmopen(%pets, "C:/dev/Scripts/pets", 0644) || die "Cannot open petFile";
#$pets{cat} = "Mao";
#$pets{dog} = "Spot";
#dbmclose(%pets);

dbmopen(%pets, "C:/dev/Scripts/pets", 0644) || die "Cannot open petFile";
while(($animal, $name) = each %pets) {
    print "the name of my $animal is $name\n";
}
dbmclose(%pets);

print("hello, world\n");
