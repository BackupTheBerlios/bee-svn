<?php
include 'base_lib.php';
?>
<html>
<head>
<link href='mystyle.css' rel='stylesheet' type='text/css'>
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
