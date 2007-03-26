require RPC::XML;
require RPC::XML::Client;

$cli = RPC::XML::Client->new('http://localhost:5000/RPCSERV') || die "errrrr";
$request = RPC::XML::request->new('listTests',100,
                                RPC::XML::string->new("/tmp/maillog"),
                                RPC::XML::string->new("axi -b axigen"));

$resp = $cli->send_request($request);
print ref $resp ? join(', ', @{$resp->value}) : "Error: $resp";
print "\n";

