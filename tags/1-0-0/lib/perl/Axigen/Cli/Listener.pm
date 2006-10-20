#----------------------------------------------------------------------
#    Description:   Listener
#    See also   :   Smtp.pm, Utils.pm, Cli.pm
#    Author     :   Marius Negreanu <marius.negreanu@axigen.com>
#    Category   :   .
#    Created at :   Tue Jun 27 18:50:15 EEST 2006
#    Computer   :   mariusn.gecadco.local
#    System     :   GNU/Debian Unstable 2.6.16 on i686
#
#    Copyright (c) Gecad Technologies
#-----------------------------------------------------------------------

package Axigen::Cli::Listener ;
require strict;
require Exporter;
use     warnings ;
use     Axigen::Cli ;

@ISA = ("Axigen::Cli");

sub new {
    my ( $sock ) = @_;
    $class = __PACKAGE__;

    print "DBG:Call new-".$class."\n" ;
    $this->{sock_fd}    = $sock ;
    $this->{compat}     = 0 ;
    $this->{err}        = 0 ;
    
    bless $this,$class;
    return $this ;
}

sub EnterContext {
    my $this = shift ;


    print "\nDBG:EnterContext >$this->{err}< Errors encountered\n" ;

    print "Called EnterContext\n";
    $this->Write("CONFIG SERVER\r\n");
    $this->Read('');
    $this->Write("CONFIG POP3\r\n");
    $this->Read('');
}

sub Add {
    my $this = shift ;
    my $a = shift ;

    $this->EnterContext() ;
    $this->Write("ADD LISTENER address $a\r\n");
    $this->Read('');
    $this->Commit() ;
    $this->Commit() ;
}

sub Update {
    my $this = shift ;
    my $a = shift ;

    $this->Write("UPDATE LISTENER address $a\r\n");
    $this->Read('');
}

sub Remove {
    my $this = shift ;
    my $a = shift ;

    $this->Write("REMOVE LISTENER address $a\r\n");
    $this->Read('');
}

sub Show {
    my $this = shift ;
    my $a = shift ;

    $this->EnterContext() ;
    $this->Write("SHOW LISTENER address $a\r\n");
    $this->Read('');
}


# TODO: return an array of hash ?
# One Array record Holds the description of a listener( Address: enable )
sub List {
    my $this = shift ;
    my @listeners = () ;
    my $tmp ;

    $this->EnterContext() ;
    $this->Write("LIST LISTENERS\r\n") ;
    $this->Read('');
    my @arr = split(/\r\n/,$this->{resp} ) ;
    my @x   = grep ( /\ \| (yes\s+$|no\s+$)/, @arr ) ;
    my @a ;
    foreach (@x) {
        my %h ;
        @k = split(/ \| /);
        $k[1] =~ s/\s//g;
        $h{enable} = $k[1];
        $h{address} = $k[0];
        push(@a, \%h);
    }
    $this->Back(2) ;
    return @a ;
}

sub LogType {
    my $this = shift ;
    my $type = shift ;

    $this->EnterContext() ;
    $this->Write("SET logType $type\r\n" );
    $this->Read('');
}



sub MaxErrors {
    my $this = shift ;
    my $max  = shift ;

    $this->Write("SET maxErrors $max\r\n");
    $this->Read('');
}

sub MaxAuthErrors {
    my $this = shift ;
    my $max  = shift ;

    $this->Write("SET maxAuthErrors $max\r\n");
    $this->Read('');
}

sub MaxConnThreads {
    my $this = shift ;
    my $max  = shift ;

    $this->Write("SET maxConnThreads $max\r\n");
    $this->Read('');
}

sub MinConnThreads {
    my $this = shift ;
    my $min  = shift ;

    $this->Write("SET minConnThreads $min\r\n");
    $this->Read('');
}

sub LogLevel {
    my $this = shift ;
    my $lvl  = shift ;

    $this->Write("SET logLevel $lvl\r\n");
    $this->Read('');
}

sub LogHost {
    my $this = shift ;
    my $host = shift ;

    $this->Write("SET logHost $host\r\n");
    $this->Read('');
}
sub AllowStartTLS {
    my $this = shift ;
    my $a    = shift ;

    $this->Write("SET allowStartTLS $a\r\n");
    $this->Read('');
}

sub Reset {
    my $this = shift ;

    $this->Write("RESET LISTENERS\r\n");
    $this->Read('');
}

