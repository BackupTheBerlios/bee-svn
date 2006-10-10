#----------------------------------------------------------------------
#    Description:   Pop3 Class.
#    See also   :   Smtp.pm, Utils.pm, Cli.pm, Socket.pm
#    Author     :   Marius Negreanu <marius.negreanu@axigen.com>
#    Category   :   .
#    Created at :   Tue Jun 27 18:50:15 EEST 2006
#    Computer   :   mariusn.gecadco.local
#    System     :   GNU/Debian Unstable 2.6.16 on i686
#
#    Copyright (c) Gecad Technologies
#-----------------------------------------------------------------------
package Axigen::Pop3;

use warnings;
use Axigen::Socket;

require strict;
require Exporter;

@ISA = ("Axigen::Socket");

$true = 1;
$false = 0;

sub new {
    my($class, $name) = @_;
    my $self = asocket::new($class);  # returns an empty "strict" object
    $self->{compat} = 0 ;          # attributes get accessed as usual
    return $self;                  # $self is already blessed
}



sub Login
{
	my $this = shift;
    my ($user, $pass ) = @_ ;


	$rc = $this->Write( "USER $user\r\n") ;
    $rc = $this->Read( '\+OK' );
    $rc = $this->Write( "PASS $pass\r\n" ) ;
    $rc = $this->Read( '\+OK' );
}#}}}



=item Quit([ etag] ) {{{
	Sends quit on the socket and reads the answer
	Returns true or false if the answer is according to
	the given tag or the default tag
=cut
sub Quit
{
	my $this = shift;
	my $etag = '\+OK';
	return $this->WriteRead( "quit\r\n", $etag );
}#}}}
sub Stat
{
    my $this = shift ;
    my @rc ;

    $this->Write( "STAT\r\n");
    $this->Read( '\+OK \d+ \d+' );
    my @t = split(/ /, $this->{resp} );
    push( @rc, $t[1] ) ;
    chop( $t[2] ); chop( $t[2] );
    push( @rc, $t[2] ) ;
    return @rc ;
}

sub Dele {
    my $this = shift ;
    my $mesg = $_[0] ;
    $this->Write("DELE $mesg\r\n");
    $this->Read('\+OK');
}

sub List {
    my $this = shift ;
    
    $this->Write("LIST\r\n" );
    $this->Read('\+OK') ;
    $this->MatchLines('\d+ \d+','\.' ) ;
}


sub Noop {
    my $this = shift ;

    $this->Write("NOOP\r\n");
    $this->Read('\+OK') ;
}

sub Retr {
    my $this = shift ;
    my $mesg = shift ;

    $this->Write("RETR $mesg\r\n" );
    if( $this->Read('\+OK' ) ) {
         $this->MatchLines('', '\.');
    }
}
sub Top {
    my $this = shift ;
    my ($mesg, $lines ) = @_ ;

    $this->Write("TOP $mesg $lines\r\n" ) ;
    if( $this->Read('\+OK' ) ) {
        $this->MatchLines('', '\.') ;
    }
}

1;
