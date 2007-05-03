<?php
session_start();
require_once 'base_lib.php';

function drawMachines()
{
    $cli = new XML_RPC_Client('/RPCSERVER', $_SESSION[host], $_SESSION["port"]);

    $msg = new XML_RPC_Message('listMachines');
    $resp = $cli->send($msg);
    if( hasErrors($resp) ) return;

    $machines = $resp->value();
    $i = $machines->arraysize();

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
?>

<html>
<head>
<link href='mystyle.css' rel='stylesheet' type='text/css'>
<script type="text/javascript">

function addEvent(spanId, sName, sValue)
{
    var sut_name = document.getElementById(spanId);
    var name = document.getElementById(sName);
    var value = document.getElementById(sValue);
    var newBold = document.createElement('b');
    newBold.innerHTML = name.value;
    var newInput = document.createElement('input');
    newInput.setAttribute('type','text');
    newInput.setAttribute('name',name.value);
    newInput.value=value.value;
    sut_name.appendChild(newBold);
    sut_name.appendChild(newInput);
}

function removeEvent(divNum)
{
    var d = document.getElementById('myDiv');
    var olddiv = document.getElementById(divNum);
    d.removeChild(olddiv);
}


function toggleSpan( id )
{
    var spans = document.getElementsByTagName("span");
    var el = document.getElementById( id );

    for( var i=0; i < spans.length; i++)
    {
        if(spans[i].style.display == "block")
        {
            spans[i].style.display = "none";
        }
    }
    el.style.display = "block";
}

</script>
</head>

<body class='bheader'>

    <?php
    if( !drawMenu() ){
        echo "</body></html>";
        return;
    }?>
    <form name="add_machine" method="post" action="add_machine.php">
        <div class='column' style="width:22em">
            <span>
            <b style="float:left">Name:</b>   <input style="float:right"type='text' name='SUT_MNAME' value=''/><br/>
            <b style="float:left">OS Name:</b><input style="float:right"type='text' name='SUT_OS' value=''/><br/>
            <b style="float:left">OS Ver:</b> <input style="float:right"type='text' name='SUT_OSVER' value=''/><br/>
            <b style="float:left">IP:</b>     <input style="float:right"type='text' name='SUT_MIP' value=''/><br/>
            </span>
            </div>
            <br>
            <input type='submit' value='Add'/>
        </form>
        <br style='clear:both;'>
        <hr/>
    Machines:
    <?php drawMachines() ;?>


</body>
</html>
