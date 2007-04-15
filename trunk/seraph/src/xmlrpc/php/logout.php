<?php
session_start();
unset($_SESSION['username']);
unset($_SESSION['password']);
$_SESSION = NULL ; // reset session array
session_destroy();   // destroy session.
$_POST = NULL ;
header('location:index.php');
?>
