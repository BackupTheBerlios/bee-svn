<?php
require_once 'XML/RPC.php';
require_once 'base_lib.php';
session_start();
session_defaults();
# collect information from the post
$username = $_POST['username'];
$name = $_POST['name'];
$email = $_POST['email'];
$password = md5($_POST['password']);
$xmlrpc = new XML_RPC_Client('/RPCSERVER',$_SESSION["host"], $_SESSION["port"] );
$req = new XML_RPC_Value( array(
                    "sut_username" => new XML_RPC_Value($username,'string'),
                    "sut_name" => new XML_RPC_Value($name,'string'),
                    "sut_email" => new XML_RPC_Value($email,'string'),
                    "sut_password" => new XML_RPC_Value($password,'string'))
            , "struct");
$msg = new XML_RPC_Message('registerUser', array($req) );
$rsp = $xmlrpc->send($msg);
?>
<!-- Begin Site -->
<html>
<head>
    <LINK href="mystyle.css" rel="stylesheet" type="text/css">
    <LINK REL="SHORTCUT ICON" href="seraph.ico">
    <title>Register new user</title>
    </head>

<body class='bheader' >

<?php
drawMenu();
if(hasErrors($rsp) || !(XML_RPC_decode($rsp->value())) )
{
    echo "I'm sorry but the username you specified has already been taken.  Please pick another one.";
    return 0;
}
//echo "Returned:".XML_RPC_decode($rsp->value())."<br>";

echo "You have successfully Registered";
?>
