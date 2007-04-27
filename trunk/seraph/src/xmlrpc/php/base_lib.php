<?php
require_once 'XML/RPC.php';

function session_defaults() {
    $_SESSION['logged'] = false;
    $_SESSION['uid'] = 0;
    $_SESSION['username'] = 'none';
    $_SESSION['cookie'] = 0;
    $_SESSION['remember'] = false;
}

function listJobs($job_type)
{
    $xmlrpc = new XML_RPC_Client('/RPCSERVER', "localhost", 5000 );
    echo " List of Jobs:(click to open errorlog)<br>";
    /*-------------------------------------------*/
    $state = "job_running";


    $req = new XML_RPC_Value( array(
                        "sut_username" => new XML_RPC_Value( $_SESSION['username'],'string'),
                        "job_type"   => new XML_RPC_Value( $job_type,'int'))
                , "struct");

    $msg = new XML_RPC_Message('listJobs', array($req));
    $resp = $xmlrpc->send($msg);
    if( hasErrors($resp) ) return false;

    $i = $resp->value()->arraysize();

    /*echo "NbJobs:$i<br>";*/
    /*<div class="progress400">
    </div>*/

    while($i--) {
        $log = XML_RPC_decode($resp->value()->arraymem($i));
        echo "<div class='$state'>";
        echo "<a href='view.php?log=$log'>";
        echo "<span class='id'>1000</span>";
        echo "<span class='state'>$state</span>";
        echo "<span>$log</span>";
        echo '<span class="bar" style="width: 10%;">TODO</span>';
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
            <!--<li><a href='jobs.php'>Jobs</a></li>-->
            <li><a href='docs.php'>Documentation</a></li>
            <!--<li><a href='settings.php'>Settings</a></li>-->";
    echo "<li><a href='./logout.php'>Logout</a></li>";
    echo "</ul></div><br>";
    return true;
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
