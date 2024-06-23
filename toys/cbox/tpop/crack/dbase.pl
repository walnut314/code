#!/usr/bin/perl

#
# let's create a databasea
dbmopen(%pets, "/Users/bartbartel/dev/repo/code/toys/cbox/tpop/crack/petdata", 0644) || die "Cannot open pets file $!";

# add fields
#$pets{cat} = "Mao";
#$pets{dog} = "Spot";
#$pets{fish} = "Nemo";
#dbmclose(%pets);

# enumerate all fields
while(($animal, $name) = each %pets) {
    print "the name of my $animal is $name\n";
}
dbmclose(%pets);



#print("hello, world\n");
