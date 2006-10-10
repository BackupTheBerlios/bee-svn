#!/usr/bin/perl
#---------------------------------------------------------------#
# Send a message with <LF>.<LF> at the end (Which end ???)      #
# Author: Daniel Carlan                                         #
# Created at: 16:20:22, Date: 01.04.2005                        #
# Computer: danielca.gecadco.local                              #
# System: Linux 2.4.26-gentoo-r9 on i686                        #
#                                                               #
# Copyright (c) 2005 Gecad Technologies All Rights reserved.    #
#                                                               #
#---------------------------------------------------------------#

use Axigen::Utils ;
use Axigen::Smtp;
my $TIME_NECESSARY_TO_SEND_A_MAIL = 2 ;

$s = asmtp->new() ;
$s->Open( $a_env{host}, $a_env{smtp});
$s->Ehlo("cucu");
$s->MailFrom( $a_env{mbox} );
$s->RcptTo( $a_env{mbox} ) ;
$s->Data("Subject: test\n\nTest line\n.\n");
$s->Quit() ;
$s->Close() ;

sleep $TIME_NECESSARY_TO_SEND_A_MAIL;

$p = asocket->new() ;
$p->Open( $a_env{host}, $a_env{pop} ) ;
$p->WriteRead("user $a_env{mbox}\r\n", '\+OK ' );
$p->WriteRead("pass $a_env{pass}\r\n", '\+OK ' );
$p->WriteRead("stat\r\n", '\+OK ' );
@tmp = split / /, $p->{resp} ;
$p->Write("retr $tmp[1]\r\n") ;
$p->MatchLines( '', '\.') ;
$p->Close() ;

$tag = 0;
@tmp = split /\x0D\x0A/, $p->{resp};
foreach $line (@tmp) {
    $tag = 1 if ($line =~ m/^\.\.\x0D?\x0A?$/);
}
if ($tag ne 1) {
    warn "search <LF>.<LF>\tno <LF>.<LF> found\n" ;
    exit a_fail() ;
}
exit a_ok() ;
