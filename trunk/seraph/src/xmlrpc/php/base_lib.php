<?php
require_once 'XML/RPC.php';

function session_defaults() {
    $_SESSION['logged'] = false;
    $_SESSION['uid'] = 0;
    $_SESSION['username'] = 'none';
    $_SESSION['cookie'] = 0;
    $_SESSION['remember'] = false;
}



function hasErrors($resp)
{
    if (!$resp)
    {   echo 'Can\' talk to <b>seraph</b> daemon: ' . $cli->errstr;
        exit;
    }
    if( $resp->faultCode() )
    {   echo 'Fault Code: ' . $resp->faultCode() . "\n";
        echo 'Fault Reason: ' . $resp->faultString() . "\n";
        return 1;
    }
    return 0;
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
function drawMenu() {
    echo "<div  class='header'>
        <font class='header'>Seraph</font>
        <br/><br/>
        </div>
        <div class='head_menu'>
        <ul>";

    if ( $_SESSION['logged'] == false ) {
            echo "<li><a href='.'>Home</a></li>";
            echo "<li><a href='login_form.php'>Login</a></li>";
            echo "<li><a href='register_form.php'>Register</a></li>";
            echo "</ul></div><br>";
            return false;
    }
    echo "<li><a href='.'>Home</a></li>
            <li><a href='machines.php'>Machines</a></li>
            <li><a href='jobs.php'>Jobs</a></li>
            <li><a href='docs.php'>Documentation</a></li>
            <li><a href='settings.php'>Settings</a></li>";
    echo "<li><a href='./logout.php'>Logout</a></li>";
    echo "</ul></div><br>";
    return true;
}

function drawMachines()
{
    $cli = new XML_RPC_Client('/RPCSERVER', 'localhost', 5000);

    #$cli->setDebug(1);
    $cli->setCredentials("user1","user1");


    $msg = new XML_RPC_Message('listMachines');
    $resp = $cli->send($msg);
    if( hasErrors($resp) ) return;

    $machines = $resp->value();
    $i = $machines->arraysize();
    #$i = 1;
    while( $i-- )
    {   $m = XML_RPC_decode( $machines->arraymem($i) );
        echo "<form name='$m' action='set_config.php?SUT_MACHINE=$m' method='post'>
        <ul class='menu' >
        <li >
        <p  onclick='javascript:toggleSpan(\"span$m\");' >$m<span id='span$m'>
        <a href='javascript:;' onclick='addEvent(\"span$m\", \"sName$m\", \"sValue$m\");'><em class='butt'>Add</em></a>
        <input type='submit' value='Save'>

        <input id='sName$m'  type='text' value='SUT_NAME' style='text-align:right; clear:both; float:left; width:12.1em;'/>
        <input id='sValue$m' type='text' value='Value'/>";
        $params = array(new XML_RPC_Value( ($m), 'string') );
        $msg    = new XML_RPC_Message('getConfig', $params);
        $resp   = $cli->send($msg);
        if( hasErrors($resp) ) return ;
        $cfgTable = $resp->value();
        $j = $cfgTable->arraysize();
        while($j--){
            $cfgEntry=$cfgTable->arraymem($j);
            $cfgEntry->structreset();
            $symbol = XML_RPC_decode($cfgEntry->structmem("symbol")) ;
            $value  = XML_RPC_decode($cfgEntry->structmem("val")) ;
            echo "<b>$symbol</b>";
            echo "<input type='text' name='$symbol' value='$value' /><br>\n";
        }
        echo " </span></p>
            </li> </ul> </form>";
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
