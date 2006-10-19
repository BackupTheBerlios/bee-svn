/**
 *   \brief    The side that runs the scripts.
 *   \see      ptgenhost.c
 *   \author   Cristina Balan, Andrei Paduraru, Marius Negreanu
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 */
#include <libgen.h>
#include <wait.h>
#include <limits.h>
#include "config.h"
#include "testbot.h"
#include "rshd.h"
#include "socket.h"
#include "strop.h"
#include "fileop.h"
#include "sut.h"

extern struct config_s cfg ;
extern char *optarg;
extern int optind ;


static int  tb_ptRefresh(int option, const char *filename);
static int  tb_checkCore(int test_type, const char* core_srcDir, const char* dbg_srcDir, const char* axi_workDir,
                         const char* axi_cfgFile, const char* crash_destDir ) ;
static int  tb_cleanupTmp(char* tmpDir) ;
static int  tb_setupTmp( const char *source_bat, char* tmpDir) ;
static int  tb_parseBat(const char *filename) ;
static int  tb_runBat(const char *bat_name, int timeout);
static int         tb_runRecursive(const char *srcName) ;
static int  tb_dirAction(const char *fileName, struct stat *statbuf,  void *junk) ;
static int  tb_fileAction(const char *fileName, struct stat *statbuf, void *junk);
static int  tb_setErrorlog(void) ;



void
tb_usage( int status )
{
        printf( "TestBot version %s\n", VER );
        printf( "USAGE: testbot [options...]\n" );
        printf( "\t-d tests_dir            The directory containing the test scripts\n" );
        printf( "\t-D                      Act as daemon( rshd )\n" );
        printf( "\t-H host                 Host on which rshd runs.\n" );
        printf( "\t-o remote_OS            The OS that runs on the remote machine\n" );
        printf( "\t-P port                 Port on which rshd listens.\n" );
        printf( "\t-r refresh              Restore server's state after each testRun ?\n" );
        printf( "\t                        (y)es, (n)o, (a)sk.[default yes]\n" );
        printf( "\t-t test_type            Local or remote\n" );
        printf( "\t-T timeout              The timeout until trying to kill the child script\n" );
        printf( "\t-k                      Allways kill the child after timeout\n" );
        printf( "\t-v                      Print version and exit\n" );
        printf( "\t-h                      Print this text and exit\n");
        printf( "\t-V                      Verbose.\n" );
        exit( status );
}



int
tb_parseArgs( struct config_s* cfg, int argc, char *argv[] )
{
        int c;
        while( ( c = getopt( argc, argv, "Dt:T:H:P:d:o:r:hvVk" ) ) != -1 ) {
                switch ( c ) {
                case 'h':
                        tb_usage( EXIT_SUCCESS ) ;
                        break ;
                case 'T':
                        cfg->script_tout = atoi( optarg );
                        break ;
                case 't':
                        if( !strcasecmp( optarg, "remote" ) )
                                cfg->test_type = TEST_REMOTE;
                        if( !strcasecmp( optarg, "local" ) )
                                cfg->test_type = TEST_LOCAL;
                        if( !cfg->test_type ) {
                                printf( "! testbot: Give valid context local/remote.\n" );
                                tb_usage( EXIT_FAILURE );
                        }
                        setenv( SUT_TTYPE, optarg, 1 );
                        break;
                case 'k':
                        cfg->allways_kill = 1;
                        break;
                case 'H':
                        cfg->hostname = optarg;
                        setenv( SUT_HOST, optarg, 1 );
                        break;
                case 'o':
                        if( !strcasecmp( optarg, "linux" ) ) {
                                cfg->config_file = "_linux";
                                break;
                        }
                        if( !strcasecmp( optarg, "bsd" ) ) {
                                cfg->config_file = "_bsd";
                                break;
                        }
                        if( !strcasecmp( optarg, "windows" ) ) {
                                cfg->config_file = "_windows";
                                break;
                        }
                        fprintf( stderr, "! testbot: Invalid -o param [%s]\n",
                                 optarg);
                        tb_usage( EXIT_FAILURE );
                case 'P':
                        cfg->port = atoi( optarg );     /* FIXME: invalid param*/
                        setenv( SUT_PORT, optarg, 1 );
                        break;
                case 'd':
                        cfg->test_dir = optarg;
                        break;
                case 'r':
                        if( !strcasecmp( optarg, "y" ) ) {
                                cfg->refresh = OPT_YES;
                                break;
                        }
                        if( !strcasecmp( optarg, "n" ) ) {
                                cfg->refresh = OPT_NO;
                                break;
                        }
                        if( !strcasecmp( optarg, "a" ) ) {
                                cfg->refresh = OPT_ASK;
                                break;
                        }
                        fprintf( stderr, "! testbot: Invalid -r param [%s]\n",
                                 optarg );
                        tb_usage( EXIT_FAILURE );
                case 'v':
                        printf( "TestBot version %s\n", VER );
                        exit( EXIT_SUCCESS );
                case 'V':
                        cfg->verbose = TRUE;
                        break;
                case 'D':
                        cfg->act_as_daemon = TRUE;
                        break;
                }
        }
        return TRUE;
}



