<?php
require_once 'XML/RPC.php';
require_once 'base_lib.php';
session_start();

class User {
    var $xmlrpc=null;
    var $failed = false;
    var $date;
    var $id;

    function User()
    {
    # database information
    $dbhost = "localhost";
    $dbname = "website";
    $dbuser = "root";
    $dbpass = "19323165171";
    $host = "localhost";
    $port = 5000;
        $this->xmlrpc = new XML_RPC_Client('/RPCSERVER',  $host, $port);
        $this->date = gmdate("'Y-m-d'");
        if( !$_SESSION['logged'] )
            $this->checkSession();
        elseif( isset($_COOKIE['mtwebLogin']) ) {}
            //echo "ERR<br>\n";

    }
    function logout()
    {
        session_unregister("username");
        //echo "Bad login<br>";
    }
    function checkSession() {
        $username = $_POST['username'];
        $cookie   = $_SESSION['cookie'];
        $session  = session_id();
        $ip       = $_SERVER['REMOTE_ADDR'];

        echo "USERNAME:".$username;
        $req = new XML_RPC_Value(
            array(
                "sut_username" => new XML_RPC_Value($username,'string'),
                "sut_cookie"   => new XML_RPC_Value($cookie, 'string'),
                "sut_session"  => new XML_RPC_Value($session,'string'),
                "sut_ip" => new XML_RPC_Value($ip, 'string')
                ), "struct");

        $msg = new XML_RPC_Message('checkSession', array($req));
        $rsp = $this->xmlrpc->send($msg);

        if( hasErrors($rsp) ) {
            echo "failed<br>";
            $this->logout();
            return false;
        }else
        {
            $this->setSession($result, false, true);
            echo "result:".XML_RPC_decode($rsp->value());
        }

    }
    function setSession( &$vals, $remember, $init=true)
    {
        $this->id = $vals->id;
        $_SESSION['uid'] = $this->id;
        $_SESSION['username'] = $vals->username;
        $_SESSION['cookie'] = $vals->cookie;
        $_SESSION['logged'] = true;
        if($remember) {
            $this->updateCookie($vals->cookie, true);
        }
        if($init) {
            $session = session_id();
            $ip = $_SERVER['REMOTE_ADDR'];
            $req = new XML_RPC_Value( array(
                    "sut_username"  => new XML_RPC_Value("user1",'string'),
                    "sut_session"   => new XML_RPC_Value($session,'string'),
                    "sut_ip"        => new XML_RPC_Value($ip, 'string'))
                    , "struct");
            $msg = new XML_RPC_Message('setSession', array($req));
            $rsp = $this->xmlrpc->send($msg);

            if( hasErrors($rsp) ) {
                echo "failed";
                $this->logout();
                return false;
            }
        }
    }

    function updateCookie($cookie, $save) {
        $_SESSION['cookie'] = $cookie;
        if ($save) {
            $cookie = serialize(array($_SESSION['username'], $cookie) );
            setcookie('mtwebLogin', $cookie, time() + 31104000, '/directory/');
        }
    }
    function checkLogin($username, $password, $remember) {
        $password = md5($password);
        $req = new XML_RPC_Value(array(
                    "sut_username" => new XML_RPC_Value($username,'string'),
                    "sut_password" => new XML_RPC_Value($password, 'string')
            ,'struct');
        $msg = new XML_RPC_Message('checkLogin', array($req) );
        $rsp = $this->xmlrpc->send($msg);
        if(hasErrors($rsp) )
        {
            $this->failed = true ;
            $this->logout();
            return false;
        }
        $this->setSession($result, $remember);
        return true ;
    }
    function checkRemembered($cookie) {
        list($username, $cookie) = @unserialize($cookie);
        if (!$username or !$cookie) return;
        $req = new XML_RPC_Value(array(
                    "sut_username"=> new XML_RPC_Value($username,'string'),
                    "sut_cookie"=>   new XML_RPC_Value($cookie, 'string')),
                    'struct');
        $msg = new XML_RPC_Message('checkRemembered',array($req));
        $rsp = $this->xmlrpc->send($msg);
        if( hasErrors($rsp) )
            return ;
        $this->setSession($rsp, true);
    } 
}
if( !isset($_SESSION['uid']) )
    session_defaults();
$user = new User();
$user->checkLogin($_POST['username'], $_POST['password'], true);
header('location:index.php');
?>
