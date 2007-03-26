/**
 *   \brief    The side that runs the scripts.
 *   \see      ptgenhost.c
 *   \author   Cristina Balan, Andrei Paduraru, Marius Negreanu
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 */
#include <libgen.h>
#include <sys/wait.h>
#include <limits.h>
#include <time.h>
#include "debug.h"
#include "seraph.h"
#include "rshd.h"
#include "socket.h"
#include "strop.h"
#include "fileop.h"
#include "sut.h"
#include "wall.h"

extern struct config_s cfg;
extern char *optarg;
extern int optind;


static int srph_runBat( const char *bat_name, int timeout );
static int srph_setupTmp( char const *source_bat, char *tmpDir );
static int srph_parseBat( const char *filename );
static int srph_dirAction( const char *fileName, struct stat *statbuf,
                         void *junk );
static int srph_checkCore( int test_type, const char *core_srcDir,
                         const char *dbg_srcDir, const char *axi_workDir,
                         const char *axi_cfgFile, const char *crash_destDir );
static int srph_fileAction( const char *fileName, struct stat *statbuf,
                          void *junk );
static int srph_sutRefresh( int option, const char *filename );
static int srph_cleanupTmp( char *tmpDir );
static int srph_setErrorlog( void );
static int srph_runRecursive( const char *srcName );


void srph_usage( int status )
{
        printf( "Usage: seraph [<options> ...]\n\n" );
        printf( "  -D|--daemon                      Act as daemon.\n" );
        printf( "  -R|--rawrpc                      Start RAWRPC service.\n" );
        printf( "  -H|--host <name>                 Host on which RAWRPC runs.\n" );
        printf( "  -P|--port <number>               Port on which RAWRPC listens.\n" );
        printf( "  -I|--ignore <name>               Ignore tests in directory 'name'\n" );
        printf( "  -d|--directory <name>            Run only the tests in directory 'name'\n" );
        printf( "  -J|--jabber                      Start Jabber remote control service.\n" );
        printf( "  -U|--jabberuser                  \n" );
        printf( "  -M|--mail <name(s)>              Whom to mail the results to\n" );
        printf( "  -S|--setup <machine>             Install seraph on <machine>\n\t\tEx:machine openbsd#user@192.168.x.y:/home/userX/seraph\n" );
        printf( "  -T|--timeout <number>            Timeout until child is killed\n" );
        printf( "  -V|--verbose                     Emit verbose output.\n" );
        printf( "  -V|--version                     Print version and exit\n" );
        printf( "  -X|--xmlrpc  <port>              Start XMLRPC service on 'port'\n" );
        printf( "  -c|--config <file>               Use 'file' as seraph config\n" );
        printf( "  -h|--help                        Print this text and exit\n" );
        printf( "  -k|--kill                        Kill child after timeout\n" );
        printf( "  -o <OS>                          OS running on remote machine\n" );
        printf( "  -r <choice>                      Restore SUT state after each testRun ?\n(y)es, (n)o, (a)sk.[yes]\n" );
        printf( "  -t <type>                        Test type: 'local' or 'remote'\n" );
        exit( status );
}



