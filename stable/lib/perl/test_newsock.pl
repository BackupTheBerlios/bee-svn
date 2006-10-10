#!/usr/bin/perl
use Axigen::Socket ;


my $s = Axigen::Socket::new() ;
$s->Open("gigi", 2500 );
$s->Write("Ehlo cucu\r\n");
$s->Read('250 ','250-');
