use File::Copy;


$src_was  = "DevBr";
$src_is   = "HdgHold";
$dest_was = "WAS";
$dest_is  = "IS";

$from = "";
$to = "";
while (<>) {
    if ($_ =~ m/(SWDev.*)/i) {
        $from = "c:/u/$src_was/$1";
        $to   = $dest_was;
        #print "From: $from\n";
        #print "To  : $to\n";
        copy($from, $to) or print "copy failed: $! -> $1\n";
        #copy("sourcefile","destinationfile") or die "Copy failed: $!";        
        #print "was: $from\n";

        $from = "c:/www/$src_is/$1";
        $to   = $dest_is;
        #print "From: $from\n";
        #print "To  : $to\n";
        copy($from, $to) or print "copy failed: $! -> $1\n";
        #print "is: $from\n\n";
        
    }
}
