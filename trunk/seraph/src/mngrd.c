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

struct config_s cfg;
extern char *optarg;
extern int optind;


int main( int argc, char *argv[] )
{
        DBG;
        if( argc == 1 )
                mngrd_usage( EXIT_FAILURE );

        /*1. fill in the config with default values */
        mngrd_initCfg( &cfg, argc, argv );

        /*2. export axi_ttype axi_host axi_port specified as -t, -H, -P */
        mngrd_parseArgs( &cfg, argc, argv );

        if( cfg.start_xmlrpc) {
            /*3. export any variable from config file */
            //mngrd_parseCfg( cfg.config_file );

            /*4. check if the needed variables are exported */
            //mngrd_initEnv( &cfg );
            /*5. check if the proper tools are installed */
            start_xmlrpc(cfg.port);
            return 0;
        }

        UNDBG;
        return 0;
}

void mngrd_usage( int status )
{
        printf( "Usage: seraph [<options> ...]\n\n" );
        printf( "  -R|--rawrpc                      Start RAWRPC service.\n" );
        printf( "  -d|--directory <name>            Run only the tests in directory 'name'\n" );
        printf( "  -J|--jabber                      Start Jabber remote control service.\n" );
        printf( "  -U|--jabberuser                  \n" );
        printf( "  -M|--mail <name(s)>              Whom to mail the results to\n" );
        printf( "  -V|--verbose                     Emit verbose output.\n" );
        printf( "  -V|--version                     Print version and exit\n" );
        printf( "  -X|--xmlrpc  <port>              Start XMLRPC service on 'port'\n" );
        printf( "  -C|--config <file>               Use 'file' as seraph config\n" );
        printf( "  -h|--help                        Print this text and exit\n" );
        exit( status );
}



int mngrd_parseArgs( struct config_s *cfg, int argc, char *argv[] )
{
        int c;
        while( ( c = getopt( argc, argv, "RX:C:d:hvV" ) ) != -1 ) {
                switch ( c ) {
                case 'h':
                        mngrd_usage( EXIT_SUCCESS );
                        break;
                case 'C':
                        cfg->config_file = optarg;
                        break;
                case 'X':
                        cfg->start_xmlrpc = true;
                        cfg->port = atoi( optarg );
                        break;
                case 'R':
                        cfg->start_rawrpc = true;
                        break;
                case 'd':
                        cfg->test_dir = optarg;
                        break;
                case 'v':
                        printf( "seraph version %s\n", VER );
                        exit( EXIT_SUCCESS );
                        break;
                case 'V':
                        cfg->verbose = TRUE;
                        break;
                }
        }
        return TRUE;
}



int mngrd_initEnv( struct config_s *config )
{
        char *st = 0;
        char *path = 0;
        /* Is the environment well set ? */
        str_isEnv( config->verbose, SUT_TOOL );
        str_isEnv( config->verbose, SUT_STOP );
        str_isEnv( config->verbose, SUT_START );
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
        //mngrd_setErrorlog(  );

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
int mngrd_free( struct config_s *config )
{
        free( config->cur_path );
        return 0;
}

int mngrd_initCfg( struct config_s *config, int argc, char *argv[] )
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



