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

#include <strings.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/wait.h>
#include <limits.h>
#include <time.h>

#include "seraph.h"
#include "dbg.h"
#include "sock.h"
#include "strop.h"
#include "fileop.h"
#include "rpc.h"
#include "sut.h"
#include "wall.h"
#include "core.h"

extern struct config_s cfg;
extern char *optarg;
extern int optind;

static int srph_checkTools( const char *tools_path );

struct config_s cfg;

static int onLineParsed(const char *name, const char *value, void* arg)
{
    char* tmp;
    tmp=core_expandVars(value);
    debug("%s = %s\n", name, tmp);
    setenv(name, tmp,1);
    free(tmp);
    return 1;
}

int main( int argc, char *argv[] )
{
    bool ret=false;
    DBG("srph.debug");

    if( argc == 1 )
        srph_usage( EXIT_FAILURE );

    /*1. fill in the config with default values */
    srph_initCfg( &cfg, argc, argv );

    /*2. export axi_ttype axi_host axi_port specified as -t, -H, -P */
    srph_parseArgs( &cfg, argc, argv );

    /*3. export any variable from config file */
    cfg.takeAction = onLineParsed;
    scan_parseCfg( cfg.config_file, (void*)1);

    if( !cfg.test_dir ) {
        printf( "! seraph: Provide the test directory.\n" );
        srph_usage( EXIT_FAILURE );
    }

    /*4. check if the needed variables are exported */
    ret = srph_initEnv( &cfg );
    if(!ret)
    {
        UNDBG;
        exit(EXIT_FAILURE);
    }

    /*5. check if the proper tools are installed */
    srph_checkTools( getenv( SUT_TOOL ) );

    if( cfg.test_type == TEST_LOCAL ) {
        dbg_verbose( "* seraph: Tests will be done LOCALLY.\n" );
    }
    if( cfg.test_type == TEST_REMOTE ) {
        str_isEnv( SUT_HOST );
        str_isEnv( SUT_PORT );
        dbg_verbose( "* seraph: Tests will be done REMOTE.\n" );
    }
    core_runTests( cfg.test_dir );
    srph_free( &cfg );
    UNDBG;
    exit(EXIT_SUCCESS);
}

void srph_usage( int status )
{
    printf( "Usage: seraph [<options> ...]\n\n" );
    printf( "  -H|--host <name>                 Host on which RAWRPC runs.\n" );
    printf( "  -P|--port <number>               Port on which RAWRPC listens.\n" );
    printf( "  -I|--ignore <name>               Ignore tests in directory 'name'\n" );
    printf( "  -d|--directory <name>            Run only the tests in directory 'name'\n" );
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
    int     c;
    while( ( c = getopt( argc, argv, "C:S:t:T:H:P:d:r:hvVk" ) ) != -1 ) {
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
                printf("I: Using [%s] as config\n", optarg);
                cfg->config_file = optarg;
                break;
            case 'R':
                cfg->start_rawrpc = true;
                break;
            case 'H':
                cfg->hostname = optarg;
                setenv( SUT_HOST, optarg, 1 );
                break;
            case 'P':
                cfg->rawport = atoi( optarg );     /* FIXME: invalid param */
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
                cfg->verbose = true;
                break;
        }
    }
    return true;
}



int srph_initEnv( struct config_s *config )
{
    char *st = 0;
    char *path = 0;
    bool ret=false;

    /* Is the environment well set ? */
    str_isEnv( SUT_STOP );
    str_isEnv( SUT_START );
    str_isEnv( SUT_TTYPE );
    str_isEnv( SUT_DBGDIR );
    str_isEnv( SUT_SYSLOG );
    str_isEnv( SUT_COREDIR );
    str_isEnv( SUT_CFGFILE );
    str_isEnv( SUT_WORKDIR );
    if( config->refresh == OPT_YES)
    {   str_isEnv( SUT_DEFWD );
        config->sutDefWD = getenv(SUT_DEFWD);
    }

    /* build the PATH like $HOME/seraph/tools:$PATH
     * so the system() will use our cp, rm, mkdir */
    st = getenv( SUT_TOOL );
    if(!st || st[0] == '\0') { st = LIBDIR ; setenv(SUT_TOOL, LIBDIR, 1); }
    path = getenv( "PATH" );

    config->cur_path = calloc( 1, strlen( st ) + strlen( path ) + 2 );
    sprintf( config->cur_path, "%s:%s", st, path );
    setenv( "PATH", config->cur_path, 1 );

    setenv( "PERLLIB", getenv( SUT_TOOL ), 1 );
    setenv( "PERL5LIB", getenv( SUT_TOOL ), 1 );
    ret = core_setErrorlog( "user1", config->test_dir ); /* TODO:hardcoded username */
    if(!ret)
        return false;

    config->axi_workDir = getenv( SUT_WORKDIR );
    config->axi_cfgFile = getenv( SUT_CFGFILE );
    config->axi_coreDir = getenv( SUT_COREDIR );
    config->axi_dbgDir = getenv( SUT_DBGDIR );
    config->axi_syslog = getenv( SUT_SYSLOG );
    config->rawport = getenv(SUT_PORT)!=NULL?atoi(getenv(SUT_PORT)):0;
    config->hostname = getenv(SUT_HOST)!=NULL?getenv(SUT_HOST):"localhost";
    return true;
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
    config->test_type = TEST_UNSET; /* 0:not set. 1:local 2:remote */
    config->test_dir = NULL;
    config->argv = argv;
    config->argc = argc;
    config->axi_syslog = "/var/log/maillog";
    config->start_rawrpc = false ;
    config->start_xmlrpc = false ;
    config->start_jabber = false ;
    config->config_file = NULL;
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


static int srph_checkTools( const char *tools_path )
{
    char buf[FILENAME_MAX] = { 0 };
#define NB_TOOLS 7
    char *tools[NB_TOOLS] =
    { "rexec", "cp", "mkdir", "refresh", "rm", "start", "stop" };
    struct stat s;
    int i, rc;

    if( !tools_path ) {
        dbg_verbose("Using default path to the tools");
        tools_path = LIBDIR ;
    }
    for( i = 0; i < NB_TOOLS; ++i ) {
        sprintf( buf, "%s/%s", tools_path, tools[i] );
        rc = stat( buf, &s );
        if( rc == -1 ) {
            dbg_error( "Can't find tool [%s/%s] : [%s]\n",
                    tools_path, tools[i], strerror( errno ) );
            exit( EXIT_FAILURE );
        }
    }
    return 0;
}
