/**
 *   \brief    The side that runs the scripts.
 *   \see      ptgenhost.c
 *   \author   Cristina Balan, Andrei Paduraru, Marius Negreanu
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 */
#include <libgen.h>

#include "util.h"
#include "testbot.h"
#include "rshdaemon.h"
#include "socket.h"



struct globals_s glob;          /* used to pass parameters around */
int recursiveFlag = 1;
int forceFlag = TRUE;

int
main( int argc, char *argv[] )
{
        extern char *optarg;
        extern int optind, optopt;

        tb_globalsInit( argc, argv );
        if( argc == 1 )
                tb_usage(  );
        tb_parseArgs( argc, argv );

        if( glob.act_as_daemon ) {
                if( glob.port == 0 ) {
                        fprintf( stderr, "You have to specify -P option\n" );
                        exit( -2 );
                }
                rshDaemon( glob.port );
                return 0;
        }

        tb_parseConf(  );
        tb_envInit(  );
        tb_checkTools( getenv( PT_TOOL ) );indent -br -brs -cdw -ce -l80 -nut -ncs -prs -nsai -npcs -nsaf -nsaw -i8 *.c

        if( !glob.test_dir ) {
                printf( "* testbot: Provide the test directory.\n" );
                tb_usage(  );
        }

        if( glob.test_type == TEST_LOCAL && glob.verbose == TRUE )
                        printf( "* testbot: Tests will be done LOCALLY.\n" );
        }
        if( glob.test_type == TEST_REMOTE ) {
                util_isEnv( PT_HOST );
                util_isEnv( PT_PORT );
                if( glob.verbose == TRUE )
                        printf( "* testbot: Tests will be done REMOTE.\n" );
        }
        tb_runTests( glob.test_dir );
        return 0;
}


static int
tb_parseArgs( int argc, char *argv[] )
{
        int c;
        while( ( c = getopt( argc, argv, "Dt:T:H:P:d:o:r:vVk" ) ) != -1 ) {
                switch ( c ) {
                case 'T':
                        glob.script_tout = atoi( optarg );
                case 't':
                        if( !strcasecmp( optarg, "remote" ) )
                                glob.test_type = TEST_REMOTE;
                        if( !strcasecmp( optarg, "local" ) )
                                glob.test_type = TEST_LOCAL;
                        if( !glob.test_type ) {
                                printf( "* testbot: ERR: Give valid context local/remote.\n" );
                                tb_usage(  );
                        }
                        setenv( PT_TTYPE, optarg, 1 );
                        break;
                case 'H':
                        glob.hostname = optarg;
                        setenv( PT_HOST, optarg, 1 );
                        break;
                case 'o':
                        if( !strcasecmp( optarg, "linux" ) ) {
                                glob.config_file = "_linux";
                                break;
                        }
                        if( !strcasecmp( optarg, "bsd" ) ) {
                                glob.config_file = "_bsd";
                                break;
                        }
                        if( !strcasecmp( optarg, "windows" ) ) {
                                glob.config_file = "_windows";
                                break;
                        }
                        fprintf( stderr,
                                 "* testbot: ERR: Provide a valid platform!\n" );
                        tb_usage(  );
                case 'P':
                        glob.port = atoi( optarg );     // fixme
                        setenv( PT_PORT, optarg, 1 );
                        break;
                case 'd':
                        glob.test_dir = optarg;
                        break;
                case 'r':
                        if( !strcasecmp( optarg, "y" ) ) {
                                glob.refresh = OPT_YES;
                                break;
                        }
                        if( !strcasecmp( optarg, "n" ) ) {
                                glob.refresh = OPT_NO;
                                break;
                        }
                        if( !strcasecmp( optarg, "a" ) ) {
                                glob.refresh = OPT_ASK;
                                break;
                        }
                        fprintf( stderr,
                                 "* testbot: ERR: invalid parameter '%s'\n\n",
                                 optarg );
                        tb_usage(  );
                case 'v':
                        printf( "TestBot version %s\n", VER );
                        exit( 0 );
                case 'V':
                        glob.verbose = TRUE;
                        break;
                case 'D':
                        glob.act_as_daemon = TRUE;
                }
        }
        return TRUE;
}

/*------------------------------------------------------*/
static void
tb_usage( void )
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
        printf( "\t-V                      Verbose.\n" );
        exit( -1 );
}

