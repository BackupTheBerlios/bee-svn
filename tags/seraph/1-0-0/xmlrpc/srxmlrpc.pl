use RPC::XML::Server;
use RPC::XML::Method;

sub testIt {
    print "hello\n";
    return "XMLRPC:call(testIt)<br>";
}

sub checkCore {
        print "checkCore\n";
        return "XMLRPC:call(checkCore): Checking for core<br>";
}

sub listTests {
        print "Listing tests\n";
        opendir(DIR, "/home/groleo/code/testbot/2.0/tests/") || die "can't opendir $some_dir: $!";
        my $rsp="<pre>";
        while (  $entry = readdir(DIR) )
        {
            print "$entry\n";
            $rsp = $rsp.$entry."<br>";
        }
        closedir DIR;
        $rsp=$rsp."</pre>";
        return $rsp;
}
$srv = RPC::XML::Server->new(port => 20000);
# Several of these, most likely:
$srv->add_method({ name => 'testIt',
      signature => [ 'string string' ], code => \&testIt });
$srv->add_method({ name => 'checkCore',
      signature => [ 'string string' ], code => \&checkCore });
$srv->add_method({ name => 'listTests',
      signature => [ 'string string' ], code => \&listTests });
$srv->server_loop; # Never returns

