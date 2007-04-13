<?php
include 'base_lib.php';
?>
<html>
<head>
<link href='mystyle.css' rel='stylesheet' type='text/css'>
<script type="text/javascript">

function addEvent()
{
var sut_name = document.getElementById('SUT_NAME');
var numi = document.getElementById('SUT_VALUE');
var num = (document.getElementById("SUT_VALUE").value -1)+ 2;
numi.value = num;
var divIdName = "my"+num+"Div";
var newBold = document.createElement('b');
newBold.setAttribute("id",divIdName);
newBold.innerHTML = "El"+num+"added!";
sut_name.appendChild(newBold);
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
