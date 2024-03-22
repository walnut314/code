#!/usr/bin/perl -w

use Finance::QuoteHist::Yahoo;

$q = new Finance::QuoteHist::Yahoo
     (
      symbols    => [qw(SPY IBM UPS AMZN WDC)],
      start_date => '03/01/2024',
      end_date   => 'today',
     );

# Values
foreach $row ($q->quotes()) {
  ($symbol, $date, $open, $high, $low, $close, $volume) = @$row;
    print "$symbol, $date, $open, $high, $low, $close, $volume \n";
}



#$q = new Finance::QuoteHist::Yahoo
#   (
#    symbols    => [qw(IBM UPS AMZN)],
#    start_date => '01/01/2009',
#    end_date   => 'today',
#   );
# 
# Values
#foreach $row ($q->quotes()) {
#  ($symbol, $date, $open, $high, $low, $close, $volume) = @$row;
#  print "$row\n";
#}
 
# Splits
#foreach $row ($q->splits()) {
#   ($symbol, $date, $post, $pre) = @$row;
#}
 
# Dividends
#foreach $row ($q->dividends()) {
#   ($symbol, $date, $dividend) = @$row;
#}

