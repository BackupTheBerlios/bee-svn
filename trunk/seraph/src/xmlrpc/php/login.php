<?php
require_once 'XML/RPC.php';
require_once 'base_lib.php';


function session_defaults() {
    $_SESSION['logged'] = false;
    $_SESSION['uid'] = 0;
    $_SESSION['username'] = '';
    $_SESSION['cookie'] = 0;
    $_SESSION['remember'] = false;
}


class User {
    var $xmlrpc=null;
    var $failed = false;
    var $date;
    var $id;

    function User($host, $port, $date)
    {
        $this->xmlrpc = new XML_RPC_Client('/RPCSERVER', $host, $port);
        $this->date = $date;
        echo $date."<br>\n";
        if( !$_SESSION['logged'] )
            $this->checkSession();
        elseif( isset($_COOKIE['mtwebLogin']) )
            echo "ERR<br>\n";

    }
    function checkLogin($username, $password, $remember)
    {
        $password = md5($password);
        $req = 'getRow'; /* Build XMLRPC request */
        $resp = $this->xmlrpc->send($req);
        if( hasErrors($resp) )
        {   $this->failed = true;
            $this->logout();
            return false;
        }

        $this->setSession($result, $remember);
        return true;
    }
    function checkSession() {
        $username = $_SESSION['username'];
        $cookie = $_SESSION['cookie'];
        $sess   = session_id();
        $ip     = $_SERVER['REMOTE_ADDR'];
        echo "$username, $cookie, $sess, $ip<br>\n";
        /*$sql = "SELECT * FROM member WHERE " .
            "(username = $username) AND (cookie = $cookie) AND " .
            "(session = $session) AND (ip = $ip)";
        $result = $this->db->getRow($sql);
        if (is_object($result) ) {
            $this->_setSession($result, false, false);
        } else {
            $this->_logout();
        }*/
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
            $req = "Set session=$session, ip=$ip, where id=$this->id";
        }
    }
}
    session_start();
?>

<html>
<head>
<link href='mystyle.css' rel='stylesheet' type='text/css'>
</head>
<body class='bheader'>
<?php
    drawMenu() ;
    if( !isset($_SESSION['uid']) )
        session_defaults();
    $user = new User('localhost', 5000, gmdate("'Y-m-d'"));
    showInfo();
?>
    <br>
</body>
</html>
