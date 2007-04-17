<?php
session_start();
require_once 'XML/RPC.php';
include 'base_lib.php' ;

class Index {

    var $xmlrpc;

    function Index( $host, $port )
    {
        $this->xmlrpc = new XML_RPC_Client('/RPCSERVER', $host, $port );
        /* check for errors */
        //$this->xmlrpc->setDebug(1);
    }

    function listTests()
    {
        echo "Tests:<br>" ;

        $msg = new XML_RPC_Message('listTests');
        $resp = $this->xmlrpc->send($msg);
        if( hasErrors($resp) ) return false;

        $i = $resp->value()->arraysize();

        echo "<select name='sut_tests[]' multiple size='5'><option>ALL</option>";
        while($i--) {
            echo "<option>"
                .XML_RPC_decode($resp->value()->arraymem($i))
            ."</option>";
        }
        echo "</select>" ;
    }



    function listOSes()
    {
        echo "ON Machine:<br>" ;

        $msg = new XML_RPC_Message('listMachines');
        $resp = $this->xmlrpc->send($msg);
        if( hasErrors($resp)) return false;
        $i = $resp->value()->arraysize();

        echo "<select name='sut_os[]' multiple size='5'><option>ALL</option>" ;
        while($i--) {
            echo "<option>"
                .XML_RPC_decode($resp->value()->arraymem($i))
            ."</option>";
        }
        echo "</select>" ;
    }



    function listSUTVersions( )
    {
        echo "T:SUT Versions:<br>
            <select style='float:left' name='sut_versions[]' multiple size='5'>
            <option>2.0</option>
            <option>3.0</option>
            <option selected>4.0</option>
            </select>
            <input style='float:left'value='0.0' name='sut_build' type='text' size='3'/>
            ";
    }
    function listSchedules( )
    {
        echo "T:Schedules:
            <input type='checkbox' name='vehicle' value='Bike' /><br>
            <select name='sut_crontests[]' multiple size='5'>
            <option>joia</option>
            <option>vinerea</option>
            <option selected>regression30</option>
            </select>
            <!--<input valign='top' value='0.0' name='sut_build' type='text' size='3'/>-->
            ";
    }
}
?>


<!-- Begin Site -->
<html>
<head>
    <LINK href="mystyle.css" rel="stylesheet" type="text/css">
    <LINK REL="SHORTCUT ICON" href="seraph.ico">
    </head>

<body class='bheader' >

<?php
    $index = new Index('localhost', 5000);
    if( !drawMenu() ){
        echo "TEST:";
        echo "<a href='machines.php'>Machines</a>
            <a href='settings.php'>Settings</a>
            <a href='docs.php'>Documentation</a>
            ";
        echo "</body></html>";
        return;
    }
    ?>

    <form action='run_tests.php' method='get' name='sut_pretest'>
        <div class='column'>
            <span > <?php $index->listTests(); ?> &nbsp;</span>
            <span > <?php $index->listSUTVersions(); ?> &nbsp;</span>
            <span > <?php $index->listOSes(); ?> &nbsp;</span>
            <span > <?php $index->listSchedules(); ?> &nbsp;</span>
            <input type='submit' value='Run'/>
            <input type='submit' value='Setup'/>
            </div>
        </form>
        <br style='clear:both'>
        <hr>
        List of Jobs:(Opens log on click)<br>
<table cellspacing='1' width='60%'>
<tr bgcolor="#c8c8ff">
        <td class="center" valign="top" width ="0" nowrap>
        <span class="small">
        <a href="view.php?id=1036" title="[assigned] aSfasfaSF">00001</a><br />&nbsp;     </span>
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
        <a href="view.php?id=1036" title="[assigned] aSfasfaSF">00002</a><br />&nbsp;     </span>
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
        <a href="view.php?id=1036" title="[assigned] aSfasfaSF">00003</a><br />&nbsp;     </span>
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
</body>
</html>

<!--
<div style="color: rgb(50,0,210); background-color: #000; padding: 0px; font: 90% tahoma,verdana,sans-serif;">
<div style="margin: 0px 0px 0px 0px; height: 1px; overflow: hidden; background-color: #0c0"></div>
</div>
-->
