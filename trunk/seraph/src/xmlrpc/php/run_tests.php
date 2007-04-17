<?php
require_once 'XML/RPC.php';
require_once 'base_lib.php';

function runTests($cli)
{

    /* Serialize the SUT Versions ++++++++++++++++*/
    print "\nVersions:\n";
    $versions = array() ;
    for($i=0;$i<count($_GET['sut_versions']);++$i) {
        $versions[] = new XML_RPC_Value($_GET['sut_versions'][$i],'string');
    };
    $xversions = new XML_RPC_Value($versions,"array");
    print "\nBuild:\n   ".$_GET['sut_build']."\n";
    /*--------------------------------------------*/

    /* Serialize the OSes  +++++++++++++++++++++++*/
    print "\nOS:\n";
    $oses = array() ;
    for($i=0;$i<count($_GET['sut_os']);++$i) {
        $oses[] = new XML_RPC_Value($_GET['sut_os'][$i],'string');
    };
    $xoses = new XML_RPC_Value($oses,'array');
    /*--------------------------------------------*/

    /* Serialize the TEST categories +++++++++++++*/
    print "\nTests:\n";
    $tests = array();
    for($i=0;$i<count($_GET['sut_tests']);++$i) {
        $tests[] = new XML_RPC_Value($_GET['sut_tests'][$i],'string');
    };
    $xtests = new XML_RPC_Value($tests, "array");
    /*--------------------------------------------*/

    /* Serialize Method parameter ++++++++++++++++*/
    $request = new XML_RPC_Value(
            array(
                "sut_tests"     => $xtests,
                "sut_versions"  => $xversions,
                "sut_os"        => $xoses,
                "sut_build"     => new XML_RPC_Value($_GET['sut_build'], 'string')
                ), "struct");
    $params=array($request);
    $msg = new XML_RPC_Message('runTests', $params);
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
    #drawMenu() ;
    $cli = new XML_RPC_Client('/RPCSERVER','localhost',5000);
    $cli->setDebug(1);
    runTests($cli);
    #showInfo();
?>
    <br>
</body>
</html>
