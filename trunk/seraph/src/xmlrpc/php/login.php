<?php
# database information
$dbhost = "localhost";
$dbname = "website";
$dbuser = "root";
$dbpass = "19323165171";

#connect to database
mysql_connect ( $dbhost, $dbuser, $dbpass)or die("Could not connect: ".mysql_error());
mysql_select_db($dbname) or die(mysql_error());
# start the session, grab the variables from the login form and then check data
session_start();
session_register("username");
$username = $_POST['username'];
$_POST['password'] = stripslashes($_POST['password']);
$password = md5($_POST['password']);
$query = "SELECT username,password from member where username='$username' and password='$password'";
$result = mysql_query($query) or die(mysql_error());
# do they match ?
if( mysql_num_rows($result) ==0 ) {
    echo "Bad login";
    include "login.html";
}else {
    $_SESSION['username'] = "$username" ;
    header('location:index.php');
}


mysql_close();
exit();
?>