int srph_parseArgs( struct config_s *cfg, int argc, char *argv[] )
{
        int c;
        while( ( c = getopt( argc, argv, "DRX:C:S:t:T:H:P:d:o:r:hvVk" ) ) != -1 ) {
                switch ( c ) {
                case 'h':
                        srph_usage( EXIT_SUCCESS );
                        break;
                case 'T':
                        cfg->script_tout = atoi( optarg );
                        break;
                case 't':
                        if( !strcasecmp( optarg, "remote" ) )
                                cfg->test_type = TEST_REMOTE;
                        if( !strcasecmp( optarg, "local" ) )
                                cfg->test_type = TEST_LOCAL;
                        if( !cfg->test_type ) {
                                debug( "! seraph: Invalid test_type.\n" );
                                srph_usage( EXIT_FAILURE );
                        }
                        setenv( SUT_TTYPE, optarg, 1 );
                        break;
                case 'k':
                        cfg->allways_kill = 1;
                        break;
                case 'S':
                        cfg->behaviour = TB_BE_SETUP;
                        cfg->machine = optarg;
                        break;
                case 'C':
                        cfg->config_file = optarg;
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
                        if( !strcasecmp( optarg, "solaris" ) ) {
                                cfg->config_file = "_solaris";
                                break;
                        }
                        fprintf( stderr, "! seraph: Invalid -o param [%s]\n",
                                 optarg );
                        srph_usage( EXIT_FAILURE );
                        break;
                case 'X':
                        cfg->start_xmlrpc = true;
                        cfg->port = atoi( optarg );
                        break;
                case 'R':
                        cfg->start_rawrpc = true;
                        break;
                case 'H':
                        cfg->hostname = optarg;
                        setenv( SUT_HOST, optarg, 1 );
                        break;
                case 'P':
                        cfg->port = atoi( optarg );     /* FIXME: invalid param */
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
                        fprintf( stderr,
                                 "! seraph: Invalid -r param [%s]\n", optarg );
                        srph_usage( EXIT_FAILURE );
                        break;
                case 'v':
                        printf( "seraph version %s\n", VER );
                        exit( EXIT_SUCCESS );
                        break;
                case 'V':
                        cfg->verbose = TRUE;
                        break;
                case 'D':
                        cfg->behaviour = TB_BE_DAEMON;
                        break;
                }
        }
        return TRUE;
}



int srph_initEnv( struct config_s *config )
{
        char *st = 0;
        char *path = 0;
        /* Is the environment well set ? */
        str_isEnv( config->verbose, SUT_TOOL );
        str_isEnv( config->verbose, SUT_STOP );
        str_isEnv( config->verbose, SUT_START );
        //str_isEnv( config->verbose, SUT_TTYPE );
        str_isEnv( config->verbose, SUT_DBGDIR );
        str_isEnv( config->verbose, SUT_SYSLOG );
        str_isEnv( config->verbose, SUT_COREDIR );
        str_isEnv( config->verbose, SUT_CFGFILE );
        str_isEnv( config->verbose, SUT_WORKDIR );

        /* build the PATH like $HOME/seraph/tools:$PATH
         * so the system() will use our cp, rm, mkdir */
        st = getenv( SUT_TOOL );
        path = getenv( "PATH" );

        config->cur_path = calloc( 1, strlen( st ) + strlen( path ) + 2 );
        sprintf( config->cur_path, "%s:%s", st, path );
        setenv( "PATH", config->cur_path, 1 );

        setenv( "PERLLIB", getenv( SUT_TOOL ), 1 );
        setenv( "PERL5LIB", getenv( SUT_TOOL ), 1 );
        srph_setErrorlog(  );

        config->axi_workDir = getenv( SUT_WORKDIR );
        config->axi_cfgFile = getenv( SUT_CFGFILE );
        config->axi_coreDir = getenv( SUT_COREDIR );
        config->axi_dbgDir = getenv( SUT_DBGDIR );
        config->axi_syslog = getenv( SUT_SYSLOG );
        return 0;
}

/*
 * Free allocated memory
 */
int srph_free( struct config_s *config )
{
        free( config->cur_path );
        return 0;
}

int srph_initCfg( struct config_s *config, int argc, char *argv[] )
{
        config->port = 0;
        config->test_type = TEST_UNSET; /* 0:not set. 1:local 2:remote */
        config->test_dir = NULL;
        config->hostname = "localhost";
        config->argv = argv;
        config->argc = argc;
        config->axi_syslog = "/var/log/maillog";
        config->start_rawrpc = false ;
        config->start_xmlrpc = false ;
        config->start_jabber = false ;
#if 0
        config->platf[0] = "linux";
        config->platf[1] = "bsd";
        config->platf[2] = "windows";
        config->platf[3] = "solaris";
#endif
        config->config_file = "_linux";
        config->refresh = OPT_YES;
        config->verbose = false;
        config->script_tout = 20;
        config->allways_kill = false;
        config->behaviour = TB_BE_TESTER;

        getcwd( config->seraph_path, FILENAME_MAX );
        if( !getcwd( config->tmp_dir, FILENAME_MAX ) ) {
                perror( "! seraph: Can't get current directory" );
        }
        return 0;
}


