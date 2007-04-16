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

/**
 *   \brief    The side that runs the scripts.
 *   \see      ptgenhost.c
 *   \author   Marius Negreanu
 *   \date     Thu Aug 17 17:38:13 2006
 *
 */
#include <libgen.h>
#include <sys/wait.h>
#include <limits.h>
#include <time.h>
#include "rpc.h"
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
extern int  optind;

static void mngrd_usage( int status );
static int  mngrd_freeCfg( struct config_s *config );
static int  mngrd_initCfg( struct config_s *config, int argc, char *argv[] );
static int  mngrd_initEnv( struct config_s *config );
static int  mngrd_parseArgs( struct config_s *cfg, int argc, char *argv[] );

int main( int argc, char *argv[] )
{
    DBG;
    if( argc == 1 )
        mngrd_usage( EXIT_FAILURE );

    /*1. initialize config with default values */
    mngrd_initCfg( &cfg, argc, argv );

    /*2. parse program arguments */
    mngrd_parseArgs( &cfg, argc, argv );

    /* Start xmlrpc */
    if( cfg.start_xmlrpc) {
        start_xmlrpc(cfg.port);
        UNDBG;
        return 0;
    }

    UNDBG;
    return 0;
}



void mngrd_usage( int status )
{
    printf( "Usage: seraph [<options> ...]\n\n" );
    printf( "  -R|--rawrpc <port>           Start RAWRPC service on 'port'\n" );
    printf( "  -X|--xmlrpc  <port>          Start XMLRPC service on 'port'\n" );
    printf( "  -J|--jabber <username>       Start Jabber remote control service.\n" );
    printf( "  -d|--directory <name>        Run only the tests in directory 'name'\n" );
    printf( "  -M|--mail <name(s)>          Whom to mail the results to.\n" );
    printf( "  -V|--verbose                 Emit verbose output.\n" );
    printf( "  -V|--version                 Print version and exit\n" );
    printf( "  -C|--config <file>           Use 'file' as seraph config\n" );
    printf( "  -h|--help                    Print this text and exit\n" );
    exit( status );
}



int mngrd_parseArgs( struct config_s *cfg, int argc, char *argv[] )
{
    int c;
    while( ( c = getopt( argc, argv, "R:X:M:C:d:J:hvV" ) ) != -1 ) {
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
                case 'J':
                    cfg->start_jabber = true ;
                cfg->jabuser = optarg ;
                break;
                case 'M':
                    cfg->notifyMail = optarg;
                break;
            case 'R':
                cfg->start_rawrpc = true;
                cfg->port = atoi( optarg );
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



int mngrd_initCfg( struct config_s *c, int argc, char *argv[] )
{
    c->argv = argv;
    c->argc = argc;
    c->test_dir = NULL;
    c->port = 0;
    c->hostname = "localhost";
    c->start_rawrpc = false ;
    c->start_xmlrpc = false ;
    c->start_jabber = false ;
    c->test_type = TEST_UNSET;
    c->axi_syslog = "/var/log/maillog";
    c->config_file = "_linux";
    c->refresh = OPT_YES;
    c->verbose = false;
    c->script_tout = 20;
    c->allways_kill = false;
    c->behaviour = TB_BE_TESTER;
    getcwd( c->seraph_path, FILENAME_MAX );
    if( !getcwd( c->tmp_dir, FILENAME_MAX ) ) {
        debug( "! seraph: Can't get current directory: %s",strerror(errno) );
        exit(EXIT_FAILURE);
    }
    return 0;
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
int mngrd_freeCfg( struct config_s *config )
{
    free( config->cur_path );
    return 0;
}

