#!/usr/bin/perl -w
 
#%quotes  = $q->fetch("nasdaq", @stocks);
#%quotes = $q->yahoo ("IBM", "SGI"); 
#print Dumper(\%quotes);

# https://metacpan.org/pod/Finance::Quote
# https://www.foo.be/docs/tpj/issues/vol5_3/tpj0503-0006.html
# usage: ./quote2.pl nyse MSFT

use strict;
use Finance::Quote;
use Data::Dumper;


# usage: ./quotes2.pl INTC IBM IVV

#my $exchange = "NYSE"; #shift;	# Where we fetch our stocks from.
my $exchange = "NASDAQ"; #shift;	# Where we fetch our stocks from.
my @symbols = @ARGV;    # Which stocks we want.

my $quoter = Finance::Quote->new;      # Create the F::Q object.

$quoter->timeout(30);	# Cancel fetch operation if it
				        # takes longer than 30 seconds.

# Grab our information and place it into %info.
my %info = $quoter->fetch($exchange, @symbols);
print Dumper(\%info);

exit;

foreach my $stock (@symbols) {
	unless ($info{$stock, "success"}) {
		warn "Lookup of $stock failed - ".$info{$stock, "errormsg"}.
		     "\n";
		next;
	}
	print "$stock:\t\t",
	      "Volume: ", $info{$stock, "volume"}, "\t",
	      "Price: " , $info{$stock, "open"}, "\n";
}
