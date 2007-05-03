<?php
require_once 'XML/RPC.php';
require_once 'base_lib.php' ;
?>

<html>
<head>
<LINK href="mystyle.css" rel="stylesheet" type="text/css">
</head>

<body class="bheader">
<?php drawMenu(); ?>
<form name="login" method="post" action="register.php">

<div class ="header" style="width:25em">
    <span><b>Registration</b></span>
</div>

<div id="menu" style="height:100; width:25em">
    <b style="width:100;">Username:</b>
    <input type="text" name="username"/>

    <b style="width:100;">Password:</b>
    <input type="password" name="password"/>

    <b style="width:100">Name:</b>
    <input type="text" name="name" maxlength="100"/>

    <b style="width:100">Email:</b>
    <input type="text" name="email" maxlength="100"/>

    </div>
<div class ="header" style="width:25em">
    <span>&nbsp;</span>
</div>
<input type="submit" name="submit" value="Submit"/>
</form>
</body>
</html>