#if 0
int srph_parseCfg( char *config_file )
{
        pcre *re = NULL;
        char buf[LINE_MAX];
        int erroffset, matches, len;
        int offsetv[30];        /*! offset vector, pointing to the found matches */
        char varName[LINE_MAX], varVal[LINE_MAX];
        const char *error = NULL, *line = NULL;

        FILE *f = fopen( config_file, "r" );

        if( !f ) {
                fprintf( stderr, "! seraph: Can't open config [%s]: [%s]\n",
                         config_file, strerror( errno ) );
                exit( EXIT_FAILURE );
        }
        re = pcre_compile( "(\\w+)\\s*=\\s*\"(.+)\"", 0, &error, &erroffset,
                           NULL );

        while( ( line = fgets( buf, LINE_MAX, f ) ) ) {
                char *c = strchr( line, '#' );
                if( c )
                        continue;

                /*! @todo pcre_dfa_exec might be slightly faster */

                matches = pcre_exec( re, NULL, line, strlen( line ), 0, 0,
                                     offsetv, 30 );
                if( matches < 3 )
                        continue;

                len = offsetv[3] - offsetv[2];
                memcpy( varName, line + offsetv[2], len );
                varName[len] = '\0';

                len = offsetv[5] - offsetv[4];
                memcpy( varVal, line + offsetv[4], len );
                varVal[len] = '\0';
                expand_env( varVal, varName, sizeof( varName ) );
        }
        fclose( f );
        return TRUE;
}
#endif


int srph_checkTools( const char *tools_path )
{
        char buf[FILENAME_MAX] = { 0 };
#define NB_TOOLS 7
        char *tools[NB_TOOLS] =
            { "rexec", "cp", "mkdir", "refresh", "rm", "start", "stop" };
        struct stat s;
        int i, rc;

        for( i = 0; i < NB_TOOLS; ++i ) {
                sprintf( buf, "%s/%s", tools_path, tools[i] );
                rc = stat( buf, &s );
                if( rc == -1 ) {
                        fprintf( stderr,
                                 "chechTools: Can't find tool [%s/%s] : [%s]\n",
                                 tools_path, tools[i], strerror( errno ) );
                        exit( EXIT_FAILURE );
                }
        }
        return 0;
}



int srph_runTests( const char *dir )
{
        struct stat inf;
        char fullPath[FILENAME_MAX] = { 0 };
        char curDir[FILENAME_MAX] = { 0 };

        if( stat( dir, &inf ) < 0 ) {
                printf( "! seraph: The directory [%s] doesn't exist.\n", dir );
                return 0;
        }
        getcwd( curDir, FILENAME_MAX );
        if( -1 == chdir( dir ) ) {
                fprintf( stderr, "! seraph: Can't change to [%s] : [%s]\n",
                         dir, strerror( errno ) );
                exit( EXIT_FAILURE );
        }
        /*! @todo verify if `tests` dir is an absolute or a relative path */
        sprintf( fullPath, "%s/%s", curDir, dir );
        srph_runRecursive( fullPath );
        return TRUE;
}



void sig_handler( int sig )
{
        int status;
        switch ( sig ) {
        case SIGCHLD:
                while( waitpid( -1, &status, WNOHANG ) > 0 )
                        if( WEXITSTATUS( status ) == 69 )
                                printf( "* seraph: PASS\n" );
                        else
                                printf( "* seraph: FAIL [%d]\n", status );
                break;
        case SIGALRM:
                fprintf( stderr, "timeout\n" );
                break;
        }
}



