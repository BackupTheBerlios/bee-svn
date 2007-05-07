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
 *   \brief  execute a given parameter, either on local machine, or remote
 */
#include "config.h"
#include "dbg.h"
#include "sock.h"
#include "sut.h"
#include "strop.h"
#include "fileop.h"

static int  rexec_parseArgs( int argc, char *argv[] );
struct      config_s cfg;

extern char *optarg;
extern int  optind;

static bool
rexec_remote( char *host, int port, char *c )
{
    int     ret, sockfd;
    char    *cmd=NULL;

    cmd = (char*)malloc( strlen(c) + 10);
    sockfd = sock_connectTo( host, port );
    if(sockfd==-1)
        return false;
    sprintf( cmd, "EXECUTE %s", c );
    dbg_verbose( "[%s]\n", cmd );
    sock_sendLine( sockfd, cmd );
    ret = sock_getStatus( sockfd );
    if( ret < 0 )
    {   dbg_error( "rexec: No remote confirmation!\n");
        free(cmd);
        return false;
    }

    if( ret > 0 )
    {   dbg_error("rexec: Error: '%s'\n", strerror( ret ) );
        dbg_error("rexec: Can't execute [%s].\n", c );
        free(cmd);
        shutdown( sockfd, 2);
        close( sockfd );
        return false;
    }
    free(cmd);
    shutdown( sockfd, 2);
    close( sockfd );
    return true;
}


int main( int argc, char *argv[] )
{
    char    *host = 0, *ttype = 0;
    int     port = 0;
    int     ret = -1;
    int     verbose = 1;

    if( argc < 2 )
    {   dbg_error( "rexec: missing operand\n" );
        printf( "Try `rexec -h` for more information.\n" );
        exit( EXIT_FAILURE);
    }

    DBG("rexec.debug");
    rexec_parseArgs( argc, argv );
    if( signal( SIGINT, sut_sigint ) == SIG_ERR )
    {   perror( "E: rexec: Signal error!" );
        UNDBG;
        exit( EXIT_FAILURE);
    }

    str_isEnv( SUT_TTYPE );
    ttype = getenv( SUT_TTYPE );

    if( !strcasecmp(ttype, "local") )
    {   ret = system( argv[optind] );
        UNDBG;
        exit( WEXITSTATUS( ret ) );
    } else if( !strcasecmp( ttype, "remote" ) )
    {   str_isEnv( SUT_HOST );
        str_isEnv( SUT_PORT );
        host = getenv( SUT_HOST );
        port = atoi( getenv( SUT_PORT ) );
        ret  = rexec_remote( host, port, argv[optind] );
        UNDBG;
        ret?exit(EXIT_SUCCESS): exit(EXIT_FAILURE);
    } else
        printf( "E: rexec: Invalid test type $SUT_TTYPE\n" );
    UNDBG;
    exit(EXIT_FAILURE);
}



void
rexec_usage( int status )
{
    printf( "Usage: rexec [OPTION] COMMAND...\n" );
    printf( "Send a COMMAND to the seraph daemon\n" );
    printf( "\n" );
    printf( "  -V, --verbose     print a message for each action executed\n" );
    printf( "  -h, --help        display this help and exit\n" );
    printf( "  -H host           Host to connect to\n" );
    printf( "  -P port           Port\n" );
    printf( "  -t testType\n" );
    exit( status );
}



static int
rexec_parseArgs( int argc, char *argv[] )
{
    int     c;
    while( ( c = getopt( argc, argv, "t:H:P:hV" ) ) != -1 )
    {   switch ( c ) {
            case 't':
                if( !strcasecmp( optarg, "remote" )
                || (!strcasecmp( optarg, "local" ))
                  )
                    setenv( SUT_TTYPE, optarg, 1 );
                break;
            case 'H':
                setenv( SUT_HOST, optarg, 1 );
                break;
            case 'P':
                setenv( SUT_PORT, optarg, 1 );
                break;
            case 'h':
                UNDBG;
                rexec_usage( EXIT_SUCCESS );
                break;
            case 'V':
                cfg.verbose=true;
                break;
        }
    }
    return true;
}
