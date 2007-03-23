<html>
<head>
<link href='mystyle.css' rel='stylesheet' type='text/css'>
</head>

<body class='bheader'>
<div  class='header'>
<font class='header'>Seraph</font><br/><br/>
<div class='head_menu'><br><br></div>
</div>

IMAP:
<input type='text' value='1480'/>
<br>

SMTP:
<input type='text' value='2580'/>
<br>

POP3: <input type='text' value='1180'/> <br>
IMAP:<input type='text' value='1430'/><br>
SMTP:<input type='text' value='2500'/><br>
POP:<input type='text' value='1100'/><br>
PATH:<input type='text' value='/var/SUT'/><br>
TOOL:<input type='text' value='${PWD}/engine'/><br>
WORKDIR:<input type='text' value='/var/SUT/'/><br>
FRESH:<input type='text' value='/home/SUT/'/><br>
DBGDIR:<input type='text' value='/var/SUT/logs/debug'/><br>
COREDIR:<input type='text' value='/var/SUT'/><br>
<span style='display:inline; padding: 10px; 1px; 1px; 1px'>CFGFILE:</span><input type='text' value='/var/SUT/run/SUT.cfg'/><br>
<span style='display:inline; padding: 10px; 1px; 1px; 1px'>STOP:</span><input type='text' value='stop -c "pkill SUT"'/><br>
BIN:<input type='text' value='/usr/bin/SUT'/><br>
START:<input type='text' value='start -c "${SUT_bin} --drop-core ${SUT_coredir} -u root -g wheel -l 15 -f ${SUT_dbgdir}" && sleep 1'/><br>
SYSLOG:<input type='text' value='/var/log/maillog'/><br>
SMTPFILTERS:<input type='text' value='/var/SUT/smtpFilters.script'/><br>
REFRESH_CLIENT:<input type='text' value ='refresh'/><br>


</body>
</html>
