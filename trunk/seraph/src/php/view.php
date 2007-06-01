<?php
session_start();
require_once 'XML/RPC.php';
include 'base_lib.php' ;?>
<html>
<head>
<LINK href="mystyle.css" rel="stylesheet" type="text/css">
</head>
<body class='bheader'>

<?php drawMenu();
echo "<object style='background-color:white' width='100%' height='50%' type='text/plain' data='view_job.php?log=".$_REQUEST['log']."'> </object>";
//showInfo();
?>
