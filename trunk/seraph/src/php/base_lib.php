<?php
require_once 'XML/RPC.php';


function session_defaults() {
    $_SESSION['logged'] = false;
    $_SESSION['uid'] = 0;
    $_SESSION['username'] = 'none';
    $_SESSION['cookie'] = 0;
    $_SESSION['remember'] = false;
    $_SESSION['host'] = "localhost";
    $_SESSION['port'] = 5000;
}

function listJobs($job_type)
{
    $xmlrpc = new XML_RPC_Client('/RPCSERVER', $_SESSION["host"], $_SESSION["port"] );
    echo "<button onClick=\"window.location='clear_logs.php'\" style='margin-left:15px'>Clear Logs</button><br/>";
    echo "List of Jobs:(click to open errorlog)<br>";
    echo "<div class='job_running'>";
    echo "<span class='id'>Id</span>";
    echo "<span >Job State</span>";
    echo "<span class='name'>Machine</span>";
    echo "<span >Start date</span>";
    echo "<span >Start time</span>";
    echo "Progress";
    echo "</div>";
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

    while($i--) {
        $log = $resp->value()->arraymem($i);
        $date = XML_RPC_decode($log->structmem("job_date") );
        $time = XML_RPC_decode($log->structmem("job_time") );
        $name = XML_RPC_decode($log->structmem("job_name") );
        $ctest= XML_RPC_decode($log->structmem("job_ctest"));
        $tests= XML_RPC_decode($log->structmem("job_tests"));
        if($tests==0) $tests=1;
        $pct  = 4+100*(int)$ctest/((int)$tests*6);
        echo "<a style='text-decoration:none' href='view.php?log=$name'>";
        echo "<div class='$state'>";
        echo "<span class='id'>1000</span>";
        echo "<span>$state</span>";
        echo "<span class='name'>$name</span>";
        echo "<span>$date</span>";
        echo "<span>$time</span>";
        echo "<span class='bar' style='margin:0.5em; height: 15px; width: $pct%;'> </span>";
        echo "$ctest of $tests";
        echo "</div>";
        echo "</a>";
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
    {   echo "Can't talk to <b>seraph</b> daemon: host(".$_SESSION['host'].") : port(".$_SESSION['port'].")" ;
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
        <img src='title.png'/>
        <br/><br/>
        </div>
        <div class='head_menu'>
        <ul>";

    if ( $_SESSION['logged'] == false ) {
            echo "<li><a href='.'>Home</a></li>";
            echo "<li><a href='form_login.php'>Login</a></li>";
            echo "<li><a href='form_register.php'>Register</a></li>";
            echo "</ul></div><br>";
            return false;
    }
    echo "<li><a href='.'>Home</a></li>
            <li><a href='machines.php'>Machines</a></li>
            <li><a href='notifications.php'>Notifications</a></li>
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
