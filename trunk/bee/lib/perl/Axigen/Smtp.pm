#----------------------------------------------------------------------
#    Description:   Smtp class.
#    Author     :   Mihai Vlad <mihai.vlad@axigen.com>
#               :   Marius Negreanu <marius.negreanu@axigen.com>
#    Category   :   Testing utility.
#    Created at :   Tue May 29 16:46:32 2006
#    Computer   :   mihaiv.gecadco.local
#    System     :   Gentoo on Intel(R) Pentium(R) 4 CPU 2.66GHz
#
#    Copyright (c) Gecad Technologies
#-----------------------------------------------------------------------
package Axigen::Smtp;

use warnings;
use Axigen::Socket;

require strict;
require Exporter;

@ISA = ("Axigen::Socket");
my $true = 1;
my $false = 0 ;

sub new {
    my($class, $name) = @_;
    my $self = Axigen::Socket::new($class);  # returns an empty "strict" object
    $self->{debug} = $true ;          # attributes get accessed as usual
    $self->{behave} = "smtp" ;          # attributes get accessed as usual
    bless $self ;
    return $self;                  # $self is already blessed
}



=item Writeread( message, etag )
    Combines Write and Read in a single function.
    Write will send `message`, and Read will
    receive until etag is matched.

    If `Write` or `Read` fails, their error codes
    are returned by Writeread.
    Overriten to suit the smtp module
=cut
sub WriteRead
{
    my ( $this, $message, $etag ) = @_;
    my $prototag = '\d{3} ';

    if( @_ < 3 ){ warn $ERR_CODE{BAD_ARGV} ; return $false ; }

    if (!defined($this->{sock_fd}))   { warn $ERR_CODE{BAD_HAND} ; return $false ; }
    if (!defined($message))           { warn $ERR_CODE{BAD_ARGV} ; return $false ; }

    my $rtcode = $this->Write( $message ) ;     # test this for error
    if( !$rtcode ) {return $false ;}

    $rtcode = $this->Read( $prototag ) ;    #test for read error
    if( !$rtcode ) {return $false ;}

    #if we don't have the correct return code:
    if (( $this->{resp} !~ /^$etag/)&&( $this->{resp} !~ /\r\n$etag/ ) )
    {
        warn("Incorrect response tag, expected: $etag and got: $this->{resp}");
        return $false
    }
    return $true
}


=item Ehlo( message, [ etag] ) {{{
	Sends ehlo message on the socket and reads the answer
	and returns true or false if the answer is according to
	the given tag or the default tag
=cut
sub Ehlo
{
    my ($this, $d) = @_;
    if( @_ < 2 ) { print "Invalid params for Ehlo\n" ; }
    $this->WriteRead( "ehlo $d\r\n", '250 ' );
}#}}}



=item MailFrom( message, [ etag] ) {{{
	Sends mail from:<message> on the socket and reads the answer
	Returns true or false if the answer is according to
	the given tag or the default tag
=cut
sub MailFrom
{
	my $this = shift;
	# taking the default code for ehlo
	my $etag = '250 ';
	if (defined( $_[1] ) )
	{
		$etag = $_[1];
	}
	return $this->WriteRead( "mail from:<$_[0]>\r\n", $etag );
}#}}}



=item RcptTo( message, [ etag] ) {{{
	Sends rcpt to:<message> on the socket and reads the answer
	Returns true or false if the answer is according to
	the given tag or the default tag
=cut
sub RcptTo
{
	my $this = shift;
	# taking the default code for ehlo
	my $etag = '250 ';
	if (defined( $_[1] ) )
	{
		$etag = $_[1];
	}
	return $this->WriteRead( "rcpt to:<$_[0]>\r\n", $etag );
}#}}}



=item Reset([ etag] ) {{{
	Sends reset on the socket and reads the answer
	Returns true or false if the answer is according to
	the given tag or the default tag
=cut
sub Reset
{
	my $this = shift;
	# taking the default code for ehlo
	my $etag = '250 ';
	if (defined( $_[0] ) )
	{
		$etag = $_[0];
	}
	return $this->WriteRead( "reset\r\n", $etag );
}#}}}



=item Data( message, [ etag] ) {{{
	Sends data\r\n<message>\r\n.\r\n on the socket and reads the answer
	Returns true or false if the answer is according to
	the given tag or the default tag
=cut
sub Data
{
	my $this = shift;
	# taking the default code for ehlo
	my $etag = '354 ';
	if (defined( $_[1] ) )
	{
		$etag = $_[1];
	}
	#begin of data
	my $rc =  $this->WriteRead( "data\r\n", $etag );
    if( $rc eq 0 )
    {
        warn "Can't send `DATA`.";
        return $false;
    }

	#end of data
	return $this->WriteRead( "$_[0]\r\n.\r\n", '\d{3} ' );
}#}}}



=item DataFromFile(file[ etag] ) {{{
	Sends data with the data content red from a file on the socket and reads the answer
	Returns true or false if the answer is according to
	the given tag or the default tag
=cut
sub DataFromFile
{
	my $this = shift;
	# taking the default code for ehlo
	my $etag = '354 ';
	if (defined( $_[1] ) )
	{
		$etag = $_[1];
	}
	#begin of data
	my $rtcode =  $this->WriteRead( "data\r\n", $etag );
	$rtcode || return $false;

	#reading the file
	my $file = '';
	if (! open( $file, "<$_[0]" ) )
	{
		warn "ERROR: Couldn't open $_[0]";
		return $false;
	}
	#sending content's to the socket
	while (<$file>)
	{
		$rtcode = $this->Write($_);

		$rtcode || return $false; #something wrong went while reading the file
	}

	#end of data
	return $this->WriteRead( "\r\n.\r\n", '250 ' );
}#}}}



=item Quit([ etag] ) {{{
	Sends quit on the socket and reads the answer
	Returns true or false if the answer is according to
	the given tag or the default tag
=cut
sub Quit
{
	my $this = shift;
	# taking the default code for ehlo
	my $etag = '221 ';
	if (defined( $_[0] ) )
	{
		$etag = $_[0];
	}
    $this->WriteRead("quit\r\n", $etag );
}#}}}
1;