int
tb_envInit( struct config_s* cfg )
{

        /* Is the environment well set ? */
        str_isEnv( cfg->verbose, SUT_TOOL );
        str_isEnv( cfg->verbose, SUT_TOOL );
        str_isEnv( cfg->verbose, SUT_STOP );
        str_isEnv( cfg->verbose, SUT_START );
        str_isEnv( cfg->verbose, SUT_TTYPE );
        str_isEnv( cfg->verbose, SUT_DBGDIR );
        str_isEnv( cfg->verbose, SUT_SYSLOG );
        str_isEnv( cfg->verbose, SUT_COREDIR );
        str_isEnv( cfg->verbose, SUT_CFGFILE );
        str_isEnv( cfg->verbose, SUT_WORKDIR );

        /* build the PATH like $HOME/testbot/tools:$PATH
         * so the system() will use our cp, rm, mkdir */
        strcpy( cfg->cur_path, getenv( SUT_TOOL ) );
        strcat( cfg->cur_path, ":" );
        strcat( cfg->cur_path, getenv( "PATH" ) );
        setenv( "PATH", cfg->cur_path, 1 );

        setenv( "PERLLIB", getenv( SUT_TOOL ), 1 );
        tb_setErrorlog(  );
        return 0;
}



int
tb_cfgInit( struct config_s* cfg, int argc, char *argv[] )
{
        cfg->port = 0;
        cfg->test_type   = TEST_UNSET;    /* 0:not set. 1:local 2:remote*/
        cfg->test_dir    = NULL;
        cfg->hostname    = NULL;
        cfg->argv        = argv;
        cfg->argc        = argc;
        cfg->axi_syslog  = "/var/log/maillog" ;
        cfg->platf[0]    = "linux";
        cfg->platf[1]    = "bsd";
        cfg->platf[2]    = "windows";
        cfg->config_file = "_linux";
        cfg->refresh     = OPT_YES;
        cfg->verbose     = false ;
        cfg->script_tout = 20;
        cfg->allways_kill= false ;
        cfg->axi_workDir = getenv( SUT_WORKDIR );
        cfg->axi_cfgFile = getenv( SUT_CFGFILE );
        cfg->axi_coreDir = getenv( SUT_COREDIR );
        cfg->axi_dbgDir  = getenv( SUT_DBGDIR );
        cfg->axi_syslog  = getenv( SUT_SYSLOG ) ;
        cfg->act_as_daemon = false ;

        getcwd( cfg->testbot_path, FILENAME_MAX );
        if( !getcwd( cfg->tmp_dir, FILENAME_MAX ) ) {
                perror( "Cant get current dir" );
        }
        return 0;
}



