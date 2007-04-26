<?php
require_once 'base_lib.php';
session_start();
session_destroy();   // destroy session.
unset($_SESSION['username']);
unset($_SESSION['password']);
$_SESSION = NULL ; // reset session array
$_POST = NULL ;
header('location:index.php');
?>
