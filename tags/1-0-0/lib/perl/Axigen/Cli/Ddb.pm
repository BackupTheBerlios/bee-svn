#----DDB Class--------------
package Axigen::Cli::Ddb;
@ISA = ("Axigen::Cli");
sub new {
    my ( $sock ) = @_;
    $class = __PACKAGE__ unless @_;

    $this->{sock_fd}    = $sock ;
    $this->{compat}     = 0 ;

    bless $this;
    return $this ;
}

sub Create {
    my $this = shift;
    my ( $path, $id ) = @_ ;
    print "Call create\n" ;
    $this->Config("SERVER");
    $this->Write("CREATE domainData path $path id $id\r\n" );
    $this->Read('') ;
    $this->Commit() ;
}

sub Register {
    my $this = shift ;
    my $path = shift ;

    $this->Config("SERVER");
    $this->Write("REGISTER domainData $path\r\n");
    $this->Read('');
    $this->Commit() ;
}
