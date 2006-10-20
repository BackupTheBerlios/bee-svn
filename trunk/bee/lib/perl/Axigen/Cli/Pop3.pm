#----Server Pop3
package Cli::Pop3 ;
use Axigen::Cli::Listener ;
@ISA = ("Axigen::Cli");

sub new {
    my ( $sock) = @_ ;
    $class = __PACKAGE__ unless @_;
    $this->{sock_fd}    = $sock ;
    $this->{compat}     = 0 ;
    $z = [] ;
    $z->[0] = Axigen::Cli::Listener::new($sock ) ;
    
    bless $this;
    return $this ;
}

sub Listener {
    my $this = shift ;
    @_ ? ($z->[0] = shift ) : $z->[0] ;
}

sub Test {
    my $this = shift ;

    print "Called Pop3::Test\n" ;
}
