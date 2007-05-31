<?php
session_start();
require_once 'XML/RPC.php';
require_once 'base_lib.php';

function addMachine($cli)
{
    /* Serialize _POST */

    $xuser  = new XML_RPC_Value( $_SESSION['username'],'string');
    $xmname = new XML_RPC_Value( $_POST['SUT_MNAME'],'string');
    $xos    = new XML_RPC_Value( $_POST['SUT_OS'],   'string');
    $xosver = new XML_RPC_Value( $_POST['SUT_OSVER'],'string');
    $xmip   = new XML_RPC_Value( $_POST['SUT_MIP'],  'string');

    /* Serialize Method parameter */
    $request = new XML_RPC_Value(
            array(
                "sut_username"  => $xuser,
                "sut_mname" => $xmname,
                "sut_os"    => $xos,
                "sut_osver" => $xosver,
                "sut_mip"   => $xmip,
                ), "struct");
    $params=array($request);
    $msg = new XML_RPC_Message('addMachine', $params);

    $resp = $cli->send($msg);
    if( hasErrors($resp) ) return;
}
/*---------------------------------------------------------------------------*/
$cli = new XML_RPC_Client('/RPCSERVER', $_SESSION["host"], $_SESSION["port"]);
addMachine($cli);
header('location:machines.php');
?>
