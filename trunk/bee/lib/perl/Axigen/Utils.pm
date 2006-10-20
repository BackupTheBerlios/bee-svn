#----------------------------------------------------------------------
#    Description:   Utils Class.
#    See also   :   Pop3.pm, Utils.pm, Cli.pm
#    Author     :   Marius Negreanu <marius.negreanu@axigen.com>
#    Category   :   .
#    Created at :   Tue Jun 27 18:50:15 EEST 2006
#    Computer   :   mariusn.gecadco.local
#    System     :   GNU/Debian Unstable 2.6.16 on i686
#
#    Copyright (c) Gecad Technologies
#-----------------------------------------------------------------------

package Axigen::Utils;

use Axigen::Socket ;
use MIME::Base64;
use Digest::MD5 qw(md5 md5_hex md5_base64) ;
require strict;
require Exporter;

@ISA = qw(Exporter);
@EXPORT = qw(
      %a_env
      a_exec
      a_import_env
      a_md5
      a_cram
      a_hmac
      a_base64
      a_match_line
      a_match_resp
      a_ok
      a_fail
      a_generate_cfg
      a_inject_cfg
      a_provisioner
      a_start
      a_stop
      a_restart
      a_reload
      a_cp
      a_rm
      a_test
      a_subtest
      );

$VERSION = 3;


my $false = 0 ;
my $true  = 1 ;


sub logerr
{
        my $elog = '';
        open( $elog, ">>$a_env{errorlog}" );
        $info = join ( "", @_);
        $info =~ s/\r?\n/\\r\\n/g;
        print "$0: $info\r\n";
        print $elog "$0: $info\r\n";
        close( $elog );
        exit( a_fail() ) ;
}
$SIG{__WARN__} =  sub { &logerr(@_); };



=head2 $a_env{}

    A uniform interface to the environment variables.
    In case those are not set, uses default values given below.

=cut

%a_env = (
    "domain"  => $ENV{'axi_domain'}   || "localdomain",
    "host"    => $ENV{'axi_host'} || "localhost" ,
    "pass"    => $ENV{'axi_pass'} || "user1" ,
    "user"    => $ENV{'axi_user'} || "user1" ,
    "imap"    => $ENV{'axi_imap'} || 143 ,
    "imaps"   => $ENV{'axi_imap'} || 143 ,
    "smtp"    => $ENV{'axi_smtp'} || 25 ,
    "smtps"   => $ENV{'axi_smtp'} || 25 ,
    "pop"     => $ENV{'axi_pop'}  || 110 ,
    "pops"    => $ENV{'axi_pop'}  || 110 ,
    "http"    => $ENV{'axi_http'} || 8000,
    "https"   => $ENV{'axi_http'} || 8001,
    "cli"     => $ENV{'axi_cli'}  || 7000,
    "errorlog"=> $ENV{'axi_errorlog'} || 'error.log',
    "start"   => $ENV{'axi_start'}    || '',
    "stop"    => $ENV{'axi_stop'}     || '',
    "reload"  => $ENV{'axi_reload'}   || '',
    "syslog"  => $ENV{'axi_syslog'}   || '',
    "bin"     => $ENV{'axi_bin'}      || '',
    "binpath" => $ENV{'axi_binpath'}  || '/opt/axigen/bin/',
    "sendmail"=> $ENV{'axi_sendmail'} || '',
    "core"    => $ENV{'axi_core'}     || '',
    "debuglog"=> $ENV{'axi_debuglog'} || '',
    "path"    => $ENV{'axi_path'}     || '',
    "ddbpath" => $ENV{'axi_ddb_path'} || '',
    "debugfolder" => $ENV{'axi_debuglog_folder'}  || '',
    "provisioner" => $ENV{'axi_provisioner'}      || '',
    "config"      => $ENV{'axi_config'}  || 'axigen.cfg',
    "configpath"  => $ENV{'axi_config_path'}      || '/var/opt/axigen/run/',
    "adminpass"   => $ENV{'axi_admin_pass'}       || 'cucu',
    "template"    => $ENV{'axi_template'} || 'axigen.cfg.template',
    "local_test"  => 1 ,
    "linelen"     => 4096,
    "domlen"      => 300,
);

