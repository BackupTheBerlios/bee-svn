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
    echo "STARTED";?>
<table cellspacing='1' width='60%'>
<tr bgcolor="#c8c8ff">
        <td class="center" valign="top" width ="0" nowrap>
        <span class="small">
        <a href="view_job.php?SUT_jobid=1036" title="[assigned] aSfasfaSF">00001</a><br />&nbsp;     </span>
    </td>

        <td class="left" valign="top" width="100%">

        <span class="small">
        [cli, hsp, html]<br/>
        [2.0] [machine1] [2007-04-12 05:02] [ 0 of 120]
        </span>
    </td>
    <td>
    <span>
    Pending
    </span>
    </td>
</tr>
<tr bgcolor="#ffa0a0">
        <td class="center" valign="top" width ="0" nowrap>
        <span class="small">
        <a href="view_job.php?SUT_jobid=1036" title="[assigned] aSfasfaSF">00002</a><br />&nbsp;     </span>
    </td>

        <td class="left" valign="top" width="100%">

        <span class="small">
        [cli, hsp, html]<br/>
        [2.0] [machine1] [2007-04-12 05:02] [120 of 120]
    <td>
    <span> Complete
    </span>
    </td>
        </span>
    </td>
</tr>
<tr bgcolor="#cceedd">
        <td class="center" valign="top" width ="0" nowrap>
        <span class="small">
        <a href="view_job.php?SUT_jobid=1036" title="[assigned] aSfasfaSF">00003</a><br />&nbsp;     </span>
    </td>

        <td class="left" valign="top" width="100%">

        <span class="small">
        [cli, hsp, html]<br/>
        [2.0] [machine1] [2007-04-12 05:02] [ 12 of 120]
        </span>
    <td>
    <span> Running
    </span>
    </td>
</tr>
</table>
    <br>
</body>
</html>
