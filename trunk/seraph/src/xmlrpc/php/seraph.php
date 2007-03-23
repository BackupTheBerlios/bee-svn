<?php
require_once 'XML/RPC.php';

function listTests($cli)
{
    echo "Tests:<br>" ;
    echo $arg2;
/* */
    $msg = new XML_RPC_Message('listTests');
    $resp = $cli->send($msg);
    if (!$resp) {
        echo 'Can\' talk to <b>seraph</b> daemon: ' . $cli->errstr;
        exit;
    }
    if (!$resp->faultCode()) {
        $i=0;
        $val = $resp->value();
        $i = $val->arraysize();
        echo "<select name='sut_tests[]' multiple size='5'>" ;
        while($i--) {
                echo "<option>".XML_RPC_decode($val->arraymem($i))."</option>";
        }
        echo "</select>" ;
        $data = XML_RPC_decode($val);
    } else {
        echo 'Fault Code: ' . $resp->faultCode() . "\n";
        echo 'Fault Reason: ' . $resp->faultString() . "\n";
    }
/* */
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

function listOSes($cli)
{
    echo "Remote OS:<br>
    <select name='sut_os[]' multiple size='5'>
    <option value='linux/gcc3' >linux/gcc3</option>
    <option value='linux/gcc4'>linux/gcc4</option>
    <option value='NetBSD/3.0'>NetBSD/3.0</option>
    <option value='Solaris/10'>Solaris/10</option>
    <option value='OpenBSD/3.9'>OpenBSD/3.9</option>
    <option>FreeBSD/5.4</option>
    <option>FreeBSD/6.1</option>
    </select> ";
}

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

$cli = new XML_RPC_Client('/RPCSERVER', 'localhost', 5000);
?>


<!-- Begin Site -->
<html>
<head>
    <LINK href="mystyle.css" rel="stylesheet" type="text/css">
    </head>

<body class='bheader' >
    <div  class='header'>
        <font class='header'>Seraph</font>
        <br/><br/>
        </div>
    <div class='head_menu'>
        <ul>
            <li><a href='login.php'>Login</a></li>
            <li><a href='register.php'>Register</a></li>
            <li><a href='settings.php'>Settings</a></li>
            <li><a href='machines.php'>Machines</a></li>
            <li><a href="README.txt">Documentation</a></li>
            </ul>
        </div>

    <br>

    <form action='run_tests.php' method='get' name='sut_pretest'>
        <div class='column'>
            <span > <?php listTests($cli); ?> &nbsp;</span>
            <span > <?php listSUTVersions($cli); ?> &nbsp;</span>
            <span > <?php listOSes($cli); ?> &nbsp;</span>
            <input type='submit' value='Run'/>
            </div>
        </form>
</body>
</html>

<!--
<div style="color: rgb(50,0,210); background-color: #000; padding: 0px; font: 90% tahoma,verdana,sans-serif;">
<div style="margin: 0px 0px 0px 0px; height: 1px; overflow: hidden; background-color: #0c0"></div>
</div>
-->