# workaround for perls one pass interpreter.
$a_env{"mbox"} = $ENV{'axi_mbox'} || "$a_env{user}\@$a_env{domain}" ;


sub a_test {
    print "$0\n" ;
}



=head2 a_ok()

    Function called when a test was ok.
    *Ex:
    if( mail_sent ) { exit a_ok() ; } ;

=cut
sub a_ok {
    print " --- Test Ok\n\n" ;
    return 69 ;
}



=head2 a_fail()

    Function called when a test fails.
    Ex: if(!connected) { exit a_fail() ; } ;
    Ex: a_fail( $rc eq $FAIL, "test lamely failed" ) ;
    In the second example, if the first parameter is evaluated to TRUE, then
    the test Fails, and `test lamely failed` is logged

=cut
sub a_fail {
    if( @_ eq 0 )
    {
        print " --- Test Failed\n\n" ;
        return 96 ;
    }
    if( @_ eq 2 )
    {
        if( @_[0] )
        {
            print "@_[1]\n" ;
            print " --- Test Failed\n\n" ;
            exit 96 ;

        }else
        {
            print "OK\n" ;
        }
    }
}


=head2 a_subtest()

    Print the currently running subtest.
    A subtest might be contained within a larger test-case
    *Ex:
    a_subtest() ;
    send_mails() ;

    a_subtest() ;
    retr_mails() ;

=cut

my $sbtst =1;

sub a_subtest {
    print "\n --- Subtest $sbtst\n";
    $sbtst+=1;
}





# emy could contribute to this, with the syslog shmen
sub a_start {
    print "Starting server" ;

    if( $a_env{local_test} == 1 ) {
        print " on local machine" ;
        system( "$a_env{start}" );
        if( !is_ok ) { return $false ; }
    }else{
        print " on remote machine" ;
        # figure this out :)
    }
    print " ok" ;
    return $true ;
}


# emy could contribute to this, with the syslog shmen
sub a_stop {
    print "Stopping server" ;

    if( $a_env{local_test} == 1 ) {
        print " on local machine" ;
        system( "$a_env{start}" );
        if( !$ok ) {
            warn " Server could not be started\n" ; return $false ;
        }
    }else{
        print " on remote machine" ;
        # figure this out :)
        # if( !$ok ) {
        #    warn " Server could not be started\n" ; return $false ; }
    }
    print " ok" ;
    return $true ;
}



sub a_cp {
    print "CP" ;
    if( $A_env{local_test} == 1 ) {
        print " local" ;
        # TODO add checking for ARGVs
        system( "cp $ARGV[0] $ARGV[1]" ) ;
        if( !ok ) { warn; return $false ; }
        print " ok\n";
    }else{
        print " remote" ;
        if( !ok ) { warn; return $false ; }
    }
    return $true ;
}

sub a_rm {
}


sub a_provisioner {
}

sub a_exec {
    system($_[0]);
    if  ( defined $DEBUG ) {
        print $color 'green';
        $exit_value  = $? >> 8;
        $signal_num  = $? & 127;
        $dumped_core = $? & 128;
        print $color 'reset';
    }

    if ( $signal_num != 0 )
    {
        $exit_value  = $? >> 8;
        $signal_num  = $? & 127;
        $dumped_core = $? & 128;

        print ($LOG_HANDLER "$ARGV[0]\n");
        print ($LOG_HANDLER "Exit  Value:$exit_value\n");
        print ($LOG_HANDLER "Signal  Num:$signal_num\n");
        print ($LOG_HANDLER "Dumped Core:$dumped_core\n\n");
    }

    if  ( defined $DEBUG )
    {
        print $color 'green';
        print ("Exit  Value:$exit_value\n");
        print ("Signal  Num:$signal_num\n");
        print ("Dumped Core:$dumped_core\n");
        print $color 'reset';
    }
}



