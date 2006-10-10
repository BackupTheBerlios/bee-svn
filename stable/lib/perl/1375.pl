#!/usr/bin/perl
use Axigen::Utils ;
use Axigen::Cli ;


$cli =Axigen::Cli->new() ;
$cli->{debug} = 1 ;
$cli->Open($a_env{host}, $a_env{cli});
$cli->Login("admin", $a_env{adminpass});
my @listeners = $cli->Server->Pop3->Listener->List() ;
$cli->Server->Pop3->Listener->Add($listeners[0]{address}) ;
$cli->SaveConfig() ;
$cli->Quit() ;

print "+++$listeners[0]{enable}----\n" ;
print "+++$listeners[0]{address}---\n" ;
a_fail($cli->{err} > 0, "FAIL");
exit a_ok() ;

