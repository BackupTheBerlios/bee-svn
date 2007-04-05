<?php
require_once 'XML/RPC.php';

function drawMenu() {
    echo "<div  class='header'>
        <font class='header'>Seraph</font>
        <br/><br/>
        </div>
    <div class='head_menu'>
        <ul>
            <li><a href='/'>Home</a></li>
            <!--<li><a href='register.php'>Register</a></li>-->
            <li><a href='machines.php'>Machines</a></li>
            <li><a href='settings.php'>Settings</a></li>
            <li><a href='README.txt'>Documentation</a></li>
            <li><a href='login.php'>Login</a></li>
            </ul>
        </div>";
}

function drawMachines()
{
    $cli = new XML_RPC_Client('/RPCSERVER', 'localhost', 5000);

    echo " <ul id='menu'>
        <li ><p>purec
        <span>
        <a href='http://google.com'><em class='butt'>Add</em></a>
        <a href='http://google.com'><em class='butt'>Save</em></a>
        ";

    $msg = new XML_RPC_Message('listMachines');

    for( nbMachines )
    {   $msg = new XML_RPC_Message('getConfig'); // machine[$i]
        $resp = $cli->send($msg);
        if (!$resp)
        {   echo 'Can\' talk to <b>seraph</b> daemon: ' . $cli->errstr;
            exit;
        }
        if( $resp->faultCode() )
        {   echo 'Fault Code: ' . $resp->faultCode() . "\n";
            echo 'Fault Reason: ' . $resp->faultString() . "\n";
            return ;
        }
        $val = $resp->value();
        $i = $val->arraysize();

        while($i--)
        {   //echo "<option>".XML_RPC_decode($val->arraymem($i))."</option>";
            echo "<b>OS:</b><input type='text' value='NetBSD'/><br>";
        }
        $data = XML_RPC_decode($val);
    }
    echo "</span></p>
        </li>
        </ul>";

}


?>
