#!/usr/bin/perl

use HTTP::Tiny;
use HTML::TreeBuilder;

my $http = HTTP::Tiny->new();
my $response = $http->get('https://www.scrapingcourse.com/ecommerce/');

my $html_content = $response->{content};


print($html_content);