int
tb_cfgParse( char* config_file )
{
        pcre    *re = NULL;
        char    buf[LINE_MAX];
        int     erroffset, matches, len ;
        int     offsetv[30];        /*! offset vector, pointing to the found matches*/
        char    varName[LINE_MAX], varVal[LINE_MAX];
        char    result[5000] = { 0 };
        const char *error = NULL, *line = NULL;

        FILE *f = fopen( config_file, "r" );

        if( !f ) {
                printf( "! testbot: Can't open config [%s]: %s\n",
                        config_file, strerror( errno ) );
                exit( EXIT_FAILURE );
        }
        re = pcre_compile( "(\\w+)\\s*=\\s*\"(.+)\"", 0, &error, &erroffset,
                           NULL );

        while( ( line = fgets( buf, LINE_MAX, f ) ) ) {
                char *c = strchr( line, '#' );
                if( c ) continue;

                /*! @todo pcre_dfa_exec might be slightly faster */

                matches = pcre_exec( re, NULL, line, strlen( line ), 0, 0,
                                   offsetv, 30 );
                if( matches < 3 ) continue;

                len = offsetv[3] - offsetv[2];
                memcpy( varName, line + offsetv[2], len );
                varName[len] = '\0';

                len = offsetv[5] - offsetv[4];
                memcpy( varVal, line + offsetv[4], len );
                varVal[len] = '\0';
                expand_env( varVal, varName, sizeof( result ) );

        }
        fclose( f );
        return TRUE ;
}



int
tb_checkTools( const char *tools_path )
{
        char buf[FILENAME_MAX] = { 0 };
        #define NB_TOOLS 7
        char *tools[NB_TOOLS] = { "rexec", "cp", "mkdir", "refresh", "rm",  "start", "stop" };
        struct stat s;
        int i, rc;

        for( i = 0; i < NB_TOOLS; ++i ) {
                sprintf( buf, "%s/%s", tools_path, tools[i] );
                rc = stat( buf, &s );
                if( rc == -1 ) {
                        fprintf( stderr, "! testbot: Can't find tool [%s/%s] : %s\n",
                                 tools_path, tools[i], strerror( errno ) );
                        exit( EXIT_FAILURE );
                }
        }
        return 0;
}



int
tb_runTests( const char *dir )
{
        struct stat inf;
        int rc = 0;
        char fullPath[FILENAME_MAX] = { 0 };
        char curDir[FILENAME_MAX] = { 0 } ;

        if( stat( dir, &inf ) < 0 ) {
                printf( "! testbot: The directory [%s] doesn't exist.\n",
                        dir );
                return 0;
        }
        getcwd( curDir, FILENAME_MAX );
        rc = chdir( dir );
        if( rc ) {
                fprintf( stderr, "! testbot: Can't change to [%s] : %s\n", dir,
                         strerror( errno ) );
                exit( EXIT_FAILURE );
        }
        /*! @todo verific daca tests este local sau global path*/
        sprintf( fullPath, "%s/%s", curDir, dir );
        tb_runRecursive( fullPath );
        return TRUE;
}



void
sig_handler( int sig )
{
        int status;
        switch ( sig ) {
        case SIGCHLD:
                while( waitpid( -1, &status, WNOHANG ) > 0 )
                        if( WEXITSTATUS( status ) == 69 )
                                printf( "* testbot: PASS\n" );
                        else
                                printf( "* testbot: FAIL\n" );
                break;
        case SIGALRM:
                fprintf( stderr, "timeout\n" );
                break;
        }
}



static int
tb_runBat( const char *bat_name , int timeout )
{
        char c ;
        int pid = 0, hasAlarm;
        struct sigaction act ;
        printf( "\n\n" );
        printf( "*-------------------------.\n" );
        printf( "* testbot: Running script : %s\n", bat_name );
        printf( "*-------------------------'\n" );

        act.sa_handler = sig_handler;
        act.sa_flags = SA_NOCLDSTOP;
        sigaction( SIGCHLD, &act, 0 );

        if( ( pid = fork(  ) ) == 0 ) {
                int  rc = 0;
                char f[FILENAME_MAX] = "./" ;
                strcat( f, bat_name );
                rc = system( f );
                exit( rc );
        } else if( pid > 0 ) {
                while( 1 ) {
                        sleep( cfg.script_tout );
                        if( waitpid( -1, &hasAlarm, WNOHANG ) )
                                break;
                        if( 1 ) {
                                kill( pid, 9 );
                                break;
                        }

                        do {
                                printf( "* testbot: KILL Child?  <y/n>" );
                                c = getchar(  );
                                while( getchar(  ) != '\n' );
                        } while( c != 'y' && c != 'n' );
                        if( c == 'y' ) {
                                kill( pid, 9 );
                                break;
                        }
                }
        } else
                perror( "! testbot: fork() failed" );
        return 0;
}



