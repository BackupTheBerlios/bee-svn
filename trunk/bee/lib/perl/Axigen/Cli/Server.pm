#----Server Class
package Axigen::Cli::Server ;
require strict;
require Exporter;
use     warnings ;
use     Axigen::Cli ;
use     Axigen::Cli::Pop3 ;


@ISA = ("Axigen::Cli") ;

sub new {
    my ($sock) = @_ ;
    $class = __PACKAGE__ ;

    print "Called $class\n" ;
    $this->{sock_fd}    = $sock ;
    $this->{compat}     = 0 ;
    $x = [] ;
    $x->[0] = Cli::Pop3::new($sock) ;
    bless $this,$class;
    return $this ;
}

sub Pop3 {
    my $this = shift ;
    @_ ? ($x->[0] = shift) : $x->[0];
}

sub Test {
    print "Called Server::Test\n";
}
