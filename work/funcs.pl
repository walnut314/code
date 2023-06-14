use List::Util 'uniq';

# strip address prefixes
#while (<>) {
#    if ($_ =~ /(nt!.*)(\([a-zA-Z].*)/) {
#        print "$1$2\n"
#        #print $_;
#    }
#}

# scan for functions with structs
#while (<>) {
#    if ($_ =~ /struct/) {
#        print $_;
#    }
#}

# remove inlines
#while (<>) {
#    if ($_ =~ /inline/) {
#        #print $_;
#    } else {
#        print $_;
#    }
#}

# append ';' to each line
#while (<>) {
#    chomp;
#    print("$_;\n");
#}

# scan structs in functions - WIP
#my @strs = qw/struct/;
#$count = 0;
#while (<>) {
#    s{ ( (.) \2+ ) \z }(substr($1, 0, length($1) / 2))ex, print, $count++
#  for @strs;
#}
#print $count;

# scan structs in functions
@structs = ();
while (<>) {
    my @strs = split ' ';
    for($i = 0; $i < scalar(@strs); $i++ ) {
        if ($strs[$i] eq 'struct') {
            print "$strs[$i+1]\n";
        }
    }
}