static int srph_runBat( const char *bat_name, int timeout )
{
        char c;
        int pid = 0, hasAlarm;
        struct sigaction act;
        printf( "\n\n" );
        printf( "*-------------------------.\n" );
        printf( "* seraph: Running script : [%s]\n", bat_name );
        printf( "*-------------------------'\n" );

        act.sa_handler = sig_handler;
        act.sa_flags = SA_NOCLDSTOP;
        sigaction( SIGCHLD, &act, 0 );

        if( ( pid = fork(  ) ) == 0 ) {
                int rc = 0;
                rc = system( bat_name );
                exit( rc );
        } else if( pid > 0 ) {
                while( 1 ) {
                        sleep( cfg.script_tout );
                        if( waitpid( -1, &hasAlarm, WNOHANG ) )
                                break;
                        if( cfg.allways_kill ) {
                                kill( pid, 9 );
                                break;
                        }

                        do {
                                printf( "* seraph: KILL Child?  [y/n]: " );
                                c = getchar(  );
                                while( getchar(  ) != '\n' );
                        } while( c != 'y' && c != 'n' );
                        if( c == 'y' ) {
                                if( -1 == kill( pid, 9 ) ) {
                                        perror
                                            ( "! seraph: Unable to kill child script" );
                                }
                                break;
                        }
                }
        } else
                perror( "! seraph: fork() failed" );
        return 0;
}



static int
srph_fileAction( const char *fileName, struct stat *statbuf, void *junk )
{
        char curDir[FILENAME_MAX] = { 0 };
        char tmpDir[FILENAME_MAX] = { 0 };
        char bn[32] = { 0 };
        char *fn_dup = 0;
        char *len = 0;

        fn_dup = strdup( fileName );
        len = strncpy( bn, basename( fn_dup ), 31 );
        free( fn_dup );

        if( strcmp( bn, "runtest.bat" ) || access( fileName, X_OK ) )
                return TRUE;
        srph_sutRefresh( cfg.refresh, fileName );
        srph_setupTmp( fileName, tmpDir );
        if( NULL == getcwd( curDir, FILENAME_MAX ) ) {
                perror( "! seraph: Unable to get current directory" );
        }

        if( -1 == chdir( tmpDir ) ) {
                fprintf( stderr,
                         "! seraph: Unable to change directory to [%s]:[%s]\n",
                         tmpDir, strerror( errno ) );
                return FALSE;
        }
        srph_runBat( fileName, cfg.script_tout );
        srph_checkCore( cfg.test_type, cfg.axi_coreDir, cfg.axi_dbgDir,
                      cfg.axi_workDir, cfg.axi_cfgFile, cfg.dest_coreDir );
        srph_cleanupTmp( tmpDir );
        sleep( 1 );
        if( -1 == chdir( curDir ) ) {
                fprintf( stderr,
                         "! seraph: Unable to change directory to [%s]:[%s]\n",
                         curDir, strerror( errno ) );
                return FALSE;
        }

        return ( TRUE );
}



static int
srph_dirAction( const char *fileName, struct stat *statbuf, void *junk )
{
        return ( TRUE );
}



static int srph_runRecursive( const char *srcName )
{
        if( recursiveAction( srcName, 1, FALSE,
                             TRUE, srph_fileAction, srph_dirAction,
                             NULL ) == FALSE ) {
                printf( "error in runRecursive\n" );
                exit( EXIT_FAILURE );
        }
        return TRUE;
}



/*!
 * Copies source_bat in /tmp/PID, to create a sandbox for the test.
 * \param source_file[in] An absolute path pointing to a bat file.
 * \todo  Implement a sut_refresh function
 * \todo  Return Bool ??
 */
