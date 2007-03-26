<?php
include 'mystyle.php';
?>
<html>
<head>
<link href='mystyle.css' rel='stylesheet' type='text/css'>
</head>

<body class='bheader'>

<?php drawMenu() ;?>
<?php drawMachines() ;?>
<!--
<input type='button' value='Add'/>
Machines:
<div >
<select>
<option value='NetBSD/3.0'>NetBSD/3.0</option>
<option value='OpenBSD/3.9'>OpenBSD/3.9</option>
<option value='linux/gcc3'>linux/gcc3</option>
<option value='linux/gcc4'>linux/gcc4</option>
<option value='Solaris/10'>Solaris/10</option>
</select>
<input type='text' value='192.168.8.29'/>
</div>
<br>
-->
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
