<?php
require_once 'XML/RPC.php';
require_once 'base_lib.php' ;

$cli = new XML_RPC_Client('/RPCSERVER', "localhost", 5000 );
$req = new XML_RPC_Value( $_REQUEST['log'], 'string');
$msg = new XML_RPC_Message( 'getErrorLog', array($req));
$rsp = $cli->send($msg);
if( hasErrors($rsp) )
    return false;
echo $_REQUEST['log']."<hr><pre>".XML_RPC_decode($rsp->value())."</pre>";
?>