static int srph_setupTmp( char const *source_bat, char *tmpDir )
{
        char cmd[FILENAME_MAX] = { 0 };
        int status;
        char *s = strdup( source_bat );

        sprintf( tmpDir, "/tmp/%d", getpid(  ) );
        if( -1 == mkdir( tmpDir, 0755 ) ) {
                fprintf( stderr,
                         "! seraph: Can't make directory [%s]: [%s]\n",
                         tmpDir, strerror( errno ) );
                exit( EXIT_FAILURE );
        }
        if( cfg.verbose == TRUE )
                printf( "# seraph: Copying [%s] to [%s]\n", s, tmpDir );
        sprintf( cmd, "/bin/cp -R %s/* %s/", dirname( s ), tmpDir );

        status = system( cmd );
        free( s );
        s = strdup( source_bat );
        if( -1 == status || 0 != WEXITSTATUS( status ) ) {
                fprintf( stderr,
                         "! seraph: Unable to copy [%s] to [%s]: [%s]\n",
                         dirname( s ), tmpDir, strerror( errno ) );
        }
        free( s );
        return 0;
}



static int srph_cleanupTmp( char *tmpDir )
{
        printf( "# seraph: Removing [%s]\n", tmpDir );
        fop_rm( TEST_LOCAL, tmpDir, 0, 0 );
        return 0;
}



/** Search a file for axi_fi=y or axi_fi=n . */
static int srph_parseBat( const char *filename )
{
#if 0
        char line[LINE_MAX] = { 0 };
        char *str = 0;
        pcre *re = NULL;
        int erroffset;
        int ovector[30];
        int matches;
        const char *error = NULL;
        struct stat inf;
        FILE *f = NULL;
        re = pcre_compile( "#.*axi_fi\\s*=\\s*\"?([y|Y|n|N])\"?\\s+",
                           0, &error, &erroffset, NULL );
        if( !re ) {
                fprintf( stderr, "Can't compile regular expression\n" );
                exit( EXIT_FAILURE );
        }
        if( -1 == stat( filename, &inf ) ) {
                fprintf( stderr, "! seraph: Unable to stat [%s]: [%s]\n",
                         filename, strerror( errno ) );
                return OPT_YES;
        }
        if( ( inf.st_mode & S_IFMT ) == S_IFDIR ) {
                printf( "! seraph: [%s] is a directory\n", filename );
                exit( EXIT_FAILURE );
        }

        f = fopen( filename, "r" );
        if( !f ) {
                fprintf( stderr,
                         "! seraph: [%d] Can't open file [%s] : [%s]\n",
                         __LINE__, filename, strerror( errno ) );
                exit( EXIT_FAILURE );
        }
        printf( "# seraph: Scanning [%s]\n", filename );
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
#endif
        return 0;
}



/*This has alot of params. pls review?*/
static int
srph_checkCore( int test_type,
              const char *core_srcDir, const char *dbg_srcDir,
              const char *axi_workDir, const char *axi_cfgFile,
              const char *crash_destDir )
{
        bool rc = false;
        rc = sut_checkCore( test_type, core_srcDir, dbg_srcDir, axi_workDir,
                            axi_cfgFile, core_srcDir );
        if( rc && !cfg.behaviour == TB_BE_TESTER )
                wall( "****  SUT dropped CORE ****\r\n", 0, 0 );
        return rc;
}



/*
 * Restore the default configuration of the SUT.
 * SUT = server under test */
static int srph_sutRefresh( int refresh, const char *bat_file )
{
        int cod;
        int rc = 0;

        if( refresh == OPT_NO )
                return 0;

        if( refresh == OPT_ASK ) {
                char r = 0;

                do {
                        printf
                            ( "* seraph: Reinstall default configuration? [y/n]: " );
                        r = getchar(  );
                        while( getchar(  ) != '\n' );
                } while( r != 'y' && r != 'n' );
                if( r == 'n' ) {
                        printf( "# seraph: Keeping current state\n" );
                        return 0;
                }
        }

        cod = srph_parseBat( bat_file );
        if( cod == OPT_NO )
                return 0;

        /* We excluded all NO's, so we're left with the yes. */
        /* Try to restore the default ptgen state --groleo */
        debug("refresh\n");
        rc = system( "refresh" );
        debug( "# seraph: refresh retVal : %d\n", rc );
        if( rc == -1 || WEXITSTATUS( rc ) != 0 ) {
                debug( "! seraph: Can't make refresh!\n" );
                exit( EXIT_FAILURE );
        }

        return 0;
}



