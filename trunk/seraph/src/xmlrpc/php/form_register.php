<?php
require_once 'XML/RPC.php';
require_once 'base_lib.php' ;
?>

<html>
<head>
<LINK href="mystyle.css" rel="stylesheet" type="text/css">
<title>Register new user</title>
</head>

<body class="bheader">
<?php drawMenu(); ?>
<form name="login" method="post" action="register.php">

<div class ="header" style="width:25em">
    <span><b>Registration</b></span>
</div>

<div style="height:100; width:23em">
<ul class="formFrame">
    <li><b>Username</b><input type="text" name="username"/></li>
    <li><b>Password</b><input type="password" name="password"/></li>
    <li><b>Name</b><input type="text" name="name" maxlength="100"/></li>
    <li><b>Email</b><input type="text" name="email" maxlength="100"/></li>
</ul>
    </div>
<div class ="header" style="width:25em">
    <span>&nbsp;</span>
</div>
<input type="submit" name="submit" value="Submit"/>
</form>
</body>
</html>