static int
tb_fileAction( const char *fileName, struct stat *statbuf, void *junk )
{
        char curDir[FILENAME_MAX] = { 0 } ;
        char tmpDir[FILENAME_MAX] = { 0 } ;

        if( !str_endsWith( fileName, ".bat" ) )
                return TRUE;
        tb_ptRefresh( cfg.refresh, fileName );
        tb_setupTmp( fileName, tmpDir );
        getcwd( curDir, FILENAME_MAX );

        chdir( tmpDir );
        tb_runBat( basename( ( char * )fileName ), cfg.script_tout );
        tb_checkCore( cfg.test_type,   cfg.axi_coreDir, cfg.axi_dbgDir
                    , cfg.axi_workDir, cfg.axi_cfgFile, cfg.dest_coreDir );
        tb_cleanupTmp( tmpDir );
        sleep( 1 );
        chdir( curDir );

        return ( TRUE );
}



static int
tb_dirAction( const char *fileName, struct stat *statbuf, void *junk )
{
        return ( TRUE );
}



static int
tb_runRecursive( const char *srcName )
{
        if( recursiveAction( srcName, 1, FALSE,
                             TRUE, tb_fileAction, tb_dirAction,
                             NULL ) == FALSE ) {
                exit( EXIT_FAILURE );
        }
        return TRUE;
}



/*
 * Copies source_bat in /tmp/PID, to create a sandbox for the test.
 * \param source_file[in] An absolute path pointing to a bat file.
 * \warn  The existence of a directory with the same name is assumed.
 * \todo  Implement a sut_refresh function
 */
static int
tb_setupTmp( const char *source_bat, char* tmpDir )
{
        char cmd[FILENAME_MAX] = { 0 };
        char cpstr[FILENAME_MAX] = { 0 };
        int cod;
        char *p;

        sprintf( tmpDir, "/tmp/%d", getpid(  ) );
        cod = mkdir( tmpDir, 0755 );
        if( cod ) {
                fprintf( stderr, "! testbot: Cant make directory [%s]: %s\n",
                         tmpDir, strerror( errno ) );
                exit( EXIT_FAILURE );
        }

        if( cfg.verbose == TRUE )
                printf( "# testbot: Copying %s to '%s'\n", source_bat,
                        tmpDir );
        sprintf( cmd, "/bin/cp -R %s %s/", source_bat, tmpDir );
        system( cmd );

        /* Find the dir coresponding to this bat */
        strcpy( cpstr, source_bat );
        p = strrchr( cpstr, '.' );
        *p = '\0';
        strcpy(cfg.cur_test, cpstr );
        sprintf( cmd, "/bin/cp -R %s %s/", cpstr, tmpDir );
        if( cfg.verbose == TRUE )
                printf( "* testbot: Running '%s'\n", cmd );
        system( cmd );

        return 0;
}



static int
tb_cleanupTmp( char* tmpDir )
{
        printf( "# testbot: Removing '%s'\n", tmpDir );
        fop_rm( tmpDir );
        return 0;
}



/** Search a file for axi_fi=y or axi_fi=n . */
static int
tb_parseBat( const char *filename )
{
        char line[LINE_MAX] = { 0 };
        char *str = 0;
        pcre *re = NULL;
        int erroffset;
        int ovector[30];
        int matches;
        const char *error = NULL;
        struct stat inf;
        FILE* f=NULL ;
        re = pcre_compile( "#.*axi_fi\\s*=\\s*\"?([y|Y|n|N])\"?\\s+",
                           0, &error, &erroffset, NULL );
        if( !re ) {
                fprintf( stderr, "Can't compile regular expression\n" );
                exit( EXIT_FAILURE );
        }
        stat( filename, &inf );
        if( ( inf.st_mode & S_IFMT ) == S_IFDIR ) {
                printf( "! testbot: [%s] is a directory\n", filename );
                exit( EXIT_FAILURE );
        }

        f = fopen( filename, "r" );
        if( !f ) {
                fprintf( stderr, "! testbot: %d Can't open file [%s] : %s\n", __LINE__,
                         filename, strerror( errno ) );
                exit( EXIT_FAILURE );
        }
        printf( "# testbot: Scanning [%s]\n", filename );
        while( ( str = fgets( line, LINE_MAX - 1, f ) ) ) {
                ;
                matches =
                        pcre_exec( re, NULL, str, strlen( str ), 0, 0, ovector,
                                   30 );
                if( matches >= 2 ) {
                        if( tolower( str[ovector[2]] ) == 'y' ) {
                                fclose( f );
                                return OPT_YES;
                        }
                        if( tolower( str[ovector[2]] ) == 'n' ) {
                                fclose( f );
                                return OPT_NO;
                        }
                }
        }
        fclose( f );
        return OPT_YES;         /* If nothing is found, return the default
                                   which is OPT_YES. */
}



