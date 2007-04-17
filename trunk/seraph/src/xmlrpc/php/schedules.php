<?php
session_start();
include 'base_lib.php'
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
<table cellspacing='1' width='60%'>
<tr bgcolor="#c8c8ff">
        <td class="center" valign="top" width ="0" nowrap>
        <span class="small">
        <a href="view.php?id=1036" title="[assigned] aSfasfaSF">00001</a><br />&nbsp;     </span>
    </td>

        <td class="left" valign="top" width="100%">

        <span class="small">
        [cli, hsp, html]<br/>
        [2.0] [machine1] [2007-04-12 05:02] [ 12 of 120]
        </span>
    </td>
    <td>
    <span>
    Running
    </span>
    </td>
</tr>
<tr bgcolor="#ffa0a0">
        <td class="center" valign="top" width ="0" nowrap>
        <span class="small">
        <a href="view.php?id=1036" title="[assigned] aSfasfaSF">00002</a><br />&nbsp;     </span>
    </td>

        <td class="left" valign="top" width="100%">

        <span class="small">
        [cli, hsp, html]<br/>
        [2.0] [machine1] [2007-04-12 05:02] [120 of 120]
    <td>
    <span> Complete
    </span>
    </td>
        </span>
    </td>
</tr>
<tr bgcolor="#cceedd">
        <td class="center" valign="top" width ="0" nowrap>
        <span class="small">
        <a href="view.php?id=1036" title="[assigned] aSfasfaSF">00003</a><br />&nbsp;     </span>
    </td>

        <td class="left" valign="top" width="100%">

        <span class="small">
        [cli, hsp, html]<br/>
        [2.0] [machine1] [2007-04-12 05:02] [ 12 of 120]
        </span>
    <td>
    <span> Running
    </span>
    </td>
</tr>
</table>
</body>
</html>