int
tb_globalsInit( int argc, char *argv[] )
{
        glob.port = 0;
        glob.test_type = TEST_UNSET;    // 0:not set. 1:local 2:remote
        glob.test_dir = NULL;
        glob.hostname = NULL;
        glob.argv = argv;
        glob.argc = argc;
        glob.platf[0] = "linux";
        glob.platf[1] = "bsd";
        glob.platf[2] = "windows";
        glob.config_file = "_linux";
        glob.refresh = OPT_YES;
        glob.verbose = FALSE;
        glob.act_as_daemon = FALSE;
        glob.script_tout = 20;
        getcwd( glob.testbot_path, PATH_MAX );
        if( !getcwd( glob.tmp_dir, PATH_MAX ) ) {
                perror( "Cant get current dir" );
        }
        return 0;
}


int
tb_envInit( void )
{
        /* build the PATH like /home/tools/bin:$PATH */
        util_isEnv( PT_TOOL );
        strcpy( glob.cur_path, getenv( PT_TOOL ) );
        strcat( glob.cur_path, ":" );
        strcat( glob.cur_path, getenv( "PATH" ) );
        setenv( "PATH", glob.cur_path, 1 );

        util_isEnv( PT_WORKDIR );
        util_isEnv( PT_TTYPE );
        util_isEnv( PT_START );
        util_isEnv( PT_TOOL );
        util_isEnv( PT_COREDIR );
        util_isEnv( PT_DBGDIR );
        util_isEnv( PT_CFGFILE );
        glob.pt_workDir = getenv( PT_WORKDIR );
        glob.pt_cfgFile = getenv( PT_CFGFILE );
        glob.pt_coreDir = getenv( PT_COREDIR );
        glob.pt_dbgDir = getenv( PT_DBGDIR );
        setenv( "PERLLIB", getenv( PT_TOOL ), 1 );
        tb_setErrorlog(  );
        return 0;
}

static int
tb_checkTools( const char *tools_path )
{
        char buf[PATH_MAX] = { 0 };
#define NB_TOOLS 5
        char *tools[NB_TOOLS] = { "rexec", "cp", "mkdir", "refresh", "rm" };
        struct stat s;
        int i, rc;

        for( i = 0; i < NB_TOOLS; ++i ) {
                sprintf( buf, "%s/%s", tools_path, tools[i] );
                rc = stat( buf, &s );
                if( rc == -1 ) {
                        fprintf( stderr, "Can't find '%s/%s' : %s\n",
                                 tools_path, tools[i], strerror( errno ) );
                        exit( -2 );
                }
        }
        return 0;
}

