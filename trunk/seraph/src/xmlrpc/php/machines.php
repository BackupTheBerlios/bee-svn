<?php
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

<?php drawMenu() ;?>
<?php drawMachines() ;?>
<hr/>
<div>
<input type='button' value='Add'/>
Machine:
<br>
<span>
<b>Name:</b><input type='text' value='ciociolina'/><br/>
<b>OS Name:</b><input type='text' value='NetBSD'/><br/>
<b>OS Ver:</b><input type='text' value='3.0'/><br/>
<b>IP:</b><input type='text' value='193.230.245.6'/><br/>
</span>
</div>
</body>
</html>
