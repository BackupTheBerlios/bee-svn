<?php
session_start();
require_once 'XML/RPC.php';
require_once 'base_lib.php';

function runTests($cli)
{

    /* Serialize the SUT Versions ++++++++++++++++*/
    //print "\nVersions:\n";
    $versions = array() ;
    for($i=0;$i<count($_GET['sut_versions']);++$i) {
        $versions[] = new XML_RPC_Value($_GET['sut_versions'][$i],'string');
    };
    $xversions = new XML_RPC_Value($versions,"array");
    //print "\nBuild:\n   ".$_GET['sut_build']."\n";
    /*--------------------------------------------*/

    /* Serialize the OSes  +++++++++++++++++++++++*/
    //print "\nOS:\n";
    $oses = array() ;
    for($i=0;$i<count($_GET['sut_os']);++$i) {
        $oses[] = new XML_RPC_Value($_GET['sut_os'][$i],'string');
    };
    $xoses = new XML_RPC_Value($oses,'array');
    /*--------------------------------------------*/

    /* Serialize the TEST categories +++++++++++++*/
    //print "\nTests:\n";
    $tests = array();
    for($i=0;$i<count($_GET['sut_tests']);++$i) {
        $tests[] = new XML_RPC_Value($_GET['sut_tests'][$i],'string');
    };
    $xtests = new XML_RPC_Value($tests, "array");
    /*--------------------------------------------*/

    $refresh = "n";
    if( $_REQUEST['refresh'] )
        $refresh="y";

    /* Serialize Method parameter ++++++++++++++++*/
    $request = new XML_RPC_Value(
            array(
                "sut_tests"     => $xtests,
                "sut_versions"  => $xversions,
                "sut_os"        => $xoses,
                "sut_build"     => new XML_RPC_Value($_GET['sut_build'], 'string'),
                "sut_refresh"   => new XML_RPC_Value($refresh, 'string')
                ), "struct");
    $params=array($request);
    $msg = new XML_RPC_Message('runTests', $params);
    /*--------------------------------------------*/

    $resp = $cli->send($msg);
    if( hasErrors($resp) ) return;
}
    $cli = new XML_RPC_Client('/RPCSERVER','localhost',5000);
    showInfo();
    runTests($cli);
    sleep(1); /* avoid hitting refresh to see the newly launched job ( maybe there is a sleep in
                the backend*/
    header("location:index.php");
?>
