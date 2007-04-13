<?php
require_once 'XML/RPC.php';
include 'base_lib.php';
function setMachineConfig($cli)
{

    /* Serialize the SUT Versions */
    print "\nSet config of machine:\n";
    $versions = array() ;
    $m = $_GET['SUT_MACHINE'];
    print "   $m\n";

    $xmachine = new XML_RPC_Value( $m,'string');
    /*--------------------------------------------*/

    /* Serialize the content of config */
    foreach ($_POST as $k => $v) {
        if( preg_match( "/SUT_/", $k) )
            print "   $k = $v\n";
    }
    /* Serialize the OSes  */
    /*print "\nOS:\n";
    $oses = array() ;
    for($i=0;$i<count($_GET['sut_os']);++$i) {
        print "   ".$_GET['sut_os'][$i]."\n";
        $oses[] = new XML_RPC_Value($_GET['sut_os'][$i],'string');
    };
    $xoses = new XML_RPC_Value($oses,'array');
    */
    /*--------------------------------------------*/

    /* Serialize the TEST categories */
    /*print "\nTests:\n";
    $tests = array();
    for($i=0;$i<count($_GET['sut_tests']);++$i) {
        print "   ".$_GET['sut_tests'][$i]."\n";
        $tests[] = new XML_RPC_Value($_GET['sut_tests'][$i],'string');
    };
    $xtests = new XML_RPC_Value($tests, "array");
    */
    /*--------------------------------------------*/

    /* Serialize Method parameter */
    $request = new XML_RPC_Value(
            array(
                "sut_tests" => $xtests,
                "sut_versions" => $xversions,
                "sut_os" => $xoses,
                "sut_build" => new XML_RPC_Value($_GET['sut_build'], 'string')
                ), "struct");
    $params=array($request);
    $msg = new XML_RPC_Message('runTests', $params);
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
#$cli = new XML_RPC_Client('/RPCSERVER','localhost',5000);
#setMachineConfig($cli);
showInfo();
?>
<br>
</body>
</html>
