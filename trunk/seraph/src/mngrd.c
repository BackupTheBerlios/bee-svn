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
#include "config.h"
#include "rpc.h"
#include "dbg.h"
#include "seraph.h"
#include "sock.h"
#include "strop.h"
#include "fileop.h"
#include "sut.h"

struct config_s cfg;
extern char *optarg;
extern int  optind;

static void mngrd_usage( int status );
static int  mngrd_freeCfg( struct config_s *c );
static int  mngrd_initCfg( struct config_s *c, int argc, char *argv[] );
static int  mngrd_initEnv( struct config_s *c );
static int  mngrd_parseArgs( struct config_s *cfg, int argc, char *argv[] );

int main( int argc, char *argv[] )
{
    DBG("mngrd.debug");
    if( argc == 1 ) {
        mngrd_usage( EXIT_FAILURE );
        UNDBG;
    }

    /*1. initialize config with default values */
    mngrd_initCfg( &cfg, argc, argv );

    /*2. parse program arguments */
    mngrd_parseArgs( &cfg, argc, argv );

    /* Start xmlrpc */
    if( cfg.startXmlRPC) {
        start_xmlrpc(cfg.xmlport);
        UNDBG;
        return 0;
    }
    if( cfg.startRawRPC) {
        start_rawrpc(cfg.rawport);
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
    printf( "  -v|--verbose                 Emit verbose output.\n" );
    printf( "  -V|--version                 Print version and exit\n" );
    printf( "  -C|--config <file>           Use 'file' as seraph config\n" );
    printf( "  -h|--help                    Print this text and exit\n" );
    exit( status );
}



int mngrd_parseArgs( struct config_s *cfg, int argc, char *argv[] )
{
    int     c;
    while( ( c = getopt( argc, argv, "R:X:M:C:d:J:hvV" ) ) != -1 ) {
        switch ( c ) {
            case 'h':
                mngrd_usage( EXIT_SUCCESS );
                break;
            case 'C':
                cfg->configFile = optarg;
                break;
            case 'X':
                cfg->startXmlRPC = true;
                cfg->xmlport = atoi( optarg );
                break;
#if 0
/* Not yet implemented */
            case 'J':
                cfg->start_jabber = true ;
                cfg->jabuser = optarg ;
                break;
#endif
            case 'M':
                cfg->notifyMail = optarg;
                break;
            case 'R':
                cfg->startRawRPC = true;
                cfg->rawport = atoi( optarg );
                break;
            case 'd':
                cfg->testDir = optarg;
                break;
            case 'V':
                printf( "seraph version %s\n", VERSION );
                exit( EXIT_SUCCESS );
                break;
            case 'v':
                cfg->verbose = true;
                setenv( SUT_VERBOSE, "true",1);
                break;
            case '?':
                if (isprint (optopt))
                    {dbg_error("Unknown option `-%c'.\n", optopt);}
                else
                    dbg_error("Unknown option character `\\x%x'.\n", optopt);
                mngrd_usage( EXIT_FAILURE);
                break;
           default:
                mngrd_usage( EXIT_FAILURE);
                break;
        }
    }
    return true;
}



int mngrd_initCfg( struct config_s *c, int argc, char *argv[] )
{
    c->argv = argv;
    c->argc = argc;
    c->testDir  = NULL;
    c->rawport  = 0;
    c->xmlport  = 0;
    c->hostname = "localhost";
    c->testType = TEST_UNSET;
    c->maillog  = "/var/log/maillog";
    c->refresh  = OPT_YES;
    c->verbose  = false;
#warn FREE
    c->children = malloc(sizeof(GSList) );
    c->startRawRPC  = false ;
    c->startXmlRPC  = false ;
    c->startJabber  = false ;
    c->configFile   = "_linux";
    c->scriptTout   = 20;
    c->allwaysKill  = false;
    c->behaviour    = TB_BE_TESTER;
    *(c->children)  = NULL;
#if 0
/* Never used */
    getcwd( c->seraph_path, FILENAME_MAX );
    if( !getcwd( c->tmp_dir, FILENAME_MAX ) ) {
        dbg_error( "seraph: Can't get current directory: %s",strerror(errno) );
        exit(EXIT_FAILURE);
    }
#endif
    return 0;
}

/*
 * Free allocated memory
 */
int mngrd_freeCfg( struct config_s *c )
{
    free( c->oldPATH );
    return 0;
}


int mngrd_initEnv( struct config_s *c )
{
    char *st = 0;
    char *path = 0;

    /* Is the environment well set ? */
    str_isEnv( SUT_TOOL );
    str_isEnv( SUT_STOP );
    str_isEnv( SUT_START );
    str_isEnv( SUT_DBGDIR );
    str_isEnv( SUT_SYSLOG );
    str_isEnv( SUT_COREDIR );
    str_isEnv( SUT_CFGFILE );
    str_isEnv( SUT_WORKDIR );
    str_isEnv( SUT_DESTCOREDIR );
    /* build the PATH like $HOME/seraph/tools:$PATH
     * so the system() will use our cp, rm, mkdir */
    st = getenv( SUT_TOOL );
    path = getenv( "PATH" );

    c->oldPATH = calloc( 1, strlen( st ) + strlen( path ) + 2 );
    sprintf( c->oldPATH, "%s:%s", st, path );
    setenv( "PATH", c->oldPATH, 1 );

    setenv( "PERLLIB", getenv( SUT_TOOL ), 1 );
    setenv( "PERL5LIB", getenv( SUT_TOOL ), 1 );

    c->maillog     = getenv( SUT_SYSLOG );
    c->sutDbgDir   = getenv( SUT_DBGDIR );
    c->sutCoreDir  = getenv( SUT_COREDIR );
    c->sutCfgFile  = getenv( SUT_CFGFILE );
    c->sutWorkDir  = getenv( SUT_WORKDIR );
    c->destCoreDir = getenv( SUT_DESTCOREDIR);
    return 0;
}



