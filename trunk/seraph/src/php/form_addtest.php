<?php
require_once 'XML/RPC.php';
require_once 'base_lib.php' ;
session_start();
?>
<html>
<head>
<title>Upload tests.</title>
<LINK href="mystyle.css" rel="stylesheet" type="text/css">
<script type="text/javascript" content="text/html; charset=utf-8">
</script>
</head>

<body class="bheader">
<?php drawMenu(); ?>


<form name="addtest" method="post" action="add_test.php" enctype="multipart/form-data">
    <input type="hidden" name="MAX_FILE_SIZE" value="100000000" />
        <div style="width:25em;"class="header">
            <span><b>Upload tar archive of test.</b></span>
        </div>
        <div>

        <ul class="formFrame">
            <input name="uploadedfile" type="file" /><br /></li>
            </ul>
        </div>

        <div style="width:25em;"class="header">
            <span>&nbsp;</span>
        </div>
        <input type="submit" name="submit" value="Submit"/>
</form>
</body></html>
