<?php
require_once 'XML/RPC.php';
include 'base_lib.php';
function setMachineConfig($cli)
{

    /* Serialize the SUT Versions */
    print "\nSet config of machine:\n";
    $versions = array() ;
    $m = $_GET['SUT_MACHINE'];
    print "   $m--\n";

    $xmachine = new XML_RPC_Value( $m,'string');
    /*--------------------------------------------*/

    /* Serialize the content of config */
    foreach ($_POST as $k => $v) {
        if( preg_match( "/SUT_/", $k) ) {
            echo "$k = $v\n";
            $cfgLines[] = new XML_RPC_Value("$k = '$v'\n", 'string');
        }
    }
    $xcfgLines = new XML_RPC_Value(  $cfgLines, 'array');

    /* Serialize Method parameter */
    $request = new XML_RPC_Value(
            array(
                "sut_machine" => $xmachine,
                "cfg_lines" => $xcfgLines,
                ), "struct");
    $params=array($request);
    $msg = new XML_RPC_Message('setConfig', $params);
    /*--------------------------------------------*/

    $resp = $cli->send($msg);

    if (!$resp) {
        echo 'Communication error: ' . $cli->errstr;
        exit;
    }
    if (!$resp->faultCode()) {
        $val = $resp->value();
        $data = XML_RPC_decode($val);
        echo $data;
    } else {
        /*
        * Display problems that have been gracefully cought and
        * reported by the xmlrpc.php script
         */
        echo 'Fault Code: ' . $resp->faultCode() . "\n";
        echo 'Fault Reason: ' . $resp->faultString() . "\n";
    }
}


function showInfo()
{
    print "<pre>\n";
    print "\nContents of \$_GET:\n";
    foreach ($_GET as $k => $v) {
        if( preg_match( "/SUT_/", $k) )
            print "   $k = $v\n";
    }
#
    print "\nContents of \$_POST:\n";
    foreach ($_POST as $k => $v) {
        if( preg_match( "/SUT_/", $k) )
            print "   $k = $v\n";
    }
#
    print "\nContents of \$_COOKIE:\n";
    foreach ($_COOKIE as $k => $v) {
        print "   $k = $v\n";
    }
#
    print "\nContents of \$_REQUEST:\n";
    foreach ($_REQUEST as $k => $v) {
        print "   $k = $v\n";
    }
#
    print "\nContents of \$_SERVER:\n";
    foreach ($_SERVER as $k => $v) {
        print "   $k = $v\n";
    }
    print "</pre>\n";
}
?>

<html>
<head>
<link href='mystyle.css' rel='stylesheet' type='text/css'>
</head>
<body class='bheader'>
<?php
drawMenu() ;
$cli = new XML_RPC_Client('/RPCSERVER','localhost',5000);
setMachineConfig($cli);
#showInfo();
?>
<br>
</body>
</html>
