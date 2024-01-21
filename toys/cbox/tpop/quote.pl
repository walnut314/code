#!/usr/bin/perl

use Finance::YahooQuote;
use Data::Dumper;

# setting TIMEOUT and PROXY is optional
$Finance::YahooQuote::TIMEOUT = 60;
$Finance::YahooQuote::PROXY = "http://some.where.net:8080";
$symbol = "DELL";
@quote = getonequote $symbol; # Get a quote for a single symbol
@quotes = getquote @symbols;  # Get quotes for a bunch of symbols
useExtendedQueryFormat();     # switch to extended query format
useRealtimeQueryFormat();     # switch to real-time query format
@quotes = getquote @symbols;  # Get quotes for a bunch of symbols
@quotes = getcustomquote(["DELL","IBM"], # using custom format
                         ["Name","Book Value"]); # note array refs
#print @quotes;
print Dumper(@quote);
#oreach (@quotes)
#
#   foreach $item (@_$ARRAYREF) {
#       print "$item\n"; # do something with $item
#   }    
#print($_);
#  @arr = $_;
#  foreach (@$arr) {
#      print "$_\n";
#  }
#
