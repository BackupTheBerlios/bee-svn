<?php
session_start();
require_once 'XML/RPC.php';
require_once 'base_lib.php';

    function listJobs()
    {
    $job_pending =1;
    $job_running=2;
    $job_complete=4;
    $job_all=8;
        $xmlrpc = new XML_RPC_Client('/RPCSERVER', "localhost", 5000 );
        echo " List of Jobs:(Opens log on click)<br>";
        /*-------------------------------------------*/
        $state = "job_running";

        $req = new XML_RPC_Value( array(
                            "sut_username" => new XML_RPC_Value( "user1",'string'),
                            "job_type"   => new XML_RPC_Value( $job_running,'int'))
                    , "struct");

        $msg = new XML_RPC_Message('listJobs', array($req));
        $resp = $xmlrpc->send($msg);
        if( hasErrors($resp) ) return false;

        $i = $resp->value()->arraysize();
        echo "NbJobs:$i<br>";

        while($i--) {
            $log = XML_RPC_decode($resp->value()->arraymem($i));
            echo "<div class='$state'>";
            echo "<a href='view.php?log=$log'>";
            echo "<span>1000</span>";
            echo "<span><b >$state</b></span>";
            echo "<span>$log</span>";
            echo "</a>";
            echo "</div>";
        }
        /*---------------------------------------------*/
        /*
        while($i++<3) {
        echo "<div class='job_running_ok'>";
        echo "<a href='http://google.com'>";
            echo "<span>1000</span>";
            echo "<span><b >Running</b></span>";
            echo "<span>".XML_RPC_decode($resp->value()->arraymem($i)) ."</span>";
        echo "</a>";
        echo "</div>";
        }
        while($i++<6) {
        echo "<div class='job_pending'>";
        echo "<a href='http://google.com'>";
            echo "<span>1000</span>";
            echo "<span><b >Pending</b></span>";
            echo "<span>".XML_RPC_decode($resp->value()->arraymem($i)) ."</span>";
        echo "</a>";
        echo "</div>";
        }*/
    }
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
    $cli = new XML_RPC_Client('/RPCSERVER','localhost',5000);
    //$cli->setDebug(1);
    drawMenu() ;
    runTests($cli);
    #showInfo();
    echo "STARTED";
    listJobs();?>
    <br>
</body>
</html>
