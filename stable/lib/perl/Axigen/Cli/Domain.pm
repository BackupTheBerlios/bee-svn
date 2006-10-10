# ,-----------------.
# | Domain Class    |
# `-----------------'

package Axigen::Cli::Domain ;
@ISA = ("Axigen::Cli");
sub new {
    my ( $sock ) = @_;
    $class = __PACKAGE__ unless @_;
    $this->{sock_fd}    = $sock ;
    bless $this;
    return $this ;
}

# CREATE Domain name <name> domainLocation <path> postmasterPasswd <pass>
sub Create {
    my $this = shift;
    my ( $name, $path, $pass ) = @_ ;
    print "Call create Domain\n" ;
    $this->Write("CREATE Domain name $name domainLocation $path postmasterPasswd $pass\r\n" );
    $this->Read('') ;
    $this->Commit() ;
}

# REGISTER Domain domainLocation <path>
sub Register {
    my $this = shift ;
    my $path = shift ;

    $this->Write("REGISTER Domain domainLocation $path\r\n");
    $this->Read('');
    $this->Commit() ;
}
