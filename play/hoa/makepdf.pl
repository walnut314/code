#!/usr/bin/perl

use strict;
use warnings;
use PDF::Create;

# https://metacpan.org/pod/PDF::Create::Page

#my $text = <<'EOT';
#Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do
#eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad
#minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliqui
#+p
#ex ea commodo consequat. Duis aute irure dolor in reprehenderit in
#voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur
#sint occaecat cupidatat non proident, sunt in culpa qui officia
#deserunt mollit anim id est laborum.
#EOT

#my $text = <STDIN>;
my $text = do { local $/; <> };

my $pdf = new PDF::Create('filename' => 'mydoc.pdf',
                          'Title'    => 'Simple Demo',
                        );

my $root = $pdf->new_page('MediaBox' => $pdf->get_page_size('A4'));
my $font = $pdf->font();   # using defaults
my $page = $root->new_page();

$page->printnl($text, $font, 14, 120, 750);  # text, font, size, x, y

$pdf->close;

