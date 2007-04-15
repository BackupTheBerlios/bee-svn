<?php
session_start();
include 'base_lib.php'
?>
<html>
<head>
<link href='mystyle.css' rel='stylesheet' type='text/css'>
</head>

<body class='bheader'>
<?php
    if( !drawMenu() ){
        echo "</body></html>";
        return;
    }
?>

<div>
<input type='button' value='Add'/>
Notification emails:
<br>
<input type='text' value='cucu@gigi.com'/>
</div>
<br>

<div>
<input type='button' value='Add'/>
Notification Jabber Account:
<br>
<input type='text' value='groleo@jabber.org'/>
</div>
<br>


</body>
</html>
