<?php
require_once 'XML/RPC.php';
require_once 'base_lib.php' ;
echo "test test test";

$cli = new XML_RPC_Client('/RPCSERVER', "localhost", 5000 );
$req = new XML_RPC_Value( $_REQUEST['log'], 'string');
$msg = new XML_RPC_Message( 'getErrorLog', array($req));
$rsp = $cli->send($msg);
if( hasErrors($rsp) )
    return false;
echo "TTT<pre>".XML_RPC_decode($rsp->value())."</pre>";
/*echo base64_decode("[Li8xMTkuYXV0aF90cmFuc2FjdGlvbi5wbDogLUVSUiBQcm90b2NvbCB2aW9sYXRpb25cclxu
XHJcbg0KLi8xMTE5LkJVRy5wbDogK09LIHVzZXIxQGxvY2FsZG9tYWluIG5lZWRzIGEgcGF
zc3dvcmRcclxuXHJcbg0KLi8xMTguYXV0aF9jYW5jZWxlZC5wbDogLUVSUiBXcm9uZyBTQV
NMIHJlc3BvbnNlXHJcblxyXG4NCg==
");*/
//echo base64_decode(XML_RPC_Decode($rsp->value())) ;
?>
