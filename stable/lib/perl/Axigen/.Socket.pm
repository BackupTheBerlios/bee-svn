#----------------------------------------------------------------------
#    Description:   Regular expression operations on socket
#    See also   :   Smtp.pm, Utils.pm, Cli.pm
#    Author     :   Marius Negreanu <marius.negreanu@axigen.com>
#    Category   :   .
#    Created at :   Tue Jun 27 18:50:15 EEST 2006
#    Computer   :   mariusn.gecadco.local
#    System     :   GNU/Debian Unstable 2.6.16 on i686
#
#    Copyright (c) Gecad Technologies
#-----------------------------------------------------------------------

package Axigen::Socket ;
require strict;
require Exporter;
use     warnings;
use     IO::Select;
use     Socket ;
do      IO::Socket::SSL ;

%ERR_CODE = (   WRITE_TIMEOUT   => "ERROR: Timeout when trying to write to socket >",
                READ_TIMEOUT    => "ERROR: Timeout when trying to read from the socket >",
                NO_TAG          => "ERROR: Timeout on tag >",
                BAD_HAND        => "ERROR: Bad socket handler >",
                BAD_ARGV        => "ERROR: Not enough parameters for function >",
                BAD_LINE        => "ERROR: Readline >",
            );

my $true   = 1 ;
my $false  = 0 ;

our $server_key = "" ;


=item Socket::new( )
    Socket class contructor.
=cut
sub new {
    my $proto = shift;
    my $class = __PACKAGE__ ;
    my $this  = {};

    $this->{sock_fd}    = undef ;
    $this->{message}    = undef ;   # save the sent message
    $this->{timeout}    = 5 ;       # 5 seconds timeout
    $this->{behave}     = "xml" ;   # xml -maintain backward compat with xml. smtp - compat with asmtp
    $this->{debug}      = $true ;   # verbose debug messages
    $this->{host}       = undef ;   # host to connect to
    $this->{port}       = undef ;   # port to connect to
    $this->{resp}       = undef ;   # save the servers' reponse
    $this->{err}        = 0 ;       # number of errors
    $this->{key}        = undef ;   # used with pop3

    my $p   = getprotobyname('tcp');
    socket($this->{sock_fd}, PF_INET, SOCK_STREAM, $p)  || die "socket: $!";
    bless $this, $class ;
    return $this;
}



=item dprint( message )
    Print a "DEBUG: text"
=cut
sub dprint {
    my ($this, $msg ) = @_ ;

    if( !defined($this->{debug})){ return $false; } # Workaround for a warning
    if( $this->{debug} == $true )
    {
        print "DEBUG: $msg" ;
    }
}



=item Open ( host, port )
    Connect to "host:port".
    Set the socket timeout to "timeout", if it's specified,
    otherwise, it uses 5[sec] as default timeout.
    RETURNS: (-1|undefined|socket fd)
=cut
sub Open {
    my ($this, $host, $port ) = @_;
    if( @_ < 3 )                     { warn "$ERR_CODE{BAD_ARGV}" ; return $false ; }
    if( !defined($this->{sock_fd}) ) { warn "$ERR_CODE{BAD_HAND}" ; return $false ; }

    if ($port =~ /\D/) { $port = getservbyname($port, 'tcp') }
    die "No port specified" unless $port;
    $iaddr   = inet_aton($host) || die "Don't know about host: $host";
    $paddr   = sockaddr_in($port, $iaddr);

    connect($this->{sock_fd}, $paddr) || die "connect: $!";

    my $bann = "" ;
    sysread( $this->{sock_fd}, $bann, 2500 ) ;
    $this->dprint ("$bann");
    my @tmp=split(/ /, $bann);
    if (@tmp>5)
    {
        $server_key = $tmp[6];
        chop($server_key); chop($server_key);
        $this->{key} = $server_key ;
    }
    return $true ;
}



=item Close()
    closes the socket handle
=cut
sub Close
{
    my $this = shift ;
    ( defined $this->{sock_fd} ) && ( close($this->{sock_fd}) );
}






=item Read( etag, line_regex )
    Receive text from socket.( async way )
    It uses `etag` to know when the server is done sending.
    Ex: `\.` might be considered an etag for POP3
    TODO: assign error code
    Behave XML : Matches each line with line_regx
                 Until etag is reached.
    SMTP Behaviour : Wait until etag is reached.
                    And return true. The smtp side will re-extract the tag
                    and see if it is an error or not.