/*This has alot of params. pls review?*/
static int
tb_checkCore( int test_type,
              const char *core_srcDir, const char *dbg_srcDir,
              const char *axi_workDir, const char *axi_cfgFile,
              const char *crash_destDir )
{
        bool rc = false ;
        rc = sut_checkCore( test_type, core_srcDir, dbg_srcDir, axi_workDir,
                                          axi_cfgFile, core_srcDir );
        if( rc && !cfg.act_as_daemon)
                wall( "****  SUT dropped CORE ****\r\n", 0, 0 ) ;
        return rc;
}



/*
 * Restore the default configuration of the SUT.
 * SUT = server under test */
static int
tb_ptRefresh( int refresh, const char *bat_file )
{
        int cod;
        int rc = 0;

        if( refresh == OPT_NO ) return 0;

        if( refresh == OPT_ASK ) {
                char r = 0;

                do {
                        printf( "* testbot: Reinstall default configuration?[y/n]:" );
                        r = getchar(  );
                        while( getchar(  ) != '\n' );
                } while( r != 'y' && r != 'n' );
                if( r == 'n' ) {
                        printf( "# testbot: Keeping current state\n" );
                        return 0;
                }
        }

        cod = tb_parseBat( bat_file );
        if( cod == OPT_NO ) return 0;

        /* We excluded all NO's, so we're left with the yes. */
        /* Try to restore the default ptgen state --groleo */
        rc = system( "refresh" );
        printf( "# testbot: refresh %d\n", rc );
        if( rc == -1 || WEXITSTATUS( rc ) != 0 ) {
                printf( "! testbot: Can't make refresh!\n" );
                exit( EXIT_FAILURE );
        }

        return 0;
}



/**
 * \brief search and expand any occurence in str, of ${VARNAME}
 * \brief placing the result in result
 * \todo how should i know the length of the memory allocated in result ?provide another param
 * UGLY HACK ( to be revised in future) */
int
expand_env( char *str, char *varName, int maxLen )
{
        pcre *re = NULL;
        int offsetv[30];
        int matches = 0;
        /*char* tmpVar=NULL;*/
        char tmp1[4096] = { 0 };
        char tmp2[4096] = { 0 };
        const char *error = NULL;
        int erroffset;
        re = pcre_compile( "\\$\\{(.+)\\}", 0, &error, &erroffset, NULL );
        if( !re ) {
                printf( "error\n" );
                return FALSE;
        }
        matches = pcre_exec( re, NULL, str, strlen( str ), 0, 0, offsetv, 30 );
        if( matches < 2 ) {
                setenv( varName, str, 1 );
                if( 1 == TRUE )
                        printf( "export $%s=%s\n", varName, str );

                return FALSE;
        }
        memcpy( tmp1, str + offsetv[0] + 2, offsetv[1] - offsetv[0] - 3 );
        strncpy( tmp2, str, offsetv[0] );
        strcat( tmp2, getenv( tmp1 ) );
        strcat( tmp2, str + offsetv[1] );
        /*printf("[%s]--[%s]\n", tmp1, tmp2);*/
        setenv( varName, tmp2, 1 );
        if( 1 == TRUE )
                printf( "export $%s=%s\n", varName, tmp2 );

        return TRUE;
}



/*
 * Export axi_errorlog=$CWD/errors/$HOST-$PID
 */
static int
tb_setErrorlog(  )
{
        char rez[LINE_MAX] = "";

        sprintf( rez, "%s/errors/%s-%d",
                 cfg.testbot_path, cfg.hostname, getpid(  ) );
        printf( "# testbot: Log is: %s\n", rez );
        return setenv( "axi_errorlog", rez, 1 );
}
