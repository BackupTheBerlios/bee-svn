<?php
require_once 'XML/RPC.php';
include 'mystyle.php' ;



function listTests($cli)
{
    echo "Tests:<br>" ;

    $msg = new XML_RPC_Message('listTests');
    $resp = $cli->send($msg);

    if (!$resp)
    {   echo 'Can\' talk to <b>seraph</b> daemon: ' . $cli->errstr;
        exit;
    }

    if (!$resp->faultCode())
    {   $val = $resp->value();
        $i = $val->arraysize();

        echo "<select name='sut_tests[]' multiple size='5'>" ;

        while($i--) {
                echo "<option>".XML_RPC_decode($val->arraymem($i))."</option>";
        }

        echo "</select>" ;
        $data = XML_RPC_decode($val);
    } else
    {   echo 'Fault Code  : ' . $resp->faultCode() . "\n";
        echo 'Fault Reason: ' . $resp->faultString() . "\n";
    }
}



function listOSes($cli)
{
    echo "ON Machine:<br>" ;

    $msg = new XML_RPC_Message('listMachines');
    $resp = $cli->send($msg);

    if (!$resp)
    {   echo 'Can\' talk to <b>seraph</b> daemon: ' . $cli->errstr;
        exit;
    }

    if (!$resp->faultCode())
    {   $val = $resp->value();
        $i = $val->arraysize();

        echo "<select name='sut_os[]' multiple size='5'>" ;

        while($i--) {
                echo "<option>".XML_RPC_decode($val->arraymem($i))."</option>";
        }
        echo "</select>" ;
        $data = XML_RPC_decode($val);
    } else
    {   echo 'Fault Code  : ' . $resp->faultCode() . "\n";
        echo 'Fault Reason: ' . $resp->faultString() . "\n";
    }
}



function listSUTVersions($cli)
{
    echo "SUT Versions:<br>
    <select name='sut_versions[]' multiple size='5'>
    <option>2.0</option>
    <option>3.0</option>
    <option selected>4.0</option>
    </select>
    <input valign='top' value='0.0' name='sut_build' type='text' size='3'/>
    ";
}



/*
function listOSes($cli)
{
    echo "On machine:<br>
    <select name='sut_os[]' multiple size='5'>
    <option value='purec' >purec</option>
    <option value='openbsd39'>openbsd39</option>
    <option value='netbsd30'>netbsd30</option>
    <option value='solaris10'>solaris10</option>
    </select> ";
}
*/
function call($arg1, $arg2, $cli)
{
    echo $arg2;
    $params = array(new XML_RPC_Value( $arg2, 'string'));
    $msg = new XML_RPC_Message($arg1, $params);
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

/*-------------------------------*/
$cli = new XML_RPC_Client('/RPCSERVER', 'localhost', 5000);
?>


<!-- Begin Site -->
<html>
<head>
    <LINK href="mystyle.css" rel="stylesheet" type="text/css">
    </head>

<body class='bheader' >

    <?php drawMenu() ; ?>
    <br>

    <form action='run_tests.php' method='get' name='sut_pretest'>
        <div class='column'>
            <span > <?php listTests($cli); ?> &nbsp;</span>
            <span > <?php listSUTVersions($cli); ?> &nbsp;</span>
            <span > <?php listOSes($cli); ?> &nbsp;</span>
            <input type='submit' value='Run'/>
            <input type='submit' value='Setup'/>
            </div>
        </form>
</body>
</html>

<!--
<div style="color: rgb(50,0,210); background-color: #000; padding: 0px; font: 90% tahoma,verdana,sans-serif;">
<div style="margin: 0px 0px 0px 0px; height: 1px; overflow: hidden; background-color: #0c0"></div>
</div>
-->
