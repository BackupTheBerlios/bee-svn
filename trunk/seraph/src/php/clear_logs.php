<?php
require_once 'XML/RPC.php';
require_once 'base_lib.php';
session_start();

$c = new XML_RPC_Client( '/RPCSERVER', $_SESSION["host"], $_SESSION["port"]);
$req = new XML_RPC_Value(
                array( "sut_username" => new XML_RPC_Value( $_SESSION['username'], 'string')
            ),"struct" );

$msg = new XML_RPC_Message('clearRunnedJobs', array($req));
$rsp = $c->send($msg);
if( hasErrors($rsp) ) return false;
header('location:index.php');

?>
