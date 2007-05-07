<?php
require_once 'XML/RPC.php';
require_once 'base_lib.php' ;
?>
<html>
<head>
<title>Login</title>
<LINK href="mystyle.css" rel="stylesheet" type="text/css">
<script type="text/javascript" content="text/html; charset=utf-8">
</script>
</head>

<body class="bheader">
<?php drawMenu(); ?>
<form name="login" method="post" action="login.php">
        <div style="width:25em;"class="header">
            <span><b>Login</b></span>
        </div>
        <div style="height:4em;width:23em">
        <ul class="formFrame">
            <li><b>Username</b><input type="text" name="username"/></li>
            <li><b>Password</b><input type="password" name="password"/></li>
        </ul>
        </div>
        <div style="width:25em;"class="header">
            <span>&nbsp;</span>
        </div>
        <input type="submit" name="submit" value="Submit"/>
</form>
</body></html>