/**
 * \brief search and expand any occurence in str, of ${VARNAME} */
/* expand ${VAR} from prefix/${VAR}/suffix */
char *expand_vars( const char *t1 )
{
        char *p = 0, *s = 0, *val;
        int i = 0;
        char id[512] = { 0 };
        int sz_new = 0, sz_old = 1024;

        char *text = calloc( sz_old, sizeof( char ) );

        strncpy( text, t1, 1023 );

        for( p = text; *p != '\0'; p++ ) {
                /*printf("[%c][%d]\n", *p, *p ) ; */
                if( *p == '$' ) {
                        s = p++;        /* saving position of $ */
                        /*printf("[%c][%d]\n", *p, *p ) ; */
                        if( *p != '{' ) {
                                printf( "Error expanding %s\n", text );
                                return 0;
                        }

                        p++;
                        /*printf("[%c][%d]\n", *p, *p ) ; */
                        while( *p != '}' && *p != '\0' && i < 512 ) {
                                id[i++] = *p;
                                p++;
                                /*printf("[%c][%d]\n", *p, *p ) ; */
                        }
                        if( *p == '\0' ) {
                                printf( "Unmatched bracket in %s\n", text );
                                return 0;
                        }
                        p++;
                        if( NULL == getenv( id ) ) {
                                debug( "Can't expand [%s] : Not exported\n", id );
                                memset( id, 0, 512 );
                                i = 0;  /* Reset ID pointer and ID size counter */
                                continue;
                        }
                        val = strdup( p );
                        /* compute the length of the final string */
                        sz_new =
                            strlen( p ) + strlen( getenv( id ) ) +
                            strlen( val );
                        if( sz_new > sz_old ) {
                                text = realloc( text, sz_new );
                                sz_old = sz_new;
                        }
                        strcpy( s, getenv( id ) );      /* Push in the expansion */
                        strcpy( s + strlen( getenv( id ) ), val );      /*Copy suffix */
                        free( val );
                        p += strlen( getenv( id ) ) - strlen( id ) - 3; /* Advance the parsing pointer */
                        memset( id, 0, 512 );
                        i = 0;  /* Reset ID pointer and ID size counter */
                }
        }
        return text;
}



/*
 * Export axi_errorlog=$CWD/errors/$HOST-$PID
 */
static int srph_setErrorlog(  )
{
        char rez[LINE_MAX] = "";
        struct tm *t = 0;
        time_t now;
        char* dn[PATH_MAX]={0}, *p=0;

        time( &now );
        t = localtime( &now );
        sprintf( rez, "%s/errors/%s-%d-%02d:%02d:%02d",
                 cfg.seraph_path, cfg.hostname, getpid(  ),
                 t->tm_hour, t->tm_min, t->tm_sec );

        printf( "# seraph: Log is: [%s]\n", rez );
        setenv( SUT_ERRLOG, rez, 1 );

        /* Create ./errors in case it doesn't exists */
        strcpy(dn,rez);
        p = dirname(dn);
        if( -1 == access( p, F_OK ) ) {
                fprintf( stderr, "%s doesn't exist\n", dirname( rez ) );
                if( -1 == mkdir( p, 0777 ) ) {
                        fprintf( stderr, "SetErrorLog: Unable to create [%s]: [%s]\n",
                                 p,strerror( errno ) );
                        exit( EXIT_FAILURE );
                }
                fprintf( stderr, "%s created\n", p );
        }

        /* If it exists, but we dont have write permissions */
        if( -1 == access( p, W_OK ) ) {
                fprintf( stderr, "%s doesn't have write permissions\n", p );
                exit( EXIT_FAILURE );
        }
        return 0;
}
