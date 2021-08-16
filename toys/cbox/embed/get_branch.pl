#!/usr/bin/perl
#
use Cwd;

my ($subsys)     = $ARGV[0];
my ($build_name) = $ARGV[1];

#print "$subsys, $build_name\n";
#exit;

# <Exec Command="svn info | findstr -is URL &gt; Resources\BuildInfo.txt" />
$cmd = "svn info ..\\..";
@output = `$cmd`;
$owner  = "";
$branch = "";
$url    = "";
$full   = "";
$rev    = "";
foreach (@output) { 
    if ($_ =~ /URL: (http:\/\/subversion.x.com\/svn\/HAP1-CERT\/branches)\/([a-zA-Z0-9]+)\/([a-zA-Z0-9_]+)/ig)  { 
        $url = $1; $branch = $3; #$full = $_; 
        print "full branch  : $_\n";
    } elsif ($_ =~ /URL: (http:\/\/subversion.xxx.com\/svn\/HAP1-CERT\/Tags)\/([a-zA-Z0-9_]+)\//ig)  { 
        $url = $1; $branch = $2; #$full = $_; 
        print "full tags : $_\n";
    } elsif ($_ =~ /Revision: (\d+)/ig) {
        $rev = $1;
    } elsif ($_ =~ /Author: (.*)/ig) {
        $owner = $1;
    }
} 
$full = "$url/$owner/$branch [$build_name] Rev: $rev";
print "url   : $url\n";
print "branch: $branch\n";
print "owner : $owner\n";

print "$full\n";

##
# Now search for token=BRANCH_ID_TOKEN in target file
# and replace the definition of the token with the build
# string.
#
# Open target file 

my $target_file;
if ($subsys eq "MCP") {
    $target_file = "..\\..\\..\\..\\MCP\\Source\\Target_MCP.c";
}
elsif ($subsys eq "ADP") {
    $target_file = "..\\..\\..\\..\\ADP\\Source\\Configuration_Mgmt_ADP.c";
} 
else 
{
    die "Invalid input specifier";
}

print "$target_file\n";

open(FILE, "<$target_file") || die "Could not open file '$target_file' $!";
# read the entire file into lines array
my @lines = <FILE>;
close(FILE);
# loop over all lines in Target_MCP.c, searching for the token
# and replace the token and the string defining it with the full
# build branch.
my @newlines;
foreach(@lines) {
   $_ =~ s/BRANCH_ID_TOKEN .*/BRANCH_ID_TOKEN "$full"/g;
   push(@newlines,$_);
}
# re-open Target_MCP.c and replace the lines with the new
# lines, updating the token.
open(FILE, ">$target_file") || die "File not found";
print FILE @newlines;
close(FILE);


