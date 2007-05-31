<?php
require_once 'XML/RPC.php';
require_once 'base_lib.php';
session_start();

if(!isset($_FILES['uploadedfile'])) {
        die "Error! Expected file not a part of the form";
}

if($_FILES['uploadedfile']['error'] != UPLOAD_ERR_OK) {
        die "Error! Upload failed.";
}

$fname=$_FILES['uploadedfile']['tmp_name'];
$fsize=filesize($fname);
$hnd  = fopen($fname, "rb");
$ctn  = fread($hnd, $fsize);
fclose($hnd);

$c = new XML_RPC_Client( '/RPCSERVER', $_SESSION["host"], $_SESSION["port"]);
$req = new XML_RPC_Value( array(
                "sut_username" => new XML_RPC_Value( $_SESSION['username'], 'string')
                ,"file_size"    => new XML_RPC_Value( $fsize, 'int')
                ,"uploadedfile"=> new XML_RPC_Value( $ctn, "base64")
            ),"struct" );
$msg = new XML_RPC_Message('addUserTest', array($req));
$rsp = $c->send($msg);
if( hasErrors($rsp) ) return false;
header('location:index.php');

?>
