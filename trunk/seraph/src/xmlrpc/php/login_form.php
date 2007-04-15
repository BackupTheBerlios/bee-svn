<?php
require_once 'XML/RPC.php';
include 'base_lib.php' ;
?>
<html>
<head>
<title>Login</title>
<LINK href="mystyle.css" rel="stylesheet" type="text/css">
<script type="text/javascript" content="text/html; charset=utf-8">
//document.write('<link rel="stylesheet" type="text/css" href="lstyle.css" title=main"');
</script>
</head>

<body class="bheader">
<?php drawMenu(); ?>
<form name="login" method="post" action="login.php">
        <div style="width:23em;"class="header">
            <span><b>Login</b></span>
        </div>
        <div id="menu" style="height: 3em;width:23em">
            <b style="width:6em">Username</b><input type="text" name="username"/>
            <b style="width:6em">Password</b><input type="password" name="password"></b>
        </div>
        <div style="width:23em;"class="header">
            <span>&nbsp;</span>
        </div>
        <input type="submit" name="submit" value="Submit"></p>
</form>
</body></html>
