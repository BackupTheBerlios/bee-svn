<?php
session_start();
require_once 'XML/RPC.php';
require_once 'base_lib.php';
function setMachineConfig($cli)
{

    /* Serialize Machine name */
    $versions = array() ;
    $m = $_GET['SUT_MACHINE'];

    $xuname   = new XML_RPC_Value( $_SESSION['username'],'string');
    $xmachine = new XML_RPC_Value( $m,'string');
    /*--------------------------------------------*/

    /* Serialize the content of config +++++++++++*/
    foreach ($_POST as $k => $v) {
        if( preg_match( "/SUT_/", $k) ) {
            $cfgLines[] = new XML_RPC_Value("$k = '".stripslashes($v)."'\n", 'string');
        }
    }
    $xcfgLines = new XML_RPC_Value(  $cfgLines, 'array');
    /*--------------------------------------------*/

    /* Serialize Method parameter ++++++++++++++++*/
    $request = new XML_RPC_Value(
            array(
                "sut_username" => $xuname,
                "sut_machine" => $xmachine,
                "cfg_lines" => $xcfgLines,
                ), "struct");
    $params=array($request);
    $msg = new XML_RPC_Message('setConfig', $params);
    /*--------------------------------------------*/

    $resp = $cli->send($msg);
    if( hasErrors($resp) ) return;
}


$cli = new XML_RPC_Client('/RPCSERVER', $_SESSION["host"], $_SESSION["port"]);
setMachineConfig($cli);
header('location:machines.php');
?>
