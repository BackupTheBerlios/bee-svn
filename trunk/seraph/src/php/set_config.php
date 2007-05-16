<?php
session_start();
require_once 'XML/RPC.php';
require_once 'base_lib.php';
function setMachineConfig($cli)
{

    /* Serialize Machine name */
    print "\nSet config of machine:\n";
    $versions = array() ;
    $m = $_GET['SUT_MACHINE'];
    print "   $m--\n";

    $xmachine = new XML_RPC_Value( $m,'string');
    /*--------------------------------------------*/

    /* Serialize the content of config +++++++++++*/
    foreach ($_POST as $k => $v) {
        if( preg_match( "/SUT_/", $k) ) {
            echo "GOT: $k = $v\n";
            $cfgLines[] = new XML_RPC_Value("$k = '".stripslashes($v)."'\n", 'string');
        }
    }
    $xcfgLines = new XML_RPC_Value(  $cfgLines, 'array');
    /*--------------------------------------------*/

    /* Serialize Method parameter ++++++++++++++++*/
    $request = new XML_RPC_Value(
            array(
                "sut_machine" => $xmachine,
                "cfg_lines" => $xcfgLines,
                ), "struct");
    $params=array($request);
    $msg = new XML_RPC_Message('setConfig', $params);
    /*--------------------------------------------*/

    $resp = $cli->send($msg);
    if( hasErrors($resp) ) return;
}


?>

<html>
<head>
<link href='mystyle.css' rel='stylesheet' type='text/css'>
</head>
<body class='bheader'>
<?php
drawMenu() ;
$cli = new XML_RPC_Client('/RPCSERVER', $_SESSION["host"], $_SESSION["port"]);
$cli->setDebug(1);
setMachineConfig($cli);
#include "machines.php";
showInfo();
?>
<br>
</body>
</html>
