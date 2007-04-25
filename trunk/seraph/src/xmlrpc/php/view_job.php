<?php
require_once 'XML/RPC.php';
echo "test test test"

$req = new XML_RPC_Value( $_REQUEST['log'], 'string');
$msg = new XML_RPC_Message( 'getErrorLog', array($req));
$rsp = $cli->send($msg);
if( hasErrors($rsp) )
    return false;
// echo base64decode($rsp->value() ;
?>
