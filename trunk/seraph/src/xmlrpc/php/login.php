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
    $dbpass = "";
        #connect to database
        mysql_connect ( $dbhost, $dbuser, $dbpass)or die("Could not connect: ".mysql_error());
        mysql_select_db($dbname) or die(mysql_error());
        //$this->xmlrpc = new XML_RPC_Client('/RPCSERVER',  $host, $port);
        $this->date = gmdate("'Y-m-d'");
        //echo $date."<br>\n";
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
        $username = $_SESSION['username'];
        $cookie   = $_SESSION['cookie'];
        $session  = session_id();
        $ip       = $_SERVER['REMOTE_ADDR'];

        //echo "$username, $cookie, $sess, $ip<br>\n";

        $sql = "SELECT * FROM member WHERE " .
            "(username = '$username') AND (cookie = '$cookie') AND " .
            "(session = '$session') AND (ip = '$ip')";
        //echo "HERE<br>";
        $result = mysql_query($sql) or die(mysql_error());
        //$this->xmlrpc->checkSession($username, $cookie, $session, $ip);
        if( mysql_num_rows($result) !=0 ) {
            $this->setSession($result, false, false);
        } else {
            $this->logout();
        }
    }
    function setSession( &$values, $remember, $init=true)
    {
        $this->id = $values->id;
        $_SESSION['uid'] = $this->id;
        $_SESSION['username'] = $values->username;
        $_SESSION['cookie'] = $values->cookie;
        $_SESSION['logged'] = true;
        if($remember) {
            $this->updateCookie($values->cookie, true);
        }
        if($init) {
            $session = session_id();
            $ip = $_SERVER['REMOTE_ADDR'];
            $sql = "UPDATE member SET session = '$session', ip = '$ip' WHERE " .
                    "id = '$this->id'";
            $result = mysql_query($sql) or die(mysql_error());
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
        //$username = $this->db->quote($username);
        $password = md5($password);
        $sql = "SELECT * FROM member WHERE " .
            "username = '$username' AND " .
            "password = '$password'";
        $result = mysql_query($sql) or die(mysql_error());
        if( mysql_num_rows($result) !=0 ) {
            $this->setSession($result, $remember);
            return true;
        } else {
            $this->failed = true;
            $this->logout();
            return false;
        }
    }
    function checkRemembered($cookie) {
        list($username, $cookie) = @unserialize($cookie);
        if (!$username or !$cookie) return;
        //$username = $this->db->quote($username);
        //$cookie = $this->db->quote($cookie);
        $sql = "SELECT * FROM member WHERE " .
            "(username = '$username') AND (cookie = '$cookie')";
        $result = mysql_query($sql) or die(mysql_error());
        if( mysql_num_rows($result) !=0 ) {
            $this->setSession($result, true);
        }
    } 
}
if( !isset($_SESSION['uid']) )
    session_defaults();
$user = new User();
$user->checkLogin($_POST['username'], $_POST['password'], true);
header('location:index.php');
?>
