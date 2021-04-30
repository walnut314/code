#!/usr/bin/perl

#use strict;
#use warnings;
use IO::Handle;
use Win32::SerialPort;

$PORT = "com4"; #customize this! Find it in device manager.
$BAUD = 3000000;

$ob = new Win32::SerialPort ($PORT, $quiet)
     || die "Can't open $PORT $^E\n";    # $quiet is optional
     
# similar
$ob->baudrate($BAUD);
$ob->parity("none");
$ob->parity_enable(1); 	# for any parity except "none"
$ob->databits(8);
$ob->stopbits(1);
$ob->handshake('rts');
$ob->write_settings;
$ob->save("tpj4.cfg");

while (1) {
    ($count_in, $string_in) = $ob->read();
    #warn("read unsuccessful\n") unless ($count_in == $InBytes);
    print($string_in);
}

