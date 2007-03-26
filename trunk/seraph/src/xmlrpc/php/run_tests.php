<?php
require_once 'XML/RPC.php';
include 'mystyle.php';
function showInfo($cli)
{
    print "<pre>\n";
    /*
    print "\nContents of \$_GET:\n";
    foreach ($_GET as $k => $v) {
        print "   $k = $v\n";
    }
    */

    /* Serialize the SUT Versions */
    print "\nVersions:\n";
    $versions = array() ;
    for($i=0;$i<count($_GET['sut_versions']);++$i) {
        print "   ".$_GET['sut_versions'][$i]."\n";
        $versions[] = new XML_RPC_Value($_GET['sut_versions'][$i],'string');
    };
    $xversions = new XML_RPC_Value($versions,"array");
    print "\nBuild:\n   ".$_GET['sut_build']."\n";
    /*--------------------------------------------*/

    /* Serialize the OSes  */
    print "\nOS:\n";
    $oses = array() ;
    for($i=0;$i<count($_GET['sut_os']);++$i) {
        print "   ".$_GET['sut_os'][$i]."\n";
        $oses[] = new XML_RPC_Value($_GET['sut_os'][$i],'string');
    };
    $xoses = new XML_RPC_Value($oses,'array');
    /*--------------------------------------------*/

    /* Serialize the TEST categories */
    print "\nTests:\n";
    $tests = array();
    for($i=0;$i<count($_GET['sut_tests']);++$i) {
        print "   ".$_GET['sut_tests'][$i]."\n";
        $tests[] = new XML_RPC_Value($_GET['sut_tests'][$i],'string');
    };
    $xtests = new XML_RPC_Value($tests, "array");
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
/*
#
    print "\nContents of \$_POST:\n";
    foreach ($_POST as $k => $v) {
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
    */
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
showInfo($cli);
?>
<br>
</body>
</html>
