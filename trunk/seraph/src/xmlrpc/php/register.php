<?php
session_start();
# database information
$dbhost = "localhost";
$dbname = "website";
$dbuser = "root";
$dbpass = "19323165171";
mysql_connect ( $dbhost, $dbuser, $dbpass)or die("Could not connect: ".mysql_error());
mysql_select_db($dbname) or die(mysql_error());

# collect information from the post
$username = NULL ;
$password = NULL ;
$username = $_POST['username'];
$password = md5($_POST['password']);

# check if user allready registred
$checkuser = mysql_query("SELECT username FROM member WHERE username='$username'");
$username_exist = mysql_num_rows($checkuser);

if($username_exist > 0){
    echo "I'm sorry but the username you specified has already been taken.  Please pick another one.";
    unset($username);
    include 'register.html';
    exit();
}

# register user
$query = "INSERT INTO member (username, password) VALUES('$username', '$password')";
mysql_query($query) or die(mysql_error());
mysql_close();

echo "You have successfully Registered";
?>
