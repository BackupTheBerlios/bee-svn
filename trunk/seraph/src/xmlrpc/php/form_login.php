<?php
require_once 'XML/RPC.php';
require_once 'base_lib.php' ;
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
        <div class="menu" style="height: 3em;width:23em">
        <ul>
            <li><b style="width:6em">Username</b><input type="text" name="username"/></li>
            <li><b style="width:6em">Password</b><input type="password" name="password"></b></li>
        </ul>
        </div>
        <div style="width:23em;"class="header">
            <span>&nbsp;</span>
        </div>
        <input type="submit" name="submit" value="Submit"></p>
</form>
</body></html>