/*--------------------------------------------------------------*/
void
sig_handler( int sig )
{
        int status;
        switch ( sig ) {
        case SIGCHLD:
                while( waitpid( -1, &status, WNOHANG ) > 0 )
                        if( WIFEXITED( status ) && WEXITSTATUS( status ) == 69 )
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
tb_runBat( const char *bat_name )
{
        char f[PATH_MAX] = "./";

        strcat( f, bat_name );
        int pid = 0;
        int hasAlarm;
        char c;
        struct sigaction act;

        printf( "\n\n" );
        printf( "*-------------------------.\n" );
        printf( "* testbot: Running script : %s/%s\n", glob.tmp_dir, bat_name );
        printf( "*-------------------------'\n" );

        act.sa_handler = sig_handler;
        act.sa_flags = SA_NOCLDSTOP;
        sigaction( SIGCHLD, &act, 0 );

        if( ( pid = fork(  ) ) == 0 ) {
                int rc = 0;
                rc = system( f );
                exit( rc );
        } else if( pid > 0 ) {
                while( 1 ) {
                        sleep( glob.script_tout );
                        if( waitpid( -1, &hasAlarm, WNOHANG ) )
                                break;
                        do {
                                printf( "kill child?<y/n>" );
                                c = getchar(  );
                                while( getchar(  ) != '\n' );
                        } while( c != 'y' && c != 'n' );
                        if( c == 'y' ) {
                                kill( pid, 9 );
                                break;
                        }
                }
        } else
                perror( "fork() failed" );
        return 0;
}

/*--------------------------------------------------------------*/





static int
tb_fileAction( const char *fileName, struct stat *statbuf, void *junk )
{
        if( !util_endsWith( fileName, ".bat" ) )
                return TRUE;
        tb_ptRefresh( fileName );
//    ptStart();
        getcwd( glob.cur_dir, PATH_MAX );
        tb_setupTmp( fileName );
        chdir( glob.tmp_dir );
        tb_runBat( basename( ( char * )fileName ) );
        tb_checkCore( glob.pt_coreDir, glob.pt_dbgDir, glob.pt_workDir,
                      glob.pt_cfgFile, glob.dest_coreDir );
        tb_cleanupTmp(  );
        sleep( 2 );
        chdir( glob.cur_dir );

        return ( TRUE );
}


static int
tb_dirAction( const char *fileName, struct stat *statbuf, void *junk )
{
        return ( TRUE );
}


int
tb_runRecursive( const char *srcName )
{
        recursiveFlag = 1;
        forceFlag = TRUE;
        if( recursiveAction( srcName, recursiveFlag, FALSE,
                             TRUE, tb_fileAction, tb_dirAction,
                             NULL ) == FALSE ) {
                exit( FALSE );
        }
        return TRUE;
}


static int
tb_runTests( const char *dir )
{
        struct stat inf;
        int rc = 0;
        char fullPath[PATH_MAX] = { 0 };

        if( stat( dir, &inf ) < 0 ) {
                printf( "! testbot: ERR: The directory '%s' doesn't exist.\n",
                        dir );
                return 0;
        }
        getcwd( glob.cur_dir, PATH_MAX );
        rc = chdir( dir );
        if( rc ) {
                fprintf( stderr, "! testbot: Can't change to '%s' : %s\n", dir,
                         strerror( errno ) );
                exit( -2 );
        }
        //! @todo tb sa verific daca tests este local sau global path
        sprintf( fullPath, "%s/%s", glob.cur_dir, dir );
        tb_runRecursive( fullPath );
        return TRUE;
}


/*------------------------------------------------------------------*/



/** Copies source in /tmp/pid, to create a sandbox for the test
    \param source_file[in] An absolute path pointing to a bat file.
                           The existence of a directory with the same name is assumed.
 */

static int
tb_setupTmp( const char *source_bat )
{
        char cmd[PATH_MAX] = { 0 };
        char cpstr[PATH_MAX] = { 0 };
        int cod;
        char *p;

        sprintf( glob.tmp_dir, "/tmp/%d", getpid(  ) );
        cod = mkdir( glob.tmp_dir, 0755 );
        if( cod ) {
                fprintf( stderr,
                         "! testbot: ERR: Cannot make directory %s: %s\n",
                         glob.tmp_dir, strerror( errno ) );
                exit( -1 );
        }

        if( glob.verbose == TRUE )
                printf( "# testbot: Copying %s to '%s'\n", source_bat,
                        glob.tmp_dir );
        sprintf( cmd, "/bin/cp -R %s %s/", source_bat, glob.tmp_dir );
        system( cmd );          // TODO: replace this with a function

        //Find the dir coresponding to this bat
        strcpy( cpstr, source_bat );
        p = strrchr( cpstr, '.' );
        *p = '\0';
        sprintf( cmd, "/bin/cp -R %s %s/", cpstr, glob.tmp_dir );
        if( glob.verbose == TRUE )
                printf( "* testbot: Running '%s'\n", cmd );
        system( cmd );          // TODO: replace this with a functionpt_fresh

        return 0;
}



static int
tb_cleanupTmp( void )
{
        printf( "# testbot: Removing '%s'\n", glob.tmp_dir );
        my_rm( glob.tmp_dir );
        chdir( glob.cur_dir );  // cine e cur_dir
        return 0;
}



/** Search a file for pt_fi=y or pt_fi=n . */
static int
tb_parseBat( const char *filename )
{
        char line[MAX_LIN] = { 0 };
        char *str = 0;
        pcre *re = NULL;
        int erroffset;
        int ovector[30];
        int matches;
        const char *error = NULL;
        struct stat inf;
        re = pcre_compile( "#.*pt_fi\\s*=\\s*\"?([y|Y|n|N])\"?\\s+",
                           0, &error, &erroffset, NULL );
        if( !re ) {
                fprintf( stderr, "Can't compile regular expression\n" );
                exit( -2 );
        }
        stat( filename, &inf );
        if( ( inf.st_mode & S_IFMT ) == S_IFDIR ) {
                printf( "! testbot: %s filename is a directory\n", filename );
                exit( -2 );
        }

        FILE *f = fopen( filename, "r" );
        if( !f ) {
                fprintf( stderr, "%d Can't open file '%s' : %s\n", __LINE__,
                         filename, strerror( errno ) );
                exit( -2 );
        }
        printf( "# testbot: Scanning %s\n", filename );
        while( ( str = fgets( line, MAX_LIN - 1, f ) ) ) {
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



int
tb_checkCore( const char *core_srcDir, const char *dbg_srcDir,
              const char *pt_workDir, const char *pt_cfgFile,
              const char *crash_destDir )
{
        int rc = FALSE;

        if( glob.test_type == TEST_LOCAL )
                rc = util_checkCoreLocal( core_srcDir, dbg_srcDir, pt_workDir,
                                          pt_cfgFile, core_srcDir );

        if( glob.test_type == TEST_REMOTE )
                rc = tb_checkCoreRemote( core_srcDir, dbg_srcDir, pt_workDir,
                                         pt_cfgFile, core_srcDir );

        if( rc && !glob.act_as_daemon )
                system( "echo -e 'Tested Product droped CORE!'|wall" );
        return rc;
}


static int
tb_checkCoreRemote( const char *core_srcDir, const char *dbg_srcDir,
                    const char *pt_workDir, const char *pt_cfgFile,
                    const char *crash_destDir )
{
        char cmd[MAX_LIN] = { 0 };      // Max line should therefore be 3 times the length of PATH_MAX
        int rc = 0;
        int sock;

        sock = sock_connectTo( glob.hostname, glob.port );
        sprintf( cmd, "CHECKCORE %s %s %s %s %s",
                 core_srcDir, dbg_srcDir, pt_workDir, pt_cfgFile,
                 crash_destDir );
        sock_sendLine( sock, cmd );
        rc = sock_getStatus( sock );
        close( sock );
        if( rc ) {
                return TRUE;
        }
        return FALSE;
}



/** Restore the default `Product under test`(PT) state. */
int
tb_ptRefresh( const char *bat_file )
{
        int cod;
        int rc = 0;

        if( glob.refresh == OPT_NO )
                return 0;

        if( glob.refresh == OPT_ASK ) {
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
        if( cod == OPT_NO )
                return 0;

        /* We excluded all NO's, so we're left with the yes. */
        /* Try to restore the default ptgen state --groleo */
        rc = system( "refresh" );
        printf( "%d\n", rc );
        if( rc == -1 || WEXITSTATUS( rc ) != 0 ) {
                printf( "! testbot: ERR: Could not make refresh!\n" );
                exit( 1 );
        }

        return 0;
}




int
tb_isNumber( char *nr )
{
        int i;

        for( i = 0; i < strlen( nr ); ++i ) {
                if( !isdigit( nr[i] ) )
                        return 0;
        }
        return 1;
}



static int
tb_parseConf(  )
{
        char buf[MAX_LIN];
        pcre *re = NULL;
        int erroffset;
        int offsetv[30];        //! offset vector, pointing to the found matches
        int matches;
        const char *error = NULL, *line = NULL;
        char varName[MAX_LIN];
        char varVal[MAX_LIN];
        int len;
        char result[5000] = { 0 };

        FILE *f = fopen( glob.config_file, "r" );

        if( !f ) {
                printf( "! testbot: ERR: Could not find config file: %s: %s!\n", glob.config_file, strerror( errno ) );
                exit( 1 );
        }
        re = pcre_compile( "(\\w+)\\s*=\\s*\"(.+)\"", 0, &error, &erroffset,
                           NULL );


        while( ( line = fgets( buf, MAX_LIN, f ) ) ) {
                char *c = strchr( line, '#' );
                if( c )
                        continue;

                //! @todo pcre_dfa_exec might be slightly faster

                matches =
                        pcre_exec( re, NULL, line, strlen( line ), 0, 0,
                                   offsetv, 30 );
                if( matches < 3 )
                        continue;

                len = offsetv[3] - offsetv[2];
                memcpy( varName, line + offsetv[2], len );
                varName[len] = '\0';


                len = offsetv[5] - offsetv[4];
                memcpy( varVal, line + offsetv[4], len );
                varVal[len] = '\0';
                //! @todo expand any ${VARIABLE} found in varVal
                expand_env( varVal, varName, sizeof( result ) );

        }
        fclose( f );
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
        //char* tmpVar=NULL;
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
                if( glob.verbose == TRUE )
                        printf( "export $%s=%s\n", varName, str );

                return FALSE;
        }
        memcpy( tmp1, str + offsetv[0] + 2, offsetv[1] - offsetv[0] - 3 );
        strncpy( tmp2, str, offsetv[0] );
        strcat( tmp2, getenv( tmp1 ) );
        strcat( tmp2, str + offsetv[1] );
        //printf("[%s]--[%s]\n", tmp1, tmp2);
        setenv( varName, tmp2, 1 );
        if( glob.verbose == TRUE )
                printf( "export $%s=%s\n", varName, tmp2 );

        return TRUE;
}



static int
tb_setErrorlog(  )
{
        char rez[MAX_LIN] = "", pathname[PATH_MAX] = "";
        char *host;
        char *defhost = "localhost";

        host = getenv( PT_HOST );
        if( !host )
                host = defhost;

        if( getcwd( pathname, PATH_MAX ) == NULL ) {
                printf( "! testbot: ERR: getting current dirrectory\n" );
                return 1;
        }

        sprintf( rez, "%s/errors/error.log.%s-%d", pathname, host, getpid(  ) );
        printf( "# testbot: log is: %s\n", rez );
        return setenv( "pt_errorlog", rez, 1 );
}
