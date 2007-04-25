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
 *   \brief    Wraps around mkdir cmd.
 */
#include <strings.h>
#include "config.h"
#include "socket.h"
#include "strop.h"
#include "sut.h"



struct      config_s cfg;
static int  mkdir_parseArgs( int argc, char *argv[] );



static bool
client_mkdir( char *host, int port, char *path )
{
    char    *cmd=NULL;
    int     ret = 0, sockfd = -1;

    cmd = (char*)malloc(strlen(path)+8);
    sockfd = sock_connectTo( host, port);
    sprintf( cmd, "MKDIR %s", path);
    sock_sendLine( sockfd, cmd);
    ret = sock_getStatus( sockfd);
    if( ret )
    {   fprintf( stderr,
                "E: mkdir: Could not create directory [%s]: %s\n", path, strerror(ret) );
        shutdown( sockfd, 2);
        close( sockfd);
        free( cmd);
        return false;
    }
    free( cmd);
    shutdown( sockfd, 2);
    close( sockfd );
    return true;
}



int
main( int argc, char *argv[] )
{
    char    *tc=NULL, *host=NULL;
    int     port=0;
    bool    ret;

    if( argc < 2 )
    {   printf( "E: mkdir: missing operand\n" );
        printf( "Try `mkdir -h` for more information.\n" );
        exit(EXIT_FAILURE);
    }

    DBG("mkdir.debug");
    mkdir_parseArgs( argc, argv );
    debug("1\n");
    str_isEnv( SUT_TTYPE );
    debug("2\n");
    tc = getenv( SUT_TTYPE );

    if( !strcmp(tc, "local") )
    {   char *cmd = (char*)malloc( strlen(argv[optind]) + 8);
        sprintf( cmd, "mkdir %s", argv[optind]);
        ret = system( cmd);
        free( cmd);
        UNDBG;
        exit( ret);
    } else if( !strcmp(tc, "remote") )
    {   str_isEnv( SUT_HOST);
        str_isEnv( SUT_PORT);
        host = getenv( SUT_HOST);
        port = atoi( getenv(SUT_PORT) );
        ret = client_mkdir( host, port, argv[optind]);
        UNDBG;
        ret? exit(EXIT_SUCCESS): exit(EXIT_FAILURE);
    } else
        printf( "E: mkdir: Invalid $axi_ttype\n" );
    UNDBG;
    exit(EXIT_FAILURE);
}



void
mkdir_usage( void )
{
    printf( "Usage: mkdir [OPTION] COMMAND...\n" );
    printf( "Create the DIRECTORY, if they do not already exist.\n" );
    printf( "\n" );
    printf( "  -v, --verbose     print a message for each action executed\n" );
    printf( "  -h, --help        display this help and exit\n" );
    printf( "  -H hostname\n" );
    printf( "  -P port\n" );
    printf( "  -t testType\n" );
    exit( 0 );
}



static int
mkdir_parseArgs( int argc, char *argv[] )
{
    int c;
    while( ( c = getopt( argc, argv, "t:H:P:hv" ) ) != -1 )
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
                mkdir_usage( );
                break;
            case 'v':
                cfg.verbose = true;
                break;
        }
    }
    return true;
}
