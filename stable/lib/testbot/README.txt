

Axigen Testing Framework.
=========================
    The Axigen Testing Framework is composed of two modules:
        1. the testCases.
        2. utility library.
        3. the testDriver(testbot).

testSuite->
    testCase ->
            testDir->testScript
            testBatch

1. Tests
--------
    The test suite is composed of what is known as a testCases.
    * testCase has two parts, a testDir and a testBatch.
    Ex:
        ./tests/pop3/apop/             (testDir)
        ./tests/pop3/apop.bat          (testBatch)
    * testDir holds the script that does the appropriate test job.
    * testBatch is a shell script, which calls the testScript in an
      unique way, and specify weather axigen configuration will be
      reseted or not, through the axi_fi=yes(no) option

    Ex: after running this testCase, axigen state will be reseted.
        perl ./apop/apop.pl
        # axi_fi=yes

    * testScript is usually a perl file and tests only one aspect of the server.
      It uses home-grown modules for specific protocols like POP3, SMTP, CLI
    home-grown libraries for specific protocols like POP3, SMTP, CLI.
    Each testCase is organized after the protocol it tests,
    thus all testCases testing pop3 will be placed inside the directory
    ./tests/pop3/.

2. Utility Library
----------------------
        In writing a testScript, you could make use of the Utility Libraries
    (which are basically perl modules) to ease your work and concentrate
    on the more important stuff, rather than dealing with the language barriers.
    Another purpose of the Library is to use code reusage and to keep track
    of each step a testScript is doing.

        Each protocol has it's corespondent Perl module,
    such as POP3 has the library called Axigen::Pop3.
        The functions implemented in a module are a direct corespondent
    of protocols command.
    Ex:
        Pop3::User("userName") will send POP3 command "user userName",
        Pop3::Stat() will send "stat".
    The collection currently has four modules:
    Axigen::Smtp, Axigen::POP3, Axigen::Cli and Axigen::Utils.
    A better description of each of them is found in the ./engine/ directory

3. Test Driver
--------------

    A testDriver is a process who runs testCases and records the failures.
    The curent implemented solution described below :


                                                                        rsh->system/axigen
                                                                         /
                                                                        /
                              |- system( copy, rm, mkdir, exec)|       (remote)
                              |- refresh                    |         /
tesBot* runScript-----calls ->|- stop                       |->effects
                              |- start                      |         \
                              |- checkCore                  |       (local)
                              |- runScript                  |           \
                                                                         \
                                                                          system/axigen

Note:
    Testbot needs read access on /var/log/maillog. This can be achieved through two ways :
    either run testbot as root( not recomended ) or you can replace the umask
    in the syslog init script ( /etc/init.d/syslog ) from 077 to 0604, which would create
    maillog with read permission for others.
