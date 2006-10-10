#!/usr/bin/perl
use Axigen::Cli ;


my $s = Axigen::Cli::new() ;
$s->Open("gigi", 7000 );
$s->Login("admin", "cucu");
$s->Domain->Create("domain1", "/home/dummy/rtd/axigen/testDomain1", "pass");
