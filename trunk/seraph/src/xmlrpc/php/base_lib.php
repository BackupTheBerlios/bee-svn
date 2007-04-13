<?php
require_once 'XML/RPC.php';

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

function drawMenu() {
    echo "<div  class='header'>
        <font class='header'>Seraph</font>
        <br/><br/>
        </div>
    <div class='head_menu'>
        <ul>
            <li><a href='.'>Home</a></li>
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


    $msg = new XML_RPC_Message('listMachines');
    $resp = $cli->send($msg);
    if( hasErrors($resp) ) return;

    $machines = $resp->value();
    $i = $machines->arraysize();
    #$i = 1;
    while( $i-- )
    {   $m = XML_RPC_decode( $machines->arraymem($i) );
        echo "<form name='$m' action='set_config.php?SUT_MACHINE=$m' method='post'> <ul id='menu'>
        <li ><p>$m
        <span id='span$m'>
        <a href='javascript:;' onclick='addEvent(\"span$m\", \"sName$m\", \"sValue$m\");'><em class='butt'>Add</em></a>
        <input type='submit' value='Save'>

        <input id='sName$m'  type='text' value='SUT_NAME' style='text-align:right; clear:both; float:left; width:12em;'/>
        <input id='sValue$m' type='text' value='Value'/>


        ";
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
        echo "

        </span></p>
            </li>
            </ul>

</form>";
    }

}


?>
