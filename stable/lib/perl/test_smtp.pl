#!/usr/bin/perl
use Axigen::Smtp ;


my $s = Axigen::Smtp::new() ;
$s->Open("gigi", 2500 );
$s->Ehlo("cucu");
$s->MailFrom('user1@localdomain');
$s->RcptTo('user1@localdomain');
$s->Data("Subject: Test\r\n\r\nOneLiner");
$s->Quit() ;
