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


function toggleSpan( id )
{
    var spans = document.getElementsByTagName("span");
    var el = document.getElementById( id );
    var toset ="block";

    if( el.style.display =="block" )
    {
        toset = "none";
    }
    for( var i=0; i < spans.length; i++)
    {
        if(spans[i].style.display == "block")
        {
            spans[i].style.display = "none";
        }
    }
    el.style.display = toset;
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
            <b style="float:left">Name:</b>   <input style="float:right"type='text' name='SUT_MNAME' value=''/><br/>
            <b style="float:left">OS Name:</b><input style="float:right"type='text' name='SUT_OS' value=''/><br/>
            <b style="float:left">OS Ver:</b> <input style="float:right"type='text' name='SUT_OSVER' value=''/><br/>
            <b style="float:left">IP:</b>     <input style="float:right"type='text' name='SUT_MIP' value=''/><br/>
            </span>
            </div>
            <br>
            <input type='submit' value='Add'/>
        </form>
</body>
</html>
