#!/usr/bin/perl
use strict;
use warnings;
 
# https://perlmaven.com/create-an-excel-file-with-perl

use Excel::Writer::XLSX;
 
my $workbook  = Excel::Writer::XLSX->new( 'simple.xlsx' );
my $worksheet = $workbook->add_worksheet();
 
$worksheet->write( "A1", "Hi Excel!" );
$worksheet->write( "A2", "second row" );
 
$workbook->close;

