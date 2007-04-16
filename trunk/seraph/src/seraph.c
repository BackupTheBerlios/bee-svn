/*
 * Copyright (C) 2006, 2007 Free Software Foundation, Inc.
 * Written by Negreanu Marius <groleo@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; see the file COPYING.  If not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include <stdlib.h>
#include <libgen.h>
#include <sys/wait.h>
#include <limits.h>
#include <time.h>

#include "seraph.h"
#include "rshd.h"
#include "debug.h"
#include "socket.h"
#include "strop.h"
#include "fileop.h"
//#include "rpc.h"
#include "sut.h"
#include "wall.h"

extern struct config_s cfg;
extern char *optarg;
extern int optind;
        extern int (*setenv)(const char *name, const char *value, int overwrite);



struct config_s cfg;

int main( int argc, char *argv[] )
{


        DBG;
        if( argc == 1 )
                srph_usage( EXIT_FAILURE );

        /*1. fill in the config with default values */
        srph_initCfg( &cfg, argc, argv );

        /*2. export axi_ttype axi_host axi_port specified as -t, -H, -P */
        srph_parseArgs( &cfg, argc, argv );
        switch ( cfg.behaviour ) {
        case TB_BE_DAEMON:
                if( cfg.start_rawrpc ) {
                        if(cfg.port == 0 ) {
                                fprintf( stderr,
                                                "! seraph: You have to specify -P option\n" );
                                exit( -2 );}
                        else
                        {
                                start_rawrpc( cfg.port );
                        }

                }                /*if( cfg.start_jabber)
                                   start_jabber();*/
                if( cfg.start_xmlrpc) {
        /*3. export any variable from config file */
       cfg.takeAction = setenv;
       srph_parseCfg( cfg.config_file,1 );

        /*4. check if the needed variables are exported */
        srph_initEnv( &cfg );
                        /*5. check if the proper tools are installed */
                        srph_checkTools( getenv( SUT_TOOL ) );
                        start_xmlrpc(cfg.port);
                }
                return 0;
        case TB_BE_SETUP:
                //setup(cfg.machine);
                return 0;
        }


        /*3. export any variable from config file */
        srph_parseCfg( cfg.config_file,1 );

        /*4. check if the needed variables are exported */
        srph_initEnv( &cfg );

        /*5. check if the proper tools are installed */
        srph_checkTools( getenv( SUT_TOOL ) );

        if( !cfg.test_dir ) {
                printf( "! seraph: Provide the test directory.\n" );
                srph_usage( EXIT_FAILURE );
        }

        if( cfg.test_type == TEST_LOCAL && cfg.verbose == TRUE ) {
                debug( "* seraph: Tests will be done LOCALLY.\n" );
        }
        if( cfg.test_type == TEST_REMOTE ) {
                str_isEnv( cfg.verbose, SUT_HOST );
                str_isEnv( cfg.verbose, SUT_PORT );
                if( cfg.verbose == TRUE )
                        debug( "* seraph: Tests will be done REMOTE.\n" );
        }
        sut_runTests( cfg.test_dir );
        srph_free( &cfg );
        UNDBG;
        return 0;
}

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
        printf( "  -C|--config <file>               Use 'file' as seraph config\n" );
        printf( "  -h|--help                        Print this text and exit\n" );
        printf( "  -k|--kill                        Kill child after timeout\n" );
        printf( "  -r <choice>                      Restore SUT state after each testRun ?\n(y)es, (n)o, (a)sk.[yes]\n" );
        printf( "  -t <type>                        Test type: 'local' or 'remote'\n" );
        exit( status );
}



int srph_parseArgs( struct config_s *cfg, int argc, char *argv[] )
{
        int c;
        while( ( c = getopt( argc, argv, "DRX:C:S:t:T:H:P:d:r:hvVk" ) ) != -1 ) {
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
//        srph_setErrorlog(  );

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
        char buf[PATH_MAX];
        int erroffset, matches, len;
        int offsetv[30];        /*! offset vector, pointing to the found matches */
        char varName[PATH_MAX], varVal[PATH_MAX];
        const char *error = NULL, *line = NULL;

        FILE *f = fopen( config_file, "r" );

        if( !f ) {
                fprintf( stderr, "! seraph: Can't open config [%s]: [%s]\n",
                         config_file, strerror( errno ) );
                exit( EXIT_FAILURE );
        }
        re = pcre_compile( "(\\w+)\\s*=\\s*\"(.+)\"", 0, &error, &erroffset,
                           NULL );

        while( ( line = fgets( buf, PATH_MAX, f ) ) ) {
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
