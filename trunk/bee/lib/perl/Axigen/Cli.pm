#----------------------------------------------------------------------
#    Description:   Cli Class.
#    See also   :   Smtp.pm, Utils.pm, Pop3.pm, Socket.pm
#    Author     :   Marius Negreanu <marius.negreanu@axigen.com>
#    Category   :   .
#    Created at :   Tue Jun 27 18:50:15 EEST 2006
#    Computer   :   mariusn.gecadco.local
#    System     :   GNU/Debian Unstable 2.6.16 on i686
#
#    Copyright (c) Gecad Technologies
#-----------------------------------------------------------------------
# Maintenance Notes:
# Adding a new Cli Context HOWTO:
#   First include the new module:
#       use Axigen::Cli::List ;
#       List.pm file will reside in the directory Axigen/Cli,
#       and will hold the functions available from `context List`
#   You start by adding a new element to the $r array.
#   The new elements is an object returned by the contructor
#   of the Context.
#   Ex: Adding Lists.
#       $r->[3] = Axigen::Cli::List::new( $this->{sock_fd} ) ;
#   Then, you let perl know how to call the new object:
#
#   ,------------------.
#   | List object      |
#   `------------------'
#   sub Domain {
#       my $this = shift ;
#       @_ ? ($r->[2] = shift) : $r->[2];
#   }
# The next thing is to implement your functions inside List.pm
package Axigen::Cli;
require strict;
require Exporter;
use     warnings ;
use     Carp;
use     Axigen::Socket ;
use     Axigen::Cli::Ddb ;
use     Axigen::Cli::Server ;
use     Axigen::Cli::Domain ;

@ISA = ("Axigen::Socket");
sub new {
    my ($class) = @_;
    $class  = __PACKAGE__ ;

    print "DBG:Call new-".$class."\n" ;
    $r      = [];
    $this   = Axigen::Socket::new($class) ;
    $class->{behave} = "cli" ;
    $class->{debug} = $false ;

    $r->[0] = Axigen::Cli::Ddb::new($this->{sock_fd} );
    $r->[1] = Axigen::Cli::Server::new( $this->{sock_fd} ) ;
    $r->[2] = Axigen::Cli::Domain::new( $this->{sock_fd} ) ;
    #$this->{compat} = 0 ;
    #$this->{err}    = 0 ;

    bless $this, $class;
    return $this ;
}


sub Read {
    my $this = shift ;
    my $resp = "" ;
    my $done = 0 ;
    my $err  = 0 ;

    do
    {
        sysread( $this->{sock_fd}, $resp, 1000) ;
        print "$resp" ;
        $this->{resp} = $resp ;
        if( $resp =~ /-ERR/ )
        {
            $err++ ;
        }
        if( $resp =~ m/<.*>/ )
        {
            $done = 1 ;
        }
    }while( $done eq 0 ) ;
    $this->{err} += $err ;
    print "\nDBG:Read >$this->{err}< Errors encountered\n" ;
}



sub Login {
    my $this = shift;
    my ($user, $pass ) = @_ ;

    $this->Write("USER $user\r\n") ;
    $this->Read('');
    $this->Write("$pass\r\n" ) ;
    $this->Read('' );
}

sub SaveConfig {
    my ($this, $path) = @_ ;

    $this->Enter("commands");
    $this->Enter("server");
    if( defined($path) && $path ne "" ) {
        $path = " ".$path ;
    }else
    {
        $path="";
    }
	$this->Write("save config$path\r\n");
    $this->Read('') ;
}

sub Config {
    my ($this, $what)= @_ ;

    $this->Write("CONFIG $what\r\n" ) ;
    $this->Read('');
}

sub ShowConfig {
    my $this = shift ;

    my $what = shift ;
    $this->Write("enter commands\r\n");
    $this->Read('');
    $this->Write("enter server\r\n");
    $this->Read('');
    $this->Write("SHOW CONFIG\r\n");
    $this->Read('');
    $this->Back(2) ;
}

sub Commit {
    my $this = shift ;

    $this->Write( "COMMIT\r\n" ) ;
    $this->Read('');
}

sub List {
    my $this = shift ;
    my $lstp = shift ;

    $this->Write( "LIST $lstp\r\n" ) ;
    $this->Read('' ) ;
}

sub Set {
    my ($this,$parm,$val) = @_ ;

    $this->Write("SET $parm $val\r\n" ) ;
    $this->Read('' ) ;
}

sub SetPasswd {
    my $this = shift ;
    my $pass = shift ;

    $this->Write("enter commands\r\n" );
    $this->Read('');
    $this->Write("SET passwd $pass\r\n" ) ;
    $this->Read('');
    $this->Back() ;
}

sub SetServices {
    my $this = shift ;
    my @services = @_ ;
    my $a =  "" ;

    $a = "(".join(" ",@services).")" ;
    $this->Write("config server\r\n");
    $this->Read('');
    $this->Write("SET SERVICES $a\r\n" ) ;
    $this->Read('');
    $this->Commit() ;
}

sub Help {
    my $this = shift ;

    $this->Write("HELP\r\n");
    $this->Read('');
}

sub Back {
    my $this = shift ;
    my $nr   = shift ;

    if( !defined($nr) ) { $nr = 1 ; }
    for( $i =0; $i < $nr ; $i++) {
        $this->Write("BACK\r\n");
        $this->Read('');
    }
}

sub Enter {
    my ($this, $ctx) = @_ ;

    $this->Write("ENTER $ctx\r\n" );
    $this->Read('');
}

sub Quit {
    my $this = shift ;

    $this->Write("QUIT\r\n" ) ;
    $this->ReadLine('');
    $this->ReadLine('');
    $this->Close() ;
}


# ,---------------.
# | DDB object    |
# `---------------'
sub Ddb {
    my $this = shift ;
    @_ ? ($r->[0] = shift) : $r->[0];
}


# ,------------------.
# | Server object    |
# `------------------'
sub Server {
    my $this = shift ;
    @_ ? ($r->[1] = shift) : $r->[1];
}

# ,------------------.
# | Domain object    |
# `------------------'
sub Domain {
    my $this = shift ;
    @_ ? ($r->[2] = shift) : $r->[2];
}

1;
