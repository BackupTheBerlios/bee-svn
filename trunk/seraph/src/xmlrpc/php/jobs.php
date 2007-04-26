<?php
session_start();
include 'base_lib.php'; 
    function listJobs()
    {
    $job_pending =1;
    $job_running=2;
    $job_complete=4;
    $job_all=8;
        $xmlrpc = new XML_RPC_Client('/RPCSERVER', "localhost", 5000 );
        echo " List of Jobs:(Opens log on click)<br>";
        /*-------------------------------------------*/
        $state = "job_running";

        $req = new XML_RPC_Value( array(
                            "sut_username" => new XML_RPC_Value( "user1",'string'),
                            "job_type"   => new XML_RPC_Value( $job_running,'int'))
                    , "struct");

        $msg = new XML_RPC_Message('listJobs', array($req));
        $resp = $xmlrpc->send($msg);
        if( hasErrors($resp) ) return false;

        $i = $resp->value()->arraysize();
        echo "NbJobs:$i<br>";
/*
<table class="width100" cellspacing="1">
<tr>
        <td class="form-title" colspan="2">
        <a class="subtle" href="view_all_set.php?type=1&amp;temporary=y&amp;handler_id=[none]&amp;hide_status=90">Unassigned</a> [<a class="subtle" href="view_all_set.php?type=1&amp;temporary=y&amp;handler_id=[none]&amp;hide_status=90" target="_blank">^</a>]      (1 - 10 / 16)   </td>

</tr>


<tr bgcolor="#ffa0a0">
        <td class="center" valign="top" width ="0" nowrap>
        <span class="small">
        <a href="view.php?id=1099" title="[new] Inserting 15 new fish breaks the hyperdrive">0001099</a><br /><img src="http://www.futureware.biz/mantisdemo/images/priority_2.gif" alt="" title="urgent" />        </span>
    </td>

        <td class="left" valign="top" width="100%">

        <span class="small">
        Inserting 15 new fish breaks the hyperdrive     <br />
        [Demo] GUI - <b>2007-04-26 00:36</b>        </span>
    </td>
</tr>
*/
/*
    echo "<table cellspacing='1' bgcolor='red' border='solid'>";
    while($i--)
    {
        $log = XML_RPC_decode($resp->value()->arraymem($i));
        echo "<tr> <td>1000</td>";
        echo "<td>$state</td>";
        echo "<td>$log</td></tr>";
    }
        echo "</table>";
*/
        while($i--) {
            $log = XML_RPC_decode($resp->value()->arraymem($i));
            echo "<div class='$state'>";
            echo "<a href='view.php?log=$log'>";
            echo "<span>1000</span>";
            echo "<span><b >$state</b></span>";
            echo "<span>$log</span>";
            echo "</a>";
            echo "</div>";
        }
        /*---------------------------------------------*/
        /*
        while($i++<3) {
        echo "<div class='job_running_ok'>";
        echo "<a href='http://google.com'>";
            echo "<span>1000</span>";
            echo "<span><b >Running</b></span>";
            echo "<span>".XML_RPC_decode($resp->value()->arraymem($i)) ."</span>";
        echo "</a>";
        echo "</div>";
        }
        while($i++<6) {
        echo "<div class='job_pending'>";
        echo "<a href='http://google.com'>";
            echo "<span>1000</span>";
            echo "<span><b >Pending</b></span>";
            echo "<span>".XML_RPC_decode($resp->value()->arraymem($i)) ."</span>";
        echo "</a>";
        echo "</div>";
        }*/
    }
?>
<html>
<head>
<link href='mystyle.css' rel='stylesheet' type='text/css'>
</head>

<body class='bheader'>
<?php
if( !drawMenu() ){
    echo "</body></html>";
    return;
}
?>

<form>
<div class='column'>
<span>
CronName:<br>
<input type='text' name='name' value='cronName'/></span>
<span>
Month / year / day <br>
<select name="start_month">
<option value="1" selected="selected">January</option><option value="2">February</option><option value="3">March</option><option value="4">April</option><option value="5">May</option><option value="6">June</option><option value="7">July</option><option value="8">August</option><option value="9">September</option><option value="10">October</option><option value="11">November</option><option value="12">December</option>
</select>

<select name="start_day">
<option value="1"> 1 </option><option value="2"> 2 </option><option value="3"> 3 </option><option value="4"> 4 </option><option value="5"> 5 </option><option value="6"> 6 </option><option value="7"> 7 </option><option value="8"> 8 </option><option value="9"> 9 </option><option value="10"> 10 </option><option value="11" selected="selected"> 11 </option><option value="12"> 12 </option><option value="13"> 13 </option><option value="14"> 14 </option><option value="15"> 15 </option><option value="16"> 16 </option><option value="17"> 17 </option><option value="18"> 18 </option><option value="19"> 19 </option><option value="20"> 20 </option><option value="21"> 21 </option><option value="22"> 22 </option><option value="23"> 23 </option><option value="24"> 24 </option><option value="25"> 25 </option><option value="26"> 26 </option><option value="27"> 27 </option><option value="28"> 28 </option><option value="29"> 29 </option><option value="30"> 30 </option><option value="31"> 31 </option>
</select>

<select name="start_year">
<option value="2007" selected="selected"> 2007 </option><option value="2006"> 2006 </option><option value="2005"> 2005 </option><option value="2004"> 2004 </option><option value="2003"> 2003 </option><option value="2002"> 2002 </option><option value="2001"> 2001 </option><option value="2000"> 2000 </option>
</select></span>
<span>
Hour:Minutes<br>
<select name="start_hour">
<option selected="selected" value="1"> 1 </option><option value="2"> 2 </option><option value="3"> 3 </option><option value="4"> 4 </option><option value="5"> 5 </option><option value="6"> 6 </option><option value="7"> 7 </option><option value="8"> 8 </option><option value="9"> 9 </option><option value="10"> 10 </option><option value="11" > 11 </option><option value="12"> 12 </option><option value="13"> 13 </option><option value="14"> 14 </option><option value="15"> 15 </option><option value="16"> 16 </option><option value="17"> 17 </option><option value="18"> 18 </option><option value="19"> 19 </option><option value="20"> 20 </option><option value="21"> 21 </option><option value="22"> 22 </option><option value="23"> 23 </option><option value="24"> 24 </option>
<input type='text' name='name' value='00' size='2'/></span>
</select></span>
<br>
<input type='button' value='Add'/>
</div>
</form>
<br>
<hr>

Current Schedules:<br>
<?php listJobs(); ?>
</body>
</html>