=cut
sub Read {
    my $this = shift ;
    my $etag = $_[0] || '' ;
    my $line_regx = $_[1] || '' ;

    if( !defined($this->{sock_fd}) ) { warn $ERR_CODE{BAD_HAND} ; return $false ; }

    my $line = $tmsg = $this->{resp} = "";
    my $selSk = IO::Select->new();
    $selSk->add($this->{sock_fd});

    $line_regx =~ s/\\\\/\\\\\\\\/g ;
    $etag =~ s/\\\\/\\\\\\\\/g ;

    while( 1 )
    {
        $optSk = $selSk->can_read($this->{timeout});
        if(!$optSk) {
            if($etag) { warn "$ERR_CODE{NO_TAG}$etag| got |$line|" ; return $false; }; # Replace warn with a entryLog
            warn $ERR_CODE{TIMEOUT} ;
            last ;
        }

        $tmsg = $line = $this->__readLine($this->{sock_fd});
        if( !defined($line) || ($line eq "")) { warn $this->{resp} ; return $false ; }
        $this->{resp} .= $line;
        $tmsg =~ s/\r\n/\\r\\n/ ; # make \r and \n visible to user

        # Backward compatibility with XMLs 
        if( $this->{behave} eq "xml" ) {
            if( $line =~ m/$line_regx/ ) {
                $this->dprint( "< OK |$tmsg\n" );
            }elsif( $line =~ m/$etag/ )
            {
                $this->dprint( "< END|$tmsg\n" );
                return $true ;
            }else
            {
                $this->dprint( "< -- |$tmsg\n" );
                last ;
            }
        }
        # Aware of the new behaviour
        else {
              $this->dprint( "READ:<$etag> $tmsg\n" );
              if ( defined($etag) && $line =~ /^$etag/ ) {
                $this->dprint( "READ: END|$tmsg\n" );
                return $true ;
              }
        }
    }
    return $false ;
}



=item Write( message )
    Send "message" on socket.
    "message" is saved in $this->{message}, in case
    a test FAILS, the user knows what was sent.
    TODO: assign error code
=cut
sub Write {
    my ($this, $message) = @_ ;

    if ( @_ < 2 ) { warn $ERR_CODE{BAD_ARGV} ; return $false ; }
    if( !defined( $this->{sock_fd} )) { warn "$ERR_CODE{BAD_HAND}"; return $false ; }

    my $selSk = IO::Select->new();
    $selSk->add( $this->{sock_fd} );

    my $optSk = $selSk->can_write( $this->{timeout} );
    if (!$optSk) { warn $ERR_CODE{WRITE_TIMEOUT} ; return $false; } ;

    syswrite($this->{sock_fd}, $message ) ;
    $this->{message} = $message ;# save the message sent, for error tracking

    # make \r\n visible to tester
    $message =~ s/\r\n/\\r\\n/g ;
    $this->dprint( "$message\n" ) ;
    return $true ;
}



sub ReadLine {
    my ($this,$match) = @_ ;
    if( @_ <2 ) { warn $ERR_CODE{BAD_ARGV} ; return $false ; }

    my $tmsg = $this->{resp} = "";

    $tmsg = $this->{resp} = $this->__readLine($this->{sock_fd});
    $tmsg =~ s/\r\n/\\r\\n/g ;

    if( $this->{resp} =~ /$match/ )
    {
        $this->dprint("< OK|$tmsg\n" );
        return $true ;
    }else
    {
        $this->dprint("< -->$tmsg\t$match\n" );
        warn "< -->$tmsg\t$match\n" ;
        return $false ;
    }
    return $true ;
}

sub WriteRead
{
    my $this = shift ;
    # TODO save message , and the whales
    my ( $message, $etag ) = @_;

    if (!defined($this->{sock_fd}))   { warn $ERR_CODE{BAD_HAND} ; return $false ; }
    if (!defined($message))           { warn $ERR_CODE{BAD_ARGV} ; return $false ; }

    my $rtcode = $this->Write( $message ) ;     # test this for error
    if( !$rtcode ) {return $false ;}

    return $this->Read( $etag ) ;
}


sub __readLine {
    my $this = shift ;
    my $c="";
    my $retstr='';
    my $endoffile=0;

    # TODO: pls pls pls optimize
    # old version : while ($c ne "\n" && ! $endoffile) {  and sysread of 1 char
    if( !defined($this->{sock_fd} ) ) { warn "$ERR_CODE{BAD_HAND}"; return $false ; }

    my $selSk = IO::Select->new();
    $selSk->add($this->{sock_fd});
    $optSk = $selSk->can_read($this->{timeout});
    if(!$optSk)
    {
        if($etag) { warn "$ERR_CODE{NO_TAG}$etag| got |$line|" ; return $false ;};
        warn $ERR_CODE{READ_TIMEOUT} ;
        return $false ;
    }

    while ( $c ne "\n" && !$endoffile) {
        if (sysread($this->{sock_fd}, $c, 1 ) > 0) {
            $retstr = $retstr . $c;
        }
        else {
            $endoffile=1;
        }
    }

    if ((!defined($retstr)) || ($retstr eq "")) {  warn $ERR_CODE{BAD_LINE}; return $false ; };
    return $retstr;
}



1;

