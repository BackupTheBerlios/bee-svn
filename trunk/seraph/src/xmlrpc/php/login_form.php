<?php
include 'base_lib.php';
?>

<html>

<head>
    <link href='mystyle.css' rel='stylesheet' type='text/css'>
    </head>

<body class='bheader'>

<?php drawMenu() ;?>
<form method='post' action='run_tests.php'>
<div >
    Username: <input name='SUT_username' type='text' value='Clear'/><br/>
    Password: <input name='SUT_password' type='text' value='Clear'/>
    <input type='submit' value='Login'/>
    </div>
<br>
</form>
</body>
