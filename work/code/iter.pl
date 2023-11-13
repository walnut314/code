use strict;
use warnings;
use File::Find;

my @files;
my $start_dir = ".\\";  # top level dir to search
find( 
    sub { push @files, $File::Find::name unless -d; }, 
    $start_dir
);

for my $file (@files) {
    if ($file =~ /log$/) {
        print("dude log: $file\n");
        open(my $fh, $file);
        while (my $row = <$fh>) {
            chomp $row;
            print "$row\n";
        }
        close($fh);
    }
}
