<?php
session_start();
require_once 'base_lib.php';
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

</script>
</head>

<body class='bheader'>

    <?php
    if( !drawMenu() ){
        echo "</body></html>";
        return;
    }
    drawMachines() ;?>

    <hr/>
    <form name="add_machine" method="post" action="add_machine.php">
        <div class='column' style="width:22em">
            <br>
            <span>
            <b style="float:left">Name:</b>   <input style="float:right"type='text' name='SUT_MNAME' value='ciociolina'/><br/>
            <b style="float:left">OS Name:</b><input style="float:right"type='text' name='SUT_OS' value='NetBSD'/><br/>
            <b style="float:left">OS Ver:</b> <input style="float:right"type='text' name='SUT_OSVER' value='3.0'/><br/>
            <b style="float:left">IP:</b>     <input style="float:right"type='text' name='SUT_MIP' value='193.230.245.6'/><br/>
            </span>
            </div>
            <br>
            <input type='submit' value='Add'/>
        </form>
</body>
</html>
