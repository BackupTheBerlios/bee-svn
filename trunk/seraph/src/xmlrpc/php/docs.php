<?php session_start(); ?>
<html>
<head>
<link href='mystyle.css' rel='stylesheet' type='text/css'>
</head>
<body class='bheader'>
<?php require_once 'base_lib.php'; 
    if( !drawMenu() ){
        echo "</body></html>";
        return;
    }
?>
<pre>
TODO.
=========================
Add Documents from the libraries of testAPI.


Testing Framework.
=========================
    In software development, a framework is a defined support structure in which another
software project can be organized and developed.
The presented framework facilitates the developement of test scripts and includes the
folowing modules:
        a) testCases( testing scripts)
        b) testDriver(seraph-support program)
        c) testAPI  ( code library)

Specifications.
=========================
1. testCases
   ---------
   A testCase usually test one aspect of the tested product and is
   composed of a testscript and a runtest.bat file.
   The testscript may be written in any language( preferably perl )
   while runtest.bat contains extra setups needed by the test and
   eventually the comment
        #sut_fi=yes(no)
   and specify weather configuration will be reseted or not

   Ex: after running this testCase, state will be reseted.
        perl ./apop/apop.pl
        # sut_fi=yes

    For perl script one may use the testAPI composed of some home-grown modules
    for specific protocols like POP3, SMTP, CLI.

    TestCases are organized after the protocol they test,
    thus all testCases testing pop3 will be placed inside the directory
    ./tests/pop3/.


2. Test Driver
   --------------

    A testDriver is a process who runs testCases and records the failures.
    The curent implemented solution is described below :


                                                                             rsh->system/ServerUnderStress
                                                                            /
                                                                           /
                              |- system( copy, rm, mkdir, exec)|          (remote)
                              |- refresh                       |         /
seraph* runScript-----calls ->|- stop                          |->effects
                              |- start                         |         \
                              |- checkCore                     |          (local)
                              |- runScript                     |           \
                                                                            \
                                                                             system/ServerUnderStress

Note:
    seraph needs read access on /var/log/maillog. This can be achieved through two ways :
    either run seraph as root( not recomended ) or you can replace the umask
    in the syslog init script ( /etc/init.d/syslog ) from 077 to 0034, which would create
    maillog with read permission for others.


3. testAPI
   ----------------------
        In writing a testScript, you could use the testAPI composed of perl modules
    to ease your work and concentrate on the more important stuff, rather than
    dealing with the language barriers.
    Another purpose of the testAPI reuse code and keep track
    of each step a testScript is performing.

    Each protocol has it's corespondent Perl module,
    such as POP3 has the library called ServerUnderStress::Pop3.

    The functions implemented in a module are a direct corespondent
    of protocols command.
    Ex:
        Pop3::User("userName") will send POP3 command "user userName",
        Pop3::Stat() will send "stat".

    The current testAPI has four modules:
    ServerUnderStress::Smtp, ServerUnderStress::POP3, ServerUnderStress::Cli and ServerUnderStress::Utils.
    A better description of each of them is found in the ./engine/ directory

3.1. Environment Variables:
   ------------------------
    To provide a network transparent script execution, certain environent
    variables are exported such as:

    sut_host        The machine where ServerUnderStress resides
    sut_errorlog    Full path where to place seraph errorlog
    sut_imap        IMAP            port
    sut_imaps       IMAP     [SSL]  port
    sut_pop         POP3            port
    sut_pops        POP3     [SSL]  port
    sut_smtp        SMTP            port
    sut_smtps       SMTP     [SSL]  port
    sut_webmail     WebMail         port
    sut_webmails    WebMail  [SSL]  port
    sut_webadmin    WebAdmin        port
    sut_webadmins   WebAdmin [SSL]  port
    sut_cli         CLI             port
    sut_clis        CLI      [SSL]  port
    sut_user        Default user used in tests( user1 )
    sut_pass        Password of the default user
    sut_defdom      Default domain
    sut_start       Command that STARTs ServerUnderStress in a syncronous way
    sut_stop        Command that STOPs ServerUnderStress in a syncronous way
    sut_reload      Command that RELOADs ServerUnderStress in a syncronous way
    sut_tool        Directory holding the framework( Perl modules and start/stop tools)
    sut_workdir     ServerUnderStress Workdir
    sut_sendmail    Sendmail binary
    sut_adminpass   Admin pass, used with CLI
    sut_bin         The path to the ServerUnderStress binary file
    sut_fresh       A fresh copy of the ServerUnderStress work folder, with the default configuration for seraph
    sut_cfgfile     was sut_cfg_file( copied when a core is droped )
    sut_coredir     was sut_core_dir( where ServerUnderStress drops a core ) ( scanned after each test is runned )
    sut_dbgdir      was sut_dbg_dir
    sut_refresh	    was refresh_client - Specify what tool will perform a SUT refresh
    sut_syslog      Path to the syslog/maillog (where ServerUnderStress logs system messages)
    sut_smtpfilters Path to smtpFilters.script

    Scripts can read environent variables using getenv() calls
    or using autils.pm from the testAPI

    testSuite->
        testCase ->
                testDir->testScript
                       ->runtest.bat

A stable branch has been created, under the name of seraph-1-0. First you should checkout a working copy
by using a `cvs co -r seraph-1-0` command. Upon later modification in the source tree, a simple commit
will syncronize the changes into the branch.
</pre>
</body>
</html>
