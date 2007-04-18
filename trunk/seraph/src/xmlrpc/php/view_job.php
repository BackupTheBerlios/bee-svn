<?php session_start(); ?>
<html>
<head>
<LINK href="mystyle.css" rel="stylesheet" type="text/css">
</head>
<body class='bheader'>
<?php
require_once 'base_lib.php';
drawMenu();
echo '<pre>
./372.pl: Incorrect response tag, expected: 250  and got: 451 Processing module not initialized\r\n
./383.pl: -ERR Wrong username/password pair\r\n
./386.pl: -ERR Wrong username/password pair\r\n
./top.pl: Incorrect response tag, expected: 250  and got: 451 Processing module not initialized\r\n
./wrong_cram.pl: -ERR Wrong SASL response\r\n
./wrong_pass.pl: -ERR Unknown command <}user anotherUser>\r\n
./152.quit.pl: +OK POP3 server closing connection\r\n
./capa.pl: TOP\r\n
./dele.pl: Incorrect response tag, expected: 250  and got: 451 Processing module not initialized\r\n
./uidl.pl: -ERR Must authenticate first\r\n
./159-1.pl: -ERR Wrong username/password pair\r\n
./157.uidl.pl: -ERR Wrong username/password pair\r\n
./189.deleuidl.pl: +OK user1@localdomain needs a password\r\n
./763.auth_syntax.pl: -ERR Wrong SASL response\r\n
./154.top_1_2.pl: Incorrect response tag, expected: 250  and got: 451 Processing module not initialized\r\n
./137.retr_0.pl: -ERR Wrong username/password pair\r\n
./764.multiple_capa.pl: TOP\r\n
./163.user_user1.pl: -ERR Wrong username/password pair\r\n
./149.normal_noop.pl: -ERR Wrong username/password pair\r\n
./136.retr_dsf.pl: -ERR Wrong username/password pair\r\n
./148.noop_a.pl: -ERR Wrong username/password pair\r\n
./138.retr_neg6.pl: -ERR Wrong username/password pair\r\n
./143.dele_neg4.pl: -ERR Wrong username/password pair\r\n
./181.deledeleted.pl: +OK user1@localdomain needs a password\r\n
./768.user_after_apop.pl: +OK user1 needs a password\r\n
./172.rset_a.pl: -ERR Wrong username/password pair\r\n
./160.uidl_123456789.pl: -ERR Wrong username/password pair\r\n
./131.list_0.pl: -ERR Wrong username/password pair\r\n
./387.normal_login.pl: -ERR Wrong username/password pair\r\n
./159.uidl_0.pl: -ERR Wrong username/password pair\r\n
./153.top_1_q.pl: -ERR Wrong username/password pair\r\n
./382.dele_1_1.pl: -ERR Wrong username/password pair\r\n
./187.delerset.pl: Incorrect response tag, expected: 250  and got: 451 Processing module not initialized\r\n
./171.dele_list_uidl_stat.pl: Incorrect response tag, expected: 250  and got: 451 Processing module not initialized\r\n
./166.pass_user2.pl: -ERR Wrong username/password pair\r\n
./162.uidl_1_1.pl: -ERR Wrong username/password pair\r\n
./127.stat_1.pl: -ERR Wrong username/password pair\r\n
./129.stat.pl: -ERR Wrong username/password pair\r\n
./155.top_123456789_1.pl: -ERR Wrong username/password pair\r\n
./132.list.pl: -ERR Wrong username/password pair\r\n
./142.dele_123456789.pl: -ERR Wrong username/password pair\r\n
./167.pass_user1.pl: -ERR Wrong username/password pair\r\n
./135.retr.pl: -ERR Wrong username/password pair\r\n
./144.dele_a.pl: -ERR Wrong username/password pair\r\n
./766.pass.pl: -ERR Protocol violation\r\n
./145.dele.pl: -ERR Wrong username/password pair\r\n
./130.list_a.pl: -ERR Wrong username/password pair\r\n
./149.noop.pl: -ERR Wrong username/password pair\r\n
./146.dele_1.pl: +OK user1@localdomain needs a password\r\n
./login_auth.pl: -ERR Wrong username/password pair\r\n
./141.dele_0.pl: -ERR Wrong username/password pair\r\n';
?>
</pre>