sub a_cram {
    my ($text,$username, $password) = @_;

    @tmp=split(/ /, $text);
    $ticket64 =$tmp[1];
    my $ticket       = decode_base64 ($ticket64);
    my $password_md5 = hmac_md5_hex($ticket, $password) ;
    my $cram_md5     = encode_base64 ($username." ".$password_md5);
    return  $cram_md5;
}


sub a_md5 {
    print "Server key=".$asocket::server_key."--\n" ;
    my $apop_pass=$server_key."$_[0]";
    my $md5_sum=md5_hex($apop_pass);
    return $md5_sum;
}



sub a_base64 {
    my $text = $_[0];

    my $base64_sum = encode_base64("$text");
    chomp $base64_sum;
    chomp $base64_sum;
    return $base64_sum;
}


sub cntd_start {
    my ($dir, $no_dir) = @_;
    my $dir_cnt=-3;

    $CWD = cwd() ;
    chdir("$dir");
    opendir(DIR, ".");
    my @dirs = readdir(DIR);
    closedir(DIR);
    foreach (@dirs) {
        $dir_cnt++ unless -f;
    }
    chdir ($CWD);

    if ( $dir_cnt != $no_dir )
    {
        print "$dir_cnt != $no_dir Number of dir's DONT'T match\n";
    }else
    {
        print "$dir_cnt == $no_dir Number of dir's DOES match\n";
    }
}


sub a_hmac {
    my($data, $key, $hash_func, $block_size) = @_;
    $block_size ||= 64;
    $key = &$hash_func($key) if length($key) > $block_size;
    my $k_ipad = $key ^ (chr(0x36) x $block_size);
    my $k_opad = $key ^ (chr(0x5c) x $block_size);
    &$hash_func($k_opad, &$hash_func($k_ipad, $data));
}


sub hmac_md5
{
    a_hmac($_[0], $_[1], \&md5, 64);
}


sub hmac_md5_hex
{
    unpack("H*", &hmac_md5)
}



# Ghio legacy
# TODO: make it work :)
# param[in] FILE_NAME
# param[in] OUTPUT_FILE
# $FILE_NAME    = "axigen.default.cfg"; # pathul poate fi skimbat
# $OUTPUT_FILE	= "axigen.cfg";  # see above

sub a_generate_cfg
{
    local $service      = $ARGV[0] ;
    local $parameter    = $ARGV[1] ;
    local $value        = $ARGV[2] ;

    local $is_service   = 0;
    local $is_parameter = 0;
    local $is_done      = 0;
    local $other_service= 0;

    open( $ifile, "<$a_env{template}" ) ;
    open( $$ofile, ">$a_env{cfg}" ) ;

    if ( $service eq "" or $parameter eq "" or $value eq "")
    {
        warn ("usage: a_generate_cfg <service> <parameter> <value>\n");
        return $false ;
    }

    while (<$ifile>)
    {

        if ($is_service == 1)
        {
            foreach $serv (@SERVICES)
            {
                if ( ($_ =~ /$serv\s*=/) and ($serv ne $service) )
                {
                    $other_service = 1;
                }
            }
        }

        if ( $_ =~ /$service\s*=/ )
        {
            $is_service = 1;
            $other_service = 0;
        }
        if ( $_ =~ /$parameter\s*=/ and $is_service == 1 and $other_service == 0)
        {
            $is_parameter = 1;
        }

        if ($is_parameter == 1 and $is_done == 0)
        {
            print $ofile "\t$parameter = $value\n";
            $is_done = 1;
        }
        else
        {
            print $ofile $_;
        }
    }

    if ($is_service == 0)
    {
        warn  ("didn`t find the service $service\n");
        return $false ;
    }
    if ($is_parameter == 0)
    {
        warn ("didn`t find the paramter $parameter in $service\n");
        return $false ;
    }
    close($ifile);
    close($ofile);
    return $true ;
}
