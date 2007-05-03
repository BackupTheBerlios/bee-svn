<?php
require_once 'XML/RPC.php';
require_once 'base_lib.php';
session_start();
# collect information from the post
$username = $_POST['username'];
$name = $_POST['name'];
$email = $_POST['email'];
$password = md5($_POST['password']);
echo "pass:".$password."<br>";
$xmlrpc = new XML_RPC_Client('/RPCSERVER',"10.10.129.128", 5000 );
$req = new XML_RPC_Value( array(
                    "sut_username" => new XML_RPC_Value($username,'string'),
                    "sut_name" => new XML_RPC_Value($name,'string'),
                    "sut_email" => new XML_RPC_Value($email,'string'),
                    "sut_password" => new XML_RPC_Value($password,'string'))
            , "struct");
$msg = new XML_RPC_Message('registerUser', array($req) );
$rsp = $xmlrpc->send($msg);
echo "Returned:".XML_RPC_decode($rsp->value())."<br>";
if(hasErrors($rsp) || !(XML_RPC_decode($rsp->value())) )
{
    echo "I'm sorry but the username you specified has already been taken.  Please pick another one.";
    return 0;
}

echo "You have successfully Registered";
?>
